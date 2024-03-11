#include "bignum.h"
#include "shell_output.h"

#include <iostream>

using dpext::cout;
using std::endl;
using std::hex;
using std::dec;


static byte degreesOf2[8] = { 1, 2, 4, 8, 16, 32, 64, 128 };

static char hexDigits[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

void showHex(const byte * bytes, unsigned int bytesLength)
{
	cout << endl << hex;

	for (unsigned int b = 0; b < bytesLength; b++)
	{
		if (b)
			cout << " ";

		cout << (bytes[b] < 16 ? "0" : "") << int(bytes[b]);
	}

	cout << dec << endl;
}

void primary_clear(byte * bytes, unsigned int length)
{
	for (unsigned int b = 0; b < length; b++)
		bytes[b] = 0;
}

bool primary_copy(const byte * source, unsigned int source_length, byte * dist, unsigned int dist_length)
{
	bool flag = false;

	if (source_length <= dist_length)
	{
		for (unsigned int b = 0; b < source_length; b++)
			dist[dist_length - 1 - b] = source[source_length - 1 - b];

		flag = true;
	}

	return flag;
}

bool primary_first_more_second(const byte * first, unsigned int first_length, const byte * second, unsigned int second_length)
{
	bool flag = false;

	if (first_length && second_length)
	{
		unsigned int b, delta;

		if (first_length > second_length)
		{
			delta = first_length - second_length;

			for (b = 0; b < delta; b++)
			{
				if (first[b])
				{
					flag = true;

					break;
				}
			}

			if (!flag)
			{
				for (b = 0; b < second_length; b++)
				{
					if (first[b + delta] != second[b])
					{						
						flag = first[b + delta] > second[b];

						break;
					}
				}
			}
		}
		else if (first_length < second_length)
		{
			bool secondmore = false;

			delta = second_length - first_length;

			for (b = 0; b < delta; b++)
			{
				if (second[b])
				{
					secondmore = true;

					break;
				}
			}

			if (!secondmore)
			{
				for (b = 0; b < first_length; b++)
				{
					if (first[b] != second[b + delta])
					{						
						flag = first[b] > second[b + delta];

						break;
					}
				}
			}
		}	
		else
		{			
			for (b = 0; b < first_length; b++)
			{
				if (first[b] != second[b])
				{
					flag = first[b] > second[b];

					break;
				}
			}
		}
	}
	else if (first_length)
	{
		for (unsigned int b = 0; b < first_length; b++)
		{
			if (first[b])
			{
				flag = true;
				break;
			}
		}
	}

	return flag;
}

bool primary_first_equal_second(const byte * first, unsigned int first_length, const byte * second, unsigned int second_length)
{
	bool flag = true;

	unsigned int minlength, maxlength, delta, b;	
	const byte * min, * max;

	if (first_length < second_length)
	{
		min = first;
		minlength = first_length;

		max = second;
		maxlength = second_length;
	}
	else
	{
		max = first;
		maxlength = first_length;

		min = second;
		minlength = second_length;
	}

	delta = maxlength - minlength;

	for (b = 0; b < delta; b++)
	{
		if (max[b])
		{
			flag = false;

			break;
		}
	}

	if (flag)
	{
		for (b = 0; b < minlength; b++)
		{
			if (min[b] != max[delta + b])
			{
				flag = false;

				break;
			}
		}
	}

	return flag;
}

void primary_summation(const byte * input1, unsigned int input1_length, const byte * input2, unsigned int input2_length, byte * output, unsigned int output_length)
{
	unsigned short op;

	bool carry = false;

	for (unsigned int b = 0; b < output_length; b++)
	{
		op = b < input1_length ? input1[input1_length - 1 - b] : 0;

		if (b < input2_length)
			op += input2[input2_length - 1 - b];

		if (carry)
			op++;

		carry = op > 255;

		output[output_length - 1 - b] = op & 255;
	}
}

void primary_subtraction(const byte * original, unsigned int original_length, const byte * subtrahend, unsigned int subtrahend_length, byte * output, unsigned int output_length)
{
	unsigned short op;

	bool carry = false;

	for (unsigned int b = 0; b < output_length; b++)
	{
		op = b < original_length ? original[original_length - 1 - b] : 0;

		if (carry)
		{
			if (op > 0)
			{
				op--;
				carry = false;
			}
			else
			{
				op = 255;
				carry = true;
			}
		}

		if (b < subtrahend_length)
		{
			if (op < subtrahend[subtrahend_length - 1 - b])
			{
				op += 256;
				carry = true;
			}

			op -= subtrahend[subtrahend_length - 1 - b];
		}

		output[output_length - 1 - b] = op & 255;
	}
}

void primary_bitshifting_down(const byte * input, unsigned int input_length, unsigned int bits, byte * output, unsigned int output_length)
{
	unsigned int bytes_shift = bits / 8;
	unsigned int bits_shifting = bits % 8;

	unsigned short op;

	unsigned int b;

	for (b = 0; (b < input_length - bytes_shift) && (b < output_length); b++)
	{
		op = 0;

		if (input_length - 1 - bytes_shift - b > 0)
			op = input[input_length - 2 - bytes_shift - b] << 8;

		op += input[input_length - 1 - bytes_shift - b];

		op >>= bits_shifting;

		output[output_length - 1 - b] = op & 255;
	}

	for (b = 0; b < bytes_shift; b++)
	{
		if (b < output_length)
			output[b] = 0;
	}
}

void primary_bitshifting_up(const byte * input, unsigned int input_length, unsigned int bits, byte * output, unsigned int output_length)
{
	unsigned int bytes_shift = bits / 8;
	unsigned int bits_shifting = bits % 8;

	unsigned short op;

	unsigned int b;

	for (b = 0; b < input_length; b++)
	{
		if (!b && input[b] && (output_length >= input_length + bytes_shift + 1))
		{
			op = input[b] << bits_shifting;
			op >>= 8;
			output[output_length - input_length - bytes_shift - 1] = op & 255;
		}

		if ((output_length + b >= input_length + bytes_shift) && (b < input_length + bytes_shift))
		{
			op = input[b] << 8;

			if (b < (input_length - 1))
				op += input[b + 1];

			op <<= bits_shifting;

			op >>= 8;

			output[output_length - input_length - bytes_shift + b] = op & 255;
		}
	}

	for (b = 0; b < bytes_shift; b++)
	{
		if (b < output_length)
			output[output_length - 1 - b] = 0;
	}
}

bool primary_multiplication(const byte * input1, unsigned int input1_length, const byte * input2, unsigned int input2_length, byte * output, unsigned int output_length)
{
	bool flag = false;

	unsigned int b;	

	if (input1_length && input2_length)
	{
		byte d;

		unsigned int tempLength = input1_length + input2_length;

		byte * tempmain = new byte[tempLength];
		byte * tempadd = new byte[tempLength];

		primary_clear(tempmain, tempLength);

		for (b = 0; b < input2_length; b++)
		{
			for (d = 0; d < 8; d++)
			{
				if (degreesOf2[d] & input2[input2_length - 1 - b])
				{
					primary_clear(tempadd, tempLength);

					primary_bitshifting_up(input1, input1_length, d + b * 8, tempadd, tempLength);

					primary_summation(tempmain, tempLength, tempadd, tempLength, tempmain, tempLength);
				}
			}
		}

		primary_clear(output, output_length);
		if (primary_copy(tempmain, tempLength, output, output_length))
			flag = true;

		delete[] tempmain;
		delete[] tempadd;
	}
	else	
		primary_clear(output, output_length);

	return flag;
}

bool primary_testbit(const byte * bytes, unsigned int bytesLength, unsigned int bitnum)
{
	unsigned int bytenum = bitnum / 8;
	bitnum %= 8;

	bool flag = false;

	if (bytenum < bytesLength)
	{
		byte op = bytes[bytesLength - 1 - bytenum];

		switch (bitnum)
		{
		case 0:
			flag = op & 1;
			break;
		case 1:
			flag = op & 2;
			break;
		case 2:
			flag = op & 4;
			break;
		case 3:
			flag = op & 8;
			break;
		case 4:
			flag = op & 16;
			break;
		case 5:
			flag = op & 32;
			break;
		case 6:
			flag = op & 64;
			break;
		case 7:
			flag = op & 128;
			break;
		default:
			break;
		}
	}

	return flag;
}

void primary_setbit(byte * bytes, unsigned int bytesLength, unsigned int bitnum, bool val)
{
	unsigned int bytenum = bitnum / 8;

	if (bytenum < bytesLength)
	{
		bitnum = bitnum % 8;

		if (val)
		{
			switch (bitnum)
			{
			case 0:
				bytes[bytesLength - 1 - bytenum] |= 1;
				break;
			case 1:
				bytes[bytesLength - 1 - bytenum] |= 2;
				break;
			case 2:
				bytes[bytesLength - 1 - bytenum] |= 4;
				break;
			case 3:
				bytes[bytesLength - 1 - bytenum] |= 8;
				break;
			case 4:
				bytes[bytesLength - 1 - bytenum] |= 16;
				break;
			case 5:
				bytes[bytesLength - 1 - bytenum] |= 32;
				break;
			case 6:
				bytes[bytesLength - 1 - bytenum] |= 64;
				break;
			case 7:
				bytes[bytesLength - 1 - bytenum] |= 128;
				break;
			default:
				break;
			}
		}
		else
		{
			switch (bitnum)
			{
			case 0:
				bytes[bytesLength - 1 - bytenum] &= 254;
				break;
			case 1:
				bytes[bytesLength - 1 - bytenum] &= 253;
				break;
			case 2:
				bytes[bytesLength - 1 - bytenum] &= 251;
				break;
			case 3:
				bytes[bytesLength - 1 - bytenum] &= 247;
				break;
			case 4:
				bytes[bytesLength - 1 - bytenum] &= 239;
				break;
			case 5:
				bytes[bytesLength - 1 - bytenum] &= 223;
				break;
			case 6:
				bytes[bytesLength - 1 - bytenum] &= 191;
				break;
			case 7:
				bytes[bytesLength - 1 - bytenum] &= 127;
				break;
			default:
				break;
			}
		}
	}
}

long long primary_gethighesttruebitnum(const byte * bytes, unsigned int bytesLength)
{
	long long num = -1;

	if (bytesLength)
	{
		for (unsigned int b = 0; b < bytesLength; b++)
		{
			if (bytes[b])
			{
				if (bytes[b] > 127)
					num = 7;
				else if (bytes[b] > 63)
					num = 6;
				else if (bytes[b] > 31)
					num = 5;
				else if (bytes[b] > 15)
					num = 4;
				else if (bytes[b] > 7)
					num = 3;
				else if (bytes[b] > 3)
					num = 2;
				else if (bytes[b] > 1)
					num = 1;
				else
					num = 0;

				num += (bytesLength - 1 - b) * 8;

				break;
			}
		}
	}

	return num;
}

bool primary_first_frontmore_second(const byte * first, unsigned int first_length, const byte * second, unsigned int second_length)
{
	bool flag = false;

	if (first_length && second_length)
	{
		long long firstBitNum = primary_gethighesttruebitnum(first, first_length);

		long long secondBitNum =  primary_gethighesttruebitnum(second, second_length);

		if (!(firstBitNum < 0 || secondBitNum < 0))
		{
			bool f1, f2;

			while (firstBitNum >= 0 && secondBitNum >= 0)
			{
				f1 = primary_testbit(first, first_length, firstBitNum);
				f2 = primary_testbit(second, second_length, secondBitNum);

				if (f1 != f2)
				{
					flag = f1;

					break;
				}

				firstBitNum--;
				secondBitNum--;
			}
		}
	}

	return flag;
}

bool primary_first_frontequal_second(const byte * first, unsigned int first_length, const byte * second, unsigned int second_length)
{
	bool flag = false;

	if (first_length && second_length)
	{
		long long firstBitNum = primary_gethighesttruebitnum(first, first_length);

		long long secondBitNum = primary_gethighesttruebitnum(second, second_length);

		if (!(firstBitNum < 0 || secondBitNum < 0))
		{
			flag = true;

			while (firstBitNum >= 0 && secondBitNum >= 0)
			{
				if (primary_testbit(first, first_length, firstBitNum) != primary_testbit(second, second_length, secondBitNum))
				{
					flag = false;

					break;
				}

				firstBitNum--;
				secondBitNum--;
			}
		}
	}

	return flag;
}

bool primary_divide(const byte * divided, unsigned int divided_length, const byte * divider, unsigned int divider_length, byte * output, unsigned int output_length, byte * residue, unsigned int residue_length)
{
	bool flag = false;

	if (divided_length && divider_length)
	{
		unsigned int b;

		bool notZeroDivider = false, dividedMoreOrEqualDivider = false;

		for (b = 0; b < divider_length; b++)
		{
			if (divider[b])
			{
				notZeroDivider = true;
				break;
			}
		}

		if (notZeroDivider)
		{
                    //cout << " (" << int(*divider) << " > " << int(*divided) << ") ";
                    
			if (primary_first_more_second(divider, divider_length, divided, divided_length))			
				primary_clear(output, output_length);			
			else
				dividedMoreOrEqualDivider = true;
		}

		if (notZeroDivider)
		{
                    if(dividedMoreOrEqualDivider)
                    {
			long long dividedBitNum, dividerBitNum, deltaBitNum;

			bool shiftdownon1bit;

			byte * tempdivided = new byte[divided_length];

			unsigned int tempdivided_length = divided_length;

			byte * temp_divider = new byte[divided_length];

			unsigned int tempdivider_length = divided_length;

			primary_clear(temp_divider, tempdivider_length);
			primary_copy(divider, divider_length, temp_divider, tempdivider_length);

			byte * tempoutput = new byte[divided_length];

			unsigned int tempoutput_length = divided_length;

			primary_clear(tempoutput, tempoutput_length);

			primary_copy(divided, divided_length, tempdivided, tempdivided_length);

			while (!primary_first_more_second(temp_divider, tempdivider_length, tempdivided, tempdivided_length))
			{
				dividedBitNum = primary_gethighesttruebitnum(tempdivided, tempdivided_length);
				dividerBitNum = primary_gethighesttruebitnum(temp_divider, tempdivider_length);

				shiftdownon1bit = primary_first_frontmore_second(temp_divider, tempdivider_length, tempdivided, tempdivided_length);

				deltaBitNum = dividedBitNum - dividerBitNum;

				if (shiftdownon1bit && deltaBitNum)
					deltaBitNum--;

				primary_bitshifting_up(temp_divider, tempdivider_length, deltaBitNum, temp_divider, tempdivider_length);

				primary_setbit(tempoutput, tempoutput_length, deltaBitNum, true);

				primary_subtraction(tempdivided, tempdivided_length, temp_divider, tempdivider_length, tempdivided, tempdivided_length);

				primary_clear(temp_divider, tempdivider_length);
				primary_copy(divider, divider_length, temp_divider, tempdivider_length);
			}

			primary_clear(residue, residue_length);

			primary_copy(tempdivided, tempdivided_length, residue, residue_length);

			primary_clear(output, output_length);

			primary_copy(tempoutput, tempoutput_length, output, output_length);

			flag = true;

			delete[] tempdivided;
			delete[] temp_divider;			
			delete[] tempoutput;
                    }
                    else
                    {
                        primary_clear(residue, residue_length);

			primary_copy(divided, divided_length, residue, residue_length);
                    }
		}
	}

	return flag;
}

bignum::bignum() : isNegative(false), bytesLength(0), bytes(NULL) {}

bignum::bignum(byte * inBytes, unsigned int inBytesLength, bool isNegativeNum)
{
	bytesLength = inBytesLength;

	isNegative = isNegativeNum;

	if (bytesLength)
	{
		bytes = new byte[bytesLength];

		for (unsigned int byt = 0; byt < bytesLength; byt++)
			bytes[byt] = inBytes[byt];
	}
	else
		bytes = NULL;
}

bignum::bignum(unsigned int inBytesLength)
{
	bytesLength = inBytesLength;

	isNegative = false;

	if (bytesLength)
	{
		bytes = new byte[bytesLength];

		setBytesVal();
	}
	else
		bytes = NULL;
}

bignum::bignum(const bignum & bn)
{
	bytesLength = bn.bytesLength;

	isNegative = bn.isNegative;

	if (bytesLength)
	{
		bytes = new byte[bytesLength];

		for (unsigned int byt = 0; byt < bytesLength; byt++)
			bytes[byt] = bn.bytes[byt];
	}
	else
		bytes = NULL;
}

bignum::bignum(const byteArray & barr)
{
	isNegative = false;

	bytesLength = barr.length();

	bytes = bytesLength ? new byte[bytesLength] : NULL;

	for (unsigned int b = 0; b < bytesLength; b++)
		bytes[b] = barr[b];
}

bignum & bignum::operator=(const bignum & bn)
{
	if (this == &bn)
		return *this;

	if (bn.bytesLength)
	{
		unsigned int usefulBnBytesLength = bn.bytesLength - bn.empties();

		int diff = bytesLength - usefulBnBytesLength;

		if ((diff == 0) || (diff == 1))
		{
			bytes[0] = 0;

			for (unsigned int b = 0; b < usefulBnBytesLength; b++)			
				bytes[bytesLength - 1 - b] = bn.bytes[bn.bytesLength - 1 - b];			
		}
		else
		{
			delete[] bytes;

			bytesLength = bn.bytesLength;			

			bytes = new byte[bytesLength];

			for (unsigned int byt = 0; byt < bytesLength; byt++)
				bytes[byt] = bn.bytes[byt];
		}
	}
	else
	{
		delete[] bytes;

		bytesLength = 0;

		bytes = NULL;
	}

	isNegative = bn.isNegative;

	return *this;
}

bignum & bignum::operator=(const byteArray & barr)
{
	delete[] bytes;

	isNegative = false;

	bytesLength = barr.length();

	bytes = bytesLength ? new byte[bytesLength] : NULL;

	for (unsigned int b = 0; b < bytesLength; b++)
		bytes[b] = barr[b];

	return *this;
}

bignum::~bignum()
{
	delete[] bytes;
}

bignum & bignum::optimize()
{
	if (bytesLength <= 1 || bytes[0])
		return *this;

	unsigned int hbCount = 0;

	for (unsigned int byt = 0; byt < bytesLength - 1; byt++)
	{
		if (bytes[byt])
			break;
		else
			hbCount++;
	}

	if (hbCount)
	{
		byte * tPtr = bytes;

		unsigned int tLength = bytesLength;

		bytesLength -= hbCount;

		bytes = new byte[bytesLength];

		for (unsigned int byt = 0; byt < bytesLength; byt++)
			bytes[byt] = tPtr[byt + hbCount];

		delete[] tPtr;
	}

	return *this;
}

bool bignum::testBit(unsigned int bitnum) const
{
	return primary_testbit(bytes, bytesLength, bitnum);
}

bool bignum::testLowestBitNum() const
{
	bool flag = false;

	if (bytesLength)
		flag = bytes[bytesLength - 1] & byte(1);	

	return flag;
}

void bignum::setBit(unsigned int bitnum, bool val)
{
	unsigned int neededBytesLength = bitnum / 8 + 1;

	if (bytesLength < neededBytesLength)
	{
		unsigned int deltaBytesLength = neededBytesLength - bytesLength;

		byte * ptr = new byte[neededBytesLength];

		primary_clear(ptr, neededBytesLength);

		primary_copy(bytes, bytesLength, ptr + deltaBytesLength, bytesLength);

		delete[] bytes;

		bytes = ptr;

		bytesLength = neededBytesLength;
	}

	primary_setbit(bytes, bytesLength, bitnum, val);
}

long long bignum::getHighestTrueBitNum() const
{
	return primary_gethighesttruebitnum(bytes, bytesLength);
}

bignum bignum::abs() const
{
	bignum outputBigNum = *this;

	outputBigNum.isNegative = false;

	return outputBigNum;
}

void bignum::changeSign()
{
	isNegative = !isNegative;
}

bool bignum::lessThanZero() const
{
	return isNegative;
}

bool bignum::isEmpty() const
{
	return bytesLength == 0;
}

void bignum::setSizeAndClear(unsigned int size)
{
	delete[] bytes;

	bytesLength = size;

	if (bytesLength)
	{
		bytes = new byte[bytesLength];
		setBytesVal();
	}
	else
		bytes = NULL;
}

void bignum::setBytesVal(byte val)
{
	for (unsigned int byt = 0; byt < bytesLength; byt++)
		bytes[byt] = val;
}

unsigned int bignum::empties() const
{
	unsigned int res = 0;

	if (bytesLength > 1 && !bytes[0])
	{
		for (unsigned int byt = 0; byt < bytesLength - 1; byt++)
		{
			if (bytes[byt])
				break;

			res++;
		}
	}

	return res;
}

bool bignum::absEqual(const bignum & bn) const
{
	bool flag = isEmpty() == bn.isEmpty();

	if (flag && bytesLength)
	{
		unsigned int usefulLength = bytesLength - empties();
                
                unsigned int bnUsefulLength = bn.bytesLength - bn.empties();
                
                if(usefulLength == bnUsefulLength)
                {
                    unsigned int index = 0;
                    unsigned int pos = bytesLength - 1;
                    unsigned int bnPos = bn.bytesLength - 1;

                    while(index < usefulLength)
                    {
                        if(bytes[pos--] != bn.bytes[bnPos--])
                        {
                            flag = false;
                            break;
                        }

                        index++;                        
                    }
                }
                else
                    flag = false;
	}

	return flag;
}

bool bignum::absMore(const bignum & bn) const
{
	unsigned int empties__ = empties();
	unsigned int bnempties = bn.empties();

	unsigned int usefulBytesLength = bytesLength - empties__;
	unsigned int usefulBnBytesLength = bn.bytesLength - bnempties;
	
	return primary_first_more_second(bytes + empties__, usefulBytesLength, bn.bytes + bnempties, usefulBnBytesLength);
}

bool bignum::absLess(const bignum & bn) const
{
	unsigned int empties__ = empties();
	unsigned int bnempties = bn.empties();

	unsigned int usefulBytesLength = bytesLength - empties__;
	unsigned int usefulBnBytesLength = bn.bytesLength - bnempties;

	return primary_first_more_second(bn.bytes + bnempties, usefulBnBytesLength, bytes + empties__, usefulBytesLength);
}

bignum bignum::absSumAbs(const bignum & bn) const
{
	bignum obn;

	if (bytesLength && bn.bytesLength)
	{
		unsigned int empties__ = empties();
		unsigned int bnempties__ = bn.empties();

		unsigned int usefulBytesLength = bytesLength - empties__;
		unsigned int usefulBnBytesLength = bn.bytesLength - bnempties__;

		obn.setSizeAndClear((usefulBytesLength > usefulBnBytesLength ? usefulBytesLength : usefulBnBytesLength) + 1);

		primary_summation(bytes + empties__, usefulBytesLength, bn.bytes + bnempties__, usefulBnBytesLength, obn.bytes, obn.bytesLength);		
	}
	else if(bytesLength)
	{
		obn = *this;
	}
	else if (bn.bytesLength)
	{
		obn = bn;
	}

	return obn;
}

bignum & bignum::addAbs(const bignum & bn)
{
	if (bytesLength && bn.bytesLength)
	{
		unsigned int empties__ = empties();
		unsigned int bnempties__ = bn.empties();

		unsigned int usefulBytesLength = bytesLength - empties__;
		unsigned int usefulBnBytesLength = bn.bytesLength - bnempties__;

		if (bytesLength > usefulBnBytesLength && empties__)		
			primary_summation(bytes + empties__, usefulBytesLength, bn.bytes + bnempties__, usefulBnBytesLength, bytes, bytesLength);		
		else
		{
			unsigned int outbytes_length = (usefulBytesLength > usefulBnBytesLength ? usefulBytesLength : usefulBnBytesLength) + 1;
			byte * outbytes = new byte[outbytes_length];
			primary_summation(bytes + empties__, usefulBytesLength, bn.bytes + bnempties__, usefulBnBytesLength, outbytes, outbytes_length);

			delete[] bytes;

			bytes = outbytes;
			bytesLength = outbytes_length;
		}
	}
	else if (bn.bytesLength)
	{
		*this = bn;
	}

	return *this;
}

bignum bignum::absDeltaAbs(const bignum & bn) const
{
	bignum obn;

	if (bytesLength && bn.bytesLength)
	{
		const bignum * mainPtr;
		const bignum * subtrahendPtr;

		if (absMore(bn))
		{
			mainPtr = this;
			subtrahendPtr = &bn;				
		}
		else
		{
			mainPtr = &bn;
			subtrahendPtr = this;
		}		

		obn.setSizeAndClear(mainPtr->bytesLength);

		unsigned int usefulSubtrahendBytesLength = subtrahendPtr->bytesLength - subtrahendPtr->empties();

		primary_subtraction(mainPtr->bytes, mainPtr->bytesLength, subtrahendPtr->bytes, subtrahendPtr->bytesLength, obn.bytes, obn.bytesLength);		
	}
	else if (bytesLength)
	{
		obn = *this;
	}
	else if (bn.bytesLength)
	{
		obn = bn;
	}

	return obn;
}

bignum & bignum::deltaAbs(const bignum & bn)
{
	if (bytesLength && bn.bytesLength)
	{
		if (absLess(bn))
		{
			*this = absDeltaAbs(bn);
		}
		else
		{
			unsigned int empties__ = empties();
			unsigned int bnempties__ = bn.empties();

			unsigned int usefulBytesLength = bytesLength - empties__;
			unsigned int usefulBnBytesLength = bn.bytesLength - bnempties__;

			primary_subtraction(bytes + empties__, usefulBytesLength, bn.bytes + bnempties__, usefulBnBytesLength, bytes, bytesLength);
		}
	}
	else if (bn.bytesLength)
	{
		*this = bn;
	}

	return *this;
}

bool bignum::frontEqual(const bignum & bn) const
{
	bool flag = false;

	if (bytesLength && bn.bytesLength)
	{
		long long bitNum = getHighestTrueBitNum();

		long long bnBitNum = bn.getHighestTrueBitNum();

		if (!(bitNum < 0 || bnBitNum < 0))
		{
			flag = true;

			while (bitNum >= 0 && bnBitNum >= 0)
			{
				if (testBit(bitNum) != bn.testBit(bnBitNum))
				{
					flag = false;

					break;
				}
			}
		}
	}

	return flag;
}

bool bignum::frontMore(const bignum & bn) const
{	
	return primary_first_frontmore_second(bytes, bytesLength, bn.bytes, bn.bytesLength);
}

bool bignum::frontLess(const bignum & bn) const
{
	return primary_first_frontmore_second(bn.bytes, bn.bytesLength, bytes, bytesLength);
}

bignum bignum::operator+(const bignum & bn) const
{
	bignum obn;

	if (bytesLength && bn.bytesLength)
	{
		if (isNegative == bn.isNegative)
		{
			obn = absSumAbs(bn);
			obn.isNegative = isNegative;
		}
		else
		{
			obn = absDeltaAbs(bn);

			obn.isNegative = absLess(bn) ? bn.isNegative : isNegative;
		}
	}
	else
	{
		if (bytesLength)
			obn = *this;
		else if (bn.bytesLength)
			obn = bn;
	}

	return obn;
}

bignum & bignum::operator+=(const bignum & bn)
{
	if (isNegative == bn.isNegative)	
		addAbs(bn);	
	else
	{	
		if(absLess(bn))
			isNegative = bn.isNegative;

		deltaAbs(bn);
	}

	return *this;
}

bignum bignum::operator-(const bignum & bn) const
{
	bignum obn;

	if (bytesLength && bn.bytesLength)
	{
		//cout << "\t\tminus: ";

		if (isNegative == bn.isNegative)
		{
			//cout << "\t\tsign1==sign2";

			obn = absDeltaAbs(bn);

			obn.isNegative = absLess(bn) ? !isNegative : isNegative;
		}
		else
		{
			//cout << "\t\tsign1!=sign2";

			obn = absSumAbs(bn);

			obn.isNegative = isNegative;
		}

		//cout << endl;
	}
	else if (bytesLength)
	{
		obn = *this;
	}
	else if (bn.bytesLength)
	{
		obn = bn;
		obn.isNegative = !bn.isNegative;
	}

	return obn;
}

bignum & bignum::operator-=(const bignum & bn)
{
	if (isNegative == bn.isNegative)
	{
		if (absLess(bn))
			isNegative = !isNegative;

		deltaAbs(bn);
	}
	else	
		addAbs(bn);	

	return *this;
}

bool bignum::operator==(const bignum & bn) const
{
	return (absEqual(bn) && (isNegative == bn.isNegative));
}

bool bignum::operator!=(const bignum & bn) const
{
	return !operator==(bn);
}

bool bignum::operator>(const bignum & bn) const
{
	bool flag;

	if (isNegative == bn.isNegative)
	{	
		flag = absMore(bn);

		if (isNegative)
			flag = !flag;
	}
	else	
		flag = !isNegative;	

	return flag;
}

bool bignum::operator<(const bignum & bn) const
{
	return (!(*this == bn) && !(*this > bn));		
}

bool bignum::operator>=(const bignum & bn) const
{
	return (*this == bn) || (*this > bn);
}

bool bignum::operator<=(const bignum & bn) const
{
	return (*this == bn) || (*this < bn);
}

bignum bignum::operator-() const
{
	bignum obn = *this;

	if (bytesLength)
	{
		unsigned int empties__ = empties();

		if (((bytesLength - empties__) > 1) || bytes[0])
			obn.isNegative = !isNegative;
	}

	return obn;
}

bignum bignum::operator>>(unsigned int bits) const
{
	bignum obn;

	if (bytesLength)
	{
		unsigned int empties__ = empties();

		unsigned int usefulBytesLength = bytesLength - empties__;

		obn.setSizeAndClear(usefulBytesLength - bits / 8);

		primary_bitshifting_down(bytes + empties__, usefulBytesLength, bits, obn.bytes, obn.bytesLength);
	}

	return obn;
}

bignum bignum::operator<<(unsigned int bits) const
{
	bignum obn;

	if (bytesLength)
	{
		unsigned int empties__ = empties();

		unsigned int usefulBytesLength = bytesLength - empties__;

		obn.setSizeAndClear(usefulBytesLength + bits / 8 + (bits % 8 ? 1 : 0));

		primary_bitshifting_up(bytes + empties__, usefulBytesLength, bits, obn.bytes, obn.bytesLength);
	}
	return obn;
}

bignum & bignum::operator>>=(unsigned int bits)
{
	if (bytesLength)
	{

		unsigned int empties__ = empties();

		unsigned int usefulBytesLength = bytesLength - empties__;

		primary_bitshifting_down(bytes + empties__, usefulBytesLength, bits, bytes + empties__, usefulBytesLength);
	}

	return *this;
}

bignum & bignum::operator<<=(unsigned int bits)
{
	if (bytesLength)
	{
		unsigned int empties__ = empties();

		unsigned int usefulBytesLength = bytesLength - empties__;

		unsigned int bytesNeeded = usefulBytesLength + bits / 8 + (bits % 8 ? 1 : 0);

		//cout << ": " << bytesNeeded << " vs " << usefulBytesLength << endl;

		if (bytesLength < bytesNeeded)
		{
			byte * ptrBytes = new byte[bytesNeeded];

			//cout << "bytes: ";
			//showHex(bytes + empties__, usefulBytesLength);

			primary_bitshifting_up(bytes + empties__, usefulBytesLength, bits, ptrBytes, bytesNeeded);

			//cout << "ptr: ";
			//showHex(ptrBytes, bytesNeeded);

			delete[] bytes;

			//cout << "ptr-a: ";
			//showHex(ptrBytes, bytesNeeded);

			bytes = ptrBytes;

			bytesLength = bytesNeeded;
		}
		else
		{	
			primary_bitshifting_up(bytes + empties__, usefulBytesLength, bits, bytes + empties__ - (bytesNeeded - usefulBytesLength), bytesNeeded);
		}
	}

	return *this;
}

bignum bignum::operator*(const bignum & bn) const
{
	bignum obn;

	if (bytesLength && bn.bytesLength)
	{
		unsigned int empties__ = empties();

		unsigned int usefulBytesLength = bytesLength - empties__;

		unsigned int bnEmpties__ = bn.empties();

		unsigned int usefulBnBytesLength = bn.bytesLength - bnEmpties__;

		obn.setSizeAndClear(usefulBytesLength + usefulBnBytesLength);

		if (primary_multiplication(bytes + empties__, usefulBytesLength, bn.bytes + bnEmpties__, usefulBnBytesLength, obn.bytes, obn.bytesLength))		
			obn.isNegative = isNegative != bn.isNegative;		
		else		
			obn.setSizeAndClear(0);		
	}

	//cout << "\t\tmult: (" << getHexString() << ") x (" << bn.getHexString() << ") = (" << obn.getHexString() << ")" << endl;

	return obn;
}

bignum & bignum::operator*=(const bignum & bn)
{
	if (bytesLength & bn.bytesLength)
	{
		unsigned int empties__ = empties();

		unsigned int usefulBytesLength = bytesLength - empties__;

		unsigned int bnEmpties__ = bn.empties();

		unsigned int usefulBnBytesLength = bn.bytesLength - bnEmpties__;

		isNegative = isNegative != bn.isNegative;

		if (bytesLength >= usefulBytesLength + usefulBnBytesLength)
		{
			if (!primary_multiplication(bytes + empties__, usefulBytesLength, bn.bytes + bnEmpties__, usefulBnBytesLength, bytes, bytesLength))
			{
				setSizeAndClear(0);
				isNegative = false;
			}
		}
		else
		{
			byte * temp = new byte[usefulBytesLength + usefulBnBytesLength];

			if (primary_multiplication(bytes + empties__, usefulBytesLength, bn.bytes + bnEmpties__, usefulBnBytesLength, temp, usefulBytesLength + usefulBnBytesLength))
			{
				delete[] bytes;

				bytes = temp;
			}
			else
			{
				setSizeAndClear(0);
				isNegative = false;

				delete[] temp;
			}
		}
	}
	else
	{
		setSizeAndClear(0);
		isNegative = false;
	}

	return *this;
}

bignum bignum::operator/(const bignum & bn) const
{
	unsigned int empties__ = empties();

	unsigned int usefulBytesLength = bytesLength - empties__;

	unsigned int bnEmpties = bn.empties();

	unsigned int usefulBnBytesLength = bn.bytesLength - bnEmpties;

	bignum obn;

	if (usefulBytesLength && (usefulBnBytesLength && ((usefulBnBytesLength > 1) || bn.bytes[bn.bytesLength - 1])))
	{
		obn.setSizeAndClear(usefulBytesLength);

		obn.isNegative = isNegative != bn.isNegative;

		primary_divide(bytes + empties__, usefulBytesLength, bn.bytes + bnEmpties, usefulBnBytesLength, obn.bytes, obn.bytesLength, NULL, 0);
	}

	return obn;
}

bignum & bignum::operator/=(const bignum & bn)
{
	unsigned int empties__ = empties();

	unsigned int usefulBytesLength = bytesLength - empties__;

	unsigned int bnEmpties = bn.empties();

	unsigned int usefulBnBytesLength = bn.bytesLength - bnEmpties;

	if (usefulBytesLength && (usefulBnBytesLength && ((usefulBnBytesLength > 1) || bn.bytes[bn.bytesLength - 1])))
	{
		isNegative = isNegative != bn.isNegative;

		primary_divide(bytes + empties__, usefulBytesLength, bn.bytes + bnEmpties, usefulBnBytesLength, bytes, bytesLength, NULL, 0);
	}

	return *this;
}

bignum bignum::operator%(const bignum & bn) const
{
	unsigned int empties__ = empties();

	unsigned int usefulBytesLength = bytesLength - empties__;

	unsigned int bnEmpties = bn.empties();

	unsigned int usefulBnBytesLength = bn.bytesLength - bnEmpties;

	bignum obn;

	if (usefulBytesLength && (usefulBnBytesLength && ((usefulBnBytesLength > 1) || bn.bytes[bn.bytesLength - 1])))
	{
		obn.setSizeAndClear(usefulBytesLength);

		obn.isNegative = isNegative != bn.isNegative;
                
                //cout << " (" << int(*(bytes + empties__)) << ") ";

		primary_divide(bytes + empties__, usefulBytesLength, bn.bytes + bnEmpties, usefulBnBytesLength, NULL, 0, obn.bytes, obn.bytesLength);
	}

	return obn;
}

bignum & bignum::operator%=(const bignum & bn)
{
	unsigned int empties__ = empties();

	unsigned int usefulBytesLength = bytesLength - empties__;

	unsigned int bnEmpties = bn.empties();

	unsigned int usefulBnBytesLength = bn.bytesLength - bnEmpties;

	if (usefulBytesLength && (usefulBnBytesLength && (usefulBnBytesLength > 1) || bn.bytes[0]))
	{
		isNegative = isNegative != bn.isNegative;

		primary_divide(bytes + empties__, usefulBytesLength, bn.bytes + bnEmpties, usefulBnBytesLength, NULL, 0, bytes, bytesLength);
	}

	return *this;
}

string bignum::getHexString(bool showHighBytesZeros) const
{
	unsigned int empties__ = empties();

	string output = (bytesLength && isNegative) ? "-" : "";

	unsigned int start = showHighBytesZeros ? 0 : empties__;

	for (unsigned int b = start; b < bytesLength; b++)
	{	
		output += hexDigits[bytes[b] / 16];
		output += hexDigits[bytes[b] % 16];
	}

	return output;
}

string bignum::getBinaryString(bool showHighBytesZeros) const
{
	unsigned int empties__ = empties();

	string output = (bytesLength && isNegative) ? "-" : "";

	unsigned int start = showHighBytesZeros ? 0 : empties__;

	for (unsigned int b = start; b < bytesLength; b++)
	{
		output += (bytes[b] & 128 ? "1" : "0");
		output += (bytes[b] & 64 ? "1" : "0");
		output += (bytes[b] & 32 ? "1" : "0");
		output += (bytes[b] & 16 ? "1" : "0");
		output += (bytes[b] & 8 ? "1" : "0");
		output += (bytes[b] & 4 ? "1" : "0");
		output += (bytes[b] & 2 ? "1" : "0");
		output += (bytes[b] & 1 ? "1" : "0");
	}

	return output;
}

byte hexCharToByte(char ch)
{
	byte  byt = 0;

	switch (ch)
	{
	case 'F':
	case 'f':	
		byt = 15;
		break;
	case 'E':
	case 'e':
		byt = 14;
		break;
	case 'D':
	case 'd':
		byt = 13;
		break;
	case 'C':
	case 'c':
		byt = 12;
		break;
	case 'B':
	case 'b':
		byt = 11;
		break;
	case 'A':
	case 'a':
		byt = 10;
		break;
	case '9':
		byt = 9;
		break;
	case '8':
		byt = 8;
		break;
	case '7':
		byt = 7;
		break;
	case '6':
		byt = 6;
		break;
	case '5':
		byt = 5;
		break;
	case '4':
		byt = 4;
		break;
	case '3':
		byt = 3;
		break;
	case '2':
		byt = 2;
		break;
	case '1':
		byt = 1;
		break;
	case '0':
		byt = 0;
		break;
	default:
		break;
	}

	return byt;
}

void bignum::setBytesFromHexString(const string & hexStr)
{
	isNegative = false;

	bool odd = hexStr.length() % 2;

	bytesLength = hexStr.length() / 2 + (odd ? 1 : 0);

	bytes = new byte[bytesLength];

	if (odd)
		bytes[0] = hexCharToByte(hexStr[0]);

	for (unsigned int b = odd ? 1 : 0; b < bytesLength; b++)
	{
		bytes[b] = hexCharToByte(hexStr[2 * b - (odd ? 1 : 0)]) << 4;
		bytes[b] += hexCharToByte(hexStr[2 * b + 1 - (odd ? 1 : 0)]);
	}
}

bignum::bignum(const string & hexStr)
{
	setBytesFromHexString(hexStr);
}

bignum & bignum::operator=(const string & hexStr)
{
	delete[] bytes;

	setBytesFromHexString(hexStr);

	return *this;

}

bignum & bignum::operator=(long long num)
{
	if (bytesLength < 8)
	{
		delete[] bytes;

		bytesLength = 8;

		bytes = new byte[bytesLength];
	}

	isNegative = (num < 0);

	if (num < 0)
		num = -num;

	setBytesVal();

	for (unsigned int b = 0; b < 8; b++)	
		bytes[bytesLength - 1 - b] = (num >> (b * 8)) & 255;	

	return *this;
}

bignum & bignum::operator=(sbyte num)
{
	if (!bytesLength)
	{
		bytesLength = 1;
		bytes = new byte[bytesLength];
	}

	setBytesVal();

	isNegative = false;

	if (num < 0)
	{
		isNegative = true;
		num = -num;
	}

	bytes[bytesLength - 1] = num & 127;

	return *this;
}

bool bignum::operator==(sbyte num) const
{
	bool flag = false;

	if (bytesLength && (getHighestTrueBitNum() < 7))
	{
		if (!num)		
			flag = !bytes[bytesLength - 1];
		else if ((num < 0) == lessThanZero())		
			flag = sbyte(bytes[bytesLength - 1]) == num;		
	}

	return flag;
}

bool bignum::operator!=(sbyte num) const
{
	return !operator==(num);
}

bool bignum::operator>(sbyte num) const
{
	bool flag = false;

	if (bytesLength)
	{
		if ((num < 0) == isNegative)
		{
			if (getHighestTrueBitNum() < 7)
				flag = sbyte(sbyte(bytes[bytesLength - 1]) * (isNegative ? -1 : 1)) > num;			
			else
				flag = !isNegative;
		}
		else
			flag = num < 0;
	}

	return flag;
}

bool bignum::operator<(sbyte num) const
{
	bool flag = true;

	if (bytesLength)
	{
		if ((num < 0) == isNegative)
		{
			if (getHighestTrueBitNum() < 7)
				flag = sbyte(sbyte(bytes[bytesLength - 1]) * (isNegative ? -1 : 1)) < num;
			else
				flag = isNegative;
		}
		else
			flag = !(num < 0);
	}

	return flag;
}

byteArray bignum::toByteArray(unsigned int bytes_length) const
{
	unsigned int usefulBytesLength = bytesLength - empties();

	byteArray outBytes(bytes_length > usefulBytesLength ? bytes_length : usefulBytesLength);
	outBytes.setAllTo(0);
	
	for (unsigned int byt = 0; byt < usefulBytesLength; byt++)
		outBytes[outBytes.length() - 1 - byt] = bytes[bytesLength - 1 - byt];

	return outBytes;
}
/* //reserv
byteArray bignum::toByteArray(unsigned int bytes_length) const
{
	byteArray outBytes(bytes_length > bytesLength ? bytes_length : bytesLength);
	outBytes.setAllTo(0);

	for (unsigned int byt = 0; byt < bytesLength; byt++)
		outBytes[outBytes.length() - 1 - byt] = bytes[bytesLength - 1 - byt];

	return outBytes;
}*/

void primary_truncate_to_bit_num(byte * bytes, unsigned int bytes_length, unsigned int bitnum)
{
	if (bytes_length)
	{
		unsigned int zeroBytesLength = bytes_length - (bitnum / 8 + 1), byt;

		bitnum %= 8;

		//showHex(bytes, bytes_length);

		for (byt = 0; byt < zeroBytesLength; byt++)
			bytes[byt] = 0;

		switch (bitnum)
		{
			case 7:
				bytes[byt] &= 255;
				break;
			case 6:
				bytes[byt] &= 127;
				break;
			case 5:
				bytes[byt] &= 63;
				break;
			case 4:
				bytes[byt] &= 31;
				break;
			case 3:
				bytes[byt] &= 15;
				break;
			case 2:
				bytes[byt] &= 7;
				break;
			case 1:
				bytes[byt] &= 3;
				break;
			case 0:
				bytes[byt] &= 1;
				break;
			default:
				break;
		}

		//showHex(bytes, bytes_length);
	}
}

bignum & bignum::truncateItselfToBitNum(unsigned int bitnum)
{
	primary_truncate_to_bit_num(bytes, bytesLength, bitnum);

	return *this;
}

bignum bignum::truncateToBitNum(unsigned int bitnum) const
{
	bignum outNum = *this;

	primary_truncate_to_bit_num(outNum.bytes, outNum.bytesLength, bitnum);

	//cout << "inFunc: " << outNum.getBinaryString() << endl;

	return outNum;
}

byte bignum::operator[](unsigned int byteNum) const
{
    byte outByte = 0;

    if (byteNum < bytesLength)	
        outByte = bytes[bytesLength - 1 - byteNum];

    return outByte;
}

void bignum::setByte(unsigned int ByteNum, byte ByteValue)
{
    if (ByteNum < bytesLength)	
        bytes[bytesLength - 1 - ByteNum] = ByteValue;
}

byte bignum::getByte(unsigned int ByteNum) const
{
    if (ByteNum < bytesLength)	
        return bytes[bytesLength - 1 - ByteNum];
    
    return 0;
}

void bignum::setBytesLengthIfNull(unsigned BytesLength)
{
    if(bytes == NULL)
    {
        bytes = new byte[BytesLength];
        
        bytesLength = BytesLength;
    }
}

bool primary_multiplication(const byte * input, unsigned int input_length, const byte & b, byte * output, unsigned int output_length)
{
	bool flag = false;

	if (input_length)
	{
		byte d;

		unsigned int tempLength = input_length + 1;

		byte * tempmain = new byte[tempLength];
		byte * tempadd = new byte[tempLength];

		primary_clear(tempmain, tempLength);
		
		for (d = 0; d < 8; d++)
		{
			if (degreesOf2[d] & b)
			{
				primary_clear(tempadd, tempLength);

				primary_bitshifting_up(input, input_length, d, tempadd, tempLength);

				primary_summation(tempmain, tempLength, tempadd, tempLength, tempmain, tempLength);
			}
		}

		primary_clear(output, output_length);
		if (primary_copy(tempmain, tempLength, output, output_length))
			flag = true;

		delete[] tempmain;
		delete[] tempadd;
	}
	else
		primary_clear(output, output_length);

	return flag;
}

bignum bignum::operator*(const byte & b) const
{
	bignum obn;	
	
	if (bytesLength)
	{
		unsigned int empties__ = empties();

		unsigned int usefulBytesLength = bytesLength - empties__;

		obn.setSizeAndClear(usefulBytesLength + 1);

		if (primary_multiplication(bytes + empties__, usefulBytesLength, b, obn.bytes, obn.bytesLength))
			obn.isNegative = isNegative;
		else
			obn.setSizeAndClear(0);
	}

	return obn;
}
