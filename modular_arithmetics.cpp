#include "bignum.h"
#include "funcs.h"
#include <iostream>
#include <sys/types.h>

typedef unsigned __int128 uint128;



bignum alignByMod(bignum num, const bignum & mod)
{	
	if (!mod.isEmpty() && !mod.lessThanZero())
	{
		if (num.absEqual(mod))
			num = sbyte(0);		
		else if (num.lessThanZero())
		{
			while (num.absMore(mod))
				num += mod;

			num += mod;
		}
		else if (num.absMore(mod))
			num %= mod;
	}

	return num;
}

void extended_euclid(int a, int b, int * x, int * y, int * d)
{
	int q, r, x1, x2, y1, y2;

	if (b == 0)
	{
		*d = a, *x = 1, *y = 0;
		return;
	}

	x2 = 1, x1 = 0, y2 = 0, y1 = 1;

	while (b > 0)
	{
		q = a / b, r = a - q * b;
		//cout << "x-1: " << *x << endl;
		*x = x2 - q * x1, *y = y2 - q * y1;
		//cout << "x: " << *x << endl;
		a = b, b = r;
		x2 = x1, x1 = *x, y2 = y1, y1 = *y;
	}

	* d = a, * x = x2, * y = y2;
}

int inverseByMod(int a, int n)
{
	int d, x, y;

	extended_euclid(a, n, &x, &y, &d);	

	if (d == 1 && x < 0)	
		x = n + x;	

	return (d == 1) ? x : 0;
}

void extended_euclid(bignum a, bignum b, bignum * x, bignum * y, bignum * d)
{
	bignum q, r, x1, x2, y1, y2;

	if (b == sbyte(0))
	{
		*d = a, *x = sbyte(1), *y = sbyte(0);
		return;
	}

	x2 = sbyte(1), x1 = sbyte(0), y2 = sbyte(0), y1 = sbyte(1);

	while (b > 0)
	{
		q = a / b, r = a - (q * b);
		
		*x = x2 - (q * x1), *y = y2 - (q * y1);
		
		a = b, b = r;
		x2 = x1, x1 = *x, y2 = y1, y1 = *y;
	}

	*d = a, *x = x2, *y = y2;
}

bignum inverseByMod(const bignum & a, const bignum & mod)
{
	bignum d, x, y;

	extended_euclid(a, mod, &x, &y, &d);

	if (d == 1)
	{
		if (x < 0)
		{
			x = mod + x;
		}
	}
	else	
		x = sbyte(0);	

	return x;
}

bignum powByMod(const bignum & num, const bignum & pow, const bignum & mod)
{
	unsigned int bit;

	bignum intermediateResult;

	bignum result("1");
        
	if (num.getBytesLength() && pow.getBytesLength() && mod.getBytesLength())
	{
		for (unsigned int byt = 0; byt < pow.getBytesLength(); byt++)
		{
			for (bit = 0; bit < 8; bit++)
			{
				if (byt || bit)
					intermediateResult = alignByMod(intermediateResult * intermediateResult, mod);
				else
					intermediateResult = alignByMod(num, mod);

				if (pow.testBit(byt * 8 + bit))
                                {
					result = alignByMod(result * intermediateResult, mod);
                                        
                                        //cout << "result: " << result.getHexString() << endl;
                                }
			}
		}
	}

	return result;
}

bignum alignByPow2FromMod(const bignum & num, unsigned int pow2FromMod)
{
	bignum outNum;

	//cout << "pow2: " << pow2FromMod << endl;

	if (num.getBytesLength() && pow2FromMod)
	{
		int numMaxBit = num.getHighestTrueBitNum();

		//cout << "maxBit: " << numMaxBit << endl;

		if (numMaxBit < pow2FromMod)
			outNum = num;
		else
			outNum = num.truncateToBitNum(pow2FromMod - 1);
	}
	//cout << "num:\t" << num.getBinaryString() << endl;
	//cout << "outNum:\t" << outNum.getBinaryString() << endl;

	return outNum;
}

bignum negInvMod_Montgomery(const bignum & mod, const bignum & modR)
{
	int w = modR.getHighestTrueBitNum();

	bignum y("1");

	bignum tempPow2NumA("2");
	bignum tempPow2NumB("1");

	for (int i = 2; i <= w; i++)
	{
		//tempPow2NumA = tempPow2NumA << 1;
		tempPow2NumA <<= 1;
		//cout << "A: " << tempPow2NumA.getBinaryString(true) << " - " << tempPow2NumA.empties() << endl;
		tempPow2NumB <<= 1;
		//tempPow2NumB = tempPow2NumB << 1;

		if (((mod * y) % tempPow2NumA) != 1)
			y += tempPow2NumB;
	}

	//cout << "y: " << y.getHexString() << endl;

	return modR - y;
}

bignum RPow2ByMod_Montgomery(const bignum & modR, const bignum & mod)
{
	int w = modR.getHighestTrueBitNum();

	int n = 1;

	bignum tempNum("0");

	tempNum.setBit(w * n - 1, true);

	//cout << "temp: " << tempNum.getBinaryString() << endl;

	for (int i = 1; i <= w * n + 1; i++)
		tempNum = alignByMod(tempNum + tempNum, mod);

	return tempNum;
}

bignum reductionByMod_Montgomery(const bignum & num, const bignum & mod, const bignum & modR, const bignum & mu)
{
	int pow2fromModR = modR.getHighestTrueBitNum();

	//bignum q = alignByMod(mu * alignByMod(num, modR), modR);
	bignum q = alignByPow2FromMod(mu * alignByPow2FromMod(num, pow2fromModR), pow2fromModR);
	bignum C = mod * q;
	C += num;
	//cout << "C:" << endl << C.getBinaryString(true) << endl;
	C >>= pow2fromModR;
	//cout << "C/R:" << endl << C.getBinaryString(true) << endl;
	if (C >= mod)
		C -= mod;

	return C;
}

bignum pseudomultiplicationByMod_Montgomery(const bignum & num1, const bignum & num2, const bignum & mod, const bignum & modR, const bignum & mu) //, int base
{	
	bool muBit = mu.testLowestBitNum();

	int n = modR.getHighestTrueBitNum(), i;	

	bignum C("0");		

	for (i = 0; i < n; i++)
	{
		

		if (num1.testBit(i))		
			C += num2;

		if (muBit && C.testLowestBitNum())
			C += mod;

		C >>= 1;
	}

	if (C >= mod)
		C -= mod;

	return C;
}

bignum powByMod_Montgomery(const bignum & prepNum, const bignum & exp, const bignum & prepR, const bignum & mod, const bignum & modR, const bignum & mu)
{
	bignum x__ = prepNum;
	bignum p__ = prepR;
	bignum ONE("01");

	int D = exp.getHighestTrueBitNum() + 1, i;

	for (int i = D - 1; i >= 0; i--)
	{
		p__ = pseudomultiplicationByMod_Montgomery(p__, p__, mod, modR, mu);

		if (exp.testBit(i))
			p__ = pseudomultiplicationByMod_Montgomery(p__, x__, mod, modR, mu);
	}

	return pseudomultiplicationByMod_Montgomery(p__, ONE, mod, modR, mu);
}

bignum pseudomultiplicationByMod_Montgomery_base256(const bignum & num1, const bignum & num2, const bignum & mod, const bignum & modR, const bignum & mu)/*experimental*/
{
	bool muBit = mu.testLowestBitNum();

	int n__ = modR.getHighestTrueBitNum();
	int n = n__ / 8, i;

	bignum C("0");

	bignum q;

	bignum base("0100");
	bignum temp;

	for (i = 0; i < n; i++)
	{
		temp = (num2 * num1[i]);

		C = temp + C;

		q = alignByMod(mu * C, base);

		temp = mod * q;
		temp = temp + C;

		C = temp / base;//  / modR
	}

	if (C >= mod)
		C -= mod;

	return C;
}

using barray::byteArray;

class ull7_spec_bignum
{
private:
	unsigned long long * items;
	unsigned int length;
public:
	ull7_spec_bignum(const byteArray & bytes);
	ull7_spec_bignum(const ull7_spec_bignum & sbn);
	ull7_spec_bignum & operator=(const ull7_spec_bignum & sbn);
	ull7_spec_bignum & shiftRigth();
	bool testBit(unsigned int num) const;
	ull7_spec_bignum & operator+=(const ull7_spec_bignum & sbn);
	ull7_spec_bignum & operator-=(const ull7_spec_bignum & sbn);
	bool operator>=(const ull7_spec_bignum & sbn);
	ull7_spec_bignum & setToZero();
	bool testLowestBitNum() { return length ? (items[length - 1] & 1ULL) : 0; }
	~ull7_spec_bignum();
	byteArray toByteArray(unsigned int arrayLength = 0) const;	
};

ull7_spec_bignum::ull7_spec_bignum(const byteArray & bytes)
{
	length = (bytes.length() / 7) + (((bytes.length() % 7) > 0) ? 1 : 0);

	items = length ? new unsigned long long[length] : NULL;

	for (unsigned int i = 0, b = 0; i < length; i++, b += 7)
	{
		items[length - 1 - i] = 0;
		items[length - 1 - i] |= (unsigned long long)bytes[bytes.length() - 1 - b];
		items[length - 1 - i] |= (unsigned long long)bytes[bytes.length() - 1 - b - 1] << 8;
		items[length - 1 - i] |= (unsigned long long)bytes[bytes.length() - 1 - b - 2] << 16;
		items[length - 1 - i] |= (unsigned long long)bytes[bytes.length() - 1 - b - 3] << 24;
		items[length - 1 - i] |= (unsigned long long)bytes[bytes.length() - 1 - b - 4] << 32;
		items[length - 1 - i] |= (unsigned long long)bytes[bytes.length() - 1 - b - 5] << 40;
		items[length - 1 - i] |= (unsigned long long)bytes[bytes.length() - 1 - b - 6] << 48;
	}
}

ull7_spec_bignum::ull7_spec_bignum(const ull7_spec_bignum & sbn)
{
	length = sbn.length;

	items = length ? new unsigned long long[length] : NULL;

	for (unsigned int i = 0; i < length; i++)	
		items[i] = sbn.items[i];
}

ull7_spec_bignum & ull7_spec_bignum::operator=(const ull7_spec_bignum & sbn)
{
	delete[] items;

	length = sbn.length;

	items = length ? new unsigned long long[length] : NULL;

	for (unsigned int i = 0; i < length; i++)
		items[i] = sbn.items[i];

	return *this;
}

ull7_spec_bignum & ull7_spec_bignum::shiftRigth()
{
	bool carry = false;

	for (unsigned int i = 0; i < length; i++)
	{
		if(carry)
			items[i] |= 0x100000000000000ULL;

		carry = bool(items[i] & 1ULL);

		items[i] >>= 1;
	}

	return *this;
}

bool ull7_spec_bignum::testBit(unsigned int num) const
{
	bool flag = false;

	unsigned int itemNum = num / 56;
	num %= 56;

	if (itemNum < length)
	{
		switch (num)
		{
		case 0:
			flag = bool(items[length - 1 - itemNum] & 1ULL);
			break;
		case 1:
			flag = bool(items[length - 1 - itemNum] & 2ULL);
			break;
		case 2:
			flag = bool(items[length - 1 - itemNum] & 4ULL);
			break;
		case 3:
			flag = bool(items[length - 1 - itemNum] & 8ULL);
			break;
		case 4:
			flag = bool(items[length - 1 - itemNum] & 16ULL);
			break;
		case 5:
			flag = bool(items[length - 1 - itemNum] & 32ULL);
			break;
		case 6:
			flag = bool(items[length - 1 - itemNum] & 64ULL);
			break;
		case 7:
			flag = bool(items[length - 1 - itemNum] & 128ULL);
			break;

		case 8:
			flag = bool(items[length - 1 - itemNum] & 0x100ULL);
			break;
		case 9:
			flag = bool(items[length - 1 - itemNum] & 0x200ULL);
			break;
		case 10:
			flag = bool(items[length - 1 - itemNum] & 0x400ULL);
			break;
		case 11:
			flag = bool(items[length - 1 - itemNum] & 0x800ULL);
			break;
		case 12:
			flag = bool(items[length - 1 - itemNum] & 0x1000ULL);
			break;
		case 13:
			flag = bool(items[length - 1 - itemNum] & 0x2000ULL);
			break;
		case 14:
			flag = bool(items[length - 1 - itemNum] & 0x4000ULL);
			break;
		case 15:
			flag = bool(items[length - 1 - itemNum] & 0x8000ULL);
			break;

		case 16:
			flag = bool(items[length - 1 - itemNum] & 0x10000ULL);
			break;
		case 17:
			flag = bool(items[length - 1 - itemNum] & 0x20000ULL);
			break;
		case 18:
			flag = bool(items[length - 1 - itemNum] & 0x40000ULL);
			break;
		case 19:
			flag = bool(items[length - 1 - itemNum] & 0x80000ULL);
			break;
		case 20:
			flag = bool(items[length - 1 - itemNum] & 0x100000ULL);
			break;
		case 21:
			flag = bool(items[length - 1 - itemNum] & 0x200000ULL);
			break;
		case 22:
			flag = bool(items[length - 1 - itemNum] & 0x400000ULL);
			break;
		case 23:
			flag = bool(items[length - 1 - itemNum] & 0x800000ULL);
			break;

		case 24:
			flag = bool(items[length - 1 - itemNum] & 0x1000000ULL);
			break;
		case 25:
			flag = bool(items[length - 1 - itemNum] & 0x2000000ULL);
			break;
		case 26:
			flag = bool(items[length - 1 - itemNum] & 0x4000000ULL);
			break;
		case 27:
			flag = bool(items[length - 1 - itemNum] & 0x8000000ULL);
			break;
		case 28:
			flag = bool(items[length - 1 - itemNum] & 0x10000000ULL);
			break;
		case 29:
			flag = bool(items[length - 1 - itemNum] & 0x20000000ULL);
			break;
		case 30:
			flag = bool(items[length - 1 - itemNum] & 0x40000000ULL);
			break;
		case 31:
			flag = bool(items[length - 1 - itemNum] & 0x80000000ULL);
			break;

		case 32:
			flag = bool(items[length - 1 - itemNum] & 0x100000000ULL);
			break;
		case 33:
			flag = bool(items[length - 1 - itemNum] & 0x200000000ULL);
			break;
		case 34:
			flag = bool(items[length - 1 - itemNum] & 0x400000000ULL);
			break;
		case 35:
			flag = bool(items[length - 1 - itemNum] & 0x800000000ULL);
			break;
		case 36:
			flag = bool(items[length - 1 - itemNum] & 0x1000000000ULL);
			break;
		case 37:
			flag = bool(items[length - 1 - itemNum] & 0x2000000000ULL);
			break;
		case 38:
			flag = bool(items[length - 1 - itemNum] & 0x4000000000ULL);
			break;
		case 39:
			flag = bool(items[length - 1 - itemNum] & 0x8000000000ULL);
			break;

		case 40:
			flag = bool(items[length - 1 - itemNum] & 0x10000000000ULL);
			break;
		case 41:
			flag = bool(items[length - 1 - itemNum] & 0x20000000000ULL);
			break;
		case 42:
			flag = bool(items[length - 1 - itemNum] & 0x40000000000ULL);
			break;
		case 43:
			flag = bool(items[length - 1 - itemNum] & 0x80000000000ULL);
			break;
		case 44:
			flag = bool(items[length - 1 - itemNum] & 0x100000000000ULL);
			break;
		case 45:
			flag = bool(items[length - 1 - itemNum] & 0x200000000000ULL);
			break;
		case 46:
			flag = bool(items[length - 1 - itemNum] & 0x400000000000ULL);
			break;
		case 47:
			flag = bool(items[length - 1 - itemNum] & 0x800000000000ULL);
			break;

		case 48:
			flag = bool(items[length - 1 - itemNum] & 0x1000000000000ULL);
			break;
		case 49:
			flag = bool(items[length - 1 - itemNum] & 0x2000000000000ULL);
			break;
		case 50:
			flag = bool(items[length - 1 - itemNum] & 0x4000000000000ULL);
			break;
		case 51:
			flag = bool(items[length - 1 - itemNum] & 0x8000000000000ULL);
			break;
		case 52:
			flag = bool(items[length - 1 - itemNum] & 0x10000000000000ULL);
			break;
		case 53:
			flag = bool(items[length - 1 - itemNum] & 0x20000000000000ULL);
			break;
		case 54:
			flag = bool(items[length - 1 - itemNum] & 0x40000000000000ULL);
			break;
		case 55:
			flag = bool(items[length - 1 - itemNum] & 0x80000000000000ULL);
			break;

		default:
			break;
		}
	}

	return flag;
}

ull7_spec_bignum & ull7_spec_bignum::operator+=(const ull7_spec_bignum & sbn)
{
	bool carry = false;
	unsigned int i;

	for (i = 0; i < sbn.length; i++)
	{
		items[length - 1 - i] &= 0xFFFFFFFFFFFFFFULL;
		items[length - 1 - i] += sbn.items[sbn.length - 1 - i];
		if (carry)
		{
			items[length - 1 - i] += 1ULL;
			items[length - i] &= 0xFFFFFFFFFFFFFFULL;
		}
		carry = bool(items[length - 1 - i] & 0x100000000000000ULL);
	}

	for (i = sbn.length; i < length; i++)
	{
		items[length - 1 - i] &= 0xFFFFFFFFFFFFFFULL;
		if (carry)
		{
			items[length - 1 - i] += 1ULL;
			items[length - i] &= 0xFFFFFFFFFFFFFFULL;
		}
		else
			break;
		carry = bool(items[length - 1 - i] & 0x100000000000000ULL);
	}

	return *this;
}

ull7_spec_bignum & ull7_spec_bignum::operator-=(const ull7_spec_bignum & sbn)
{
	bool carry = false;
	unsigned int i;

	for (i = 0; i < sbn.length; i++)
	{
		if (carry)
		{
			if (items[length - 1 - i] == 0ULL)			
				items[length - 1 - i] = 0xFFFFFFFFFFFFFFULL;							
			else
			{
				items[length - 1 - i]--;
				carry = false;
			}
		}

		if (items[length - 1 - i] < sbn.items[sbn.length - 1 - i])
		{
			items[length - 1 - i] |= 0x100000000000000ULL;
			carry = true;
		}

		items[length - 1 - i] -= sbn.items[sbn.length - 1 - i];
	}

	for (i = sbn.length; i < length; i++)
	{
		if (carry)
		{
			if (items[length - 1 - i] == 0ULL)
				items[length - 1 - i] = 0xFFFFFFFFFFFFFFULL;
			else
			{
				items[length - 1 - i]--;
				break;
			}
		}
		else
			break;
	}

	return *this;
}

bool ull7_spec_bignum::operator>=(const ull7_spec_bignum & sbn)
{
	bool more = false, equal = true, cont___ = true;
	unsigned int i;

	if (length > sbn.length)
	{
		int delta = length - sbn.length;

		for (i = 0; i < delta; i++)
		{
			if (items[i] > 0ULL)
			{
				more = true;
				equal = false;
				cont___ = false;
				break;
			}
		}

		if (cont___)
		{
			for (i = 0; i < sbn.length; i++)
			{
				if (items[i + delta] != sbn.items[i])
				{
					more = items[i + delta] > sbn.items[i];
					equal = false;
					break;
				}
			}
		}
	}
	else if (length < sbn.length)
	{
		int delta = sbn.length - length;

		for (i = 0; i < delta; i++)
		{
			if (sbn.items[i] > 0ULL)
			{
				more = false;
				equal = false;
				cont___ = false;
				break;
			}
		}

		if (cont___)
		{
			for (i = 0; i < length; i++)
			{
				if (sbn.items[i + delta] != items[i])
				{
					more = items[i] > sbn.items[i + delta];
					equal = false;
					break;
				}
			}
		}
	}
	else
	{
		for (i = 0; i < length; i++)
		{
			if (sbn.items[i] != items[i])
			{
				more = items[i] > sbn.items[i];
				equal = false;
				break;
			}
		}
	}

	return more || equal;
}

ull7_spec_bignum & ull7_spec_bignum::setToZero()
{
	for (unsigned int i = 0; i < length; i++)
		items[i] = 0;

	return *this;
}

ull7_spec_bignum::~ull7_spec_bignum()
{
	delete[] items;
}

byteArray ull7_spec_bignum::toByteArray(unsigned int arrayLength) const
{
	byteArray outBytes(arrayLength ? arrayLength : length * 7);

	outBytes.setAllTo(0);

	for (unsigned int i = 0, b = 0; i < length; i++, b += 7)
	{
		if (b < outBytes.length())		
			outBytes[outBytes.length() - 1 - b] = items[length - 1 - i] & 255;		

		if (b < outBytes.length() - 1)
			outBytes[outBytes.length() - 1 - b - 1] = (items[length - 1 - i] >> 8) & 255;

		if (b < outBytes.length() - 2)
			outBytes[outBytes.length() - 1 - b - 2] = (items[length - 1 - i] >> 16) & 255;

		if (b < outBytes.length() - 3)
			outBytes[outBytes.length() - 1 - b - 3] = (items[length - 1 - i] >> 24) & 255;

		if (b < outBytes.length() - 4)
			outBytes[outBytes.length() - 1 - b - 4] = (items[length - 1 - i] >> 32) & 255;

		if (b < outBytes.length() - 5)
			outBytes[outBytes.length() - 1 - b - 5] = (items[length - 1 - i] >> 40) & 255;

		if (b < outBytes.length() - 6)
			outBytes[outBytes.length() - 1 - b - 6] = (items[length - 1 - i] >> 48) & 255;
	}

	return outBytes;
}

ull7_spec_bignum pseudomultiplicationByMod_Montgomery(const ull7_spec_bignum & num1, const ull7_spec_bignum & num2, const ull7_spec_bignum & mod, int powOfModR, bool muBit, ull7_spec_bignum & C)
{
	for (int i = 0; i < powOfModR; i++)
	{
		if (num1.testBit(i))		
			C += num2;					

		if (muBit && C.testLowestBitNum())
			C += mod;
		
		C.shiftRigth();
	}

	if (C >= mod)
		C -= mod;

	return C;
}

bignum powByMod_Montgomery_experimental(const bignum & prepNum, const bignum & exp, const bignum & prepR, const bignum & mod, const bignum & modR, const bignum & mu)
{
    //cout << "experimental using..." << endl;
    
	ull7_spec_bignum x__(prepNum.toByteArray());
	ull7_spec_bignum p__(prepR.toByteArray());
	bignum ONE("01");

	ull7_spec_bignum mod_ull7sb(mod.toByteArray());	

	int D = exp.getHighestTrueBitNum() + 1, i;


	ull7_spec_bignum C___(byteArray(mod.getBytesLength() + 1).setAllTo(0));

	int powOfModR = modR.getHighestTrueBitNum();

	bool muBit = mu.testLowestBitNum();	

	for (int i = D - 1; i >= 0; i--)
	{
		p__ = pseudomultiplicationByMod_Montgomery(p__, p__, mod_ull7sb, powOfModR, muBit, C___);
		C___.setToZero();

		if (exp.testBit(i))
		{
			p__ = pseudomultiplicationByMod_Montgomery(p__, x__, mod_ull7sb, powOfModR, muBit, C___);
			C___.setToZero();
		}
	}

	bignum pbn___(p__.toByteArray());

	return pseudomultiplicationByMod_Montgomery(pbn___, ONE, mod, modR, mu);
}



class ull15_spec_bignum
{
private:
    uint128 * items;
    unsigned int length;
public:
    ull15_spec_bignum(const byteArray & bytes);
    ull15_spec_bignum(const ull15_spec_bignum & sbn);
    ull15_spec_bignum & operator=(const ull15_spec_bignum & sbn);
    ull15_spec_bignum & shiftRigth();
    bool testBit(unsigned int num) const;
    ull15_spec_bignum & operator+=(const ull15_spec_bignum & sbn);
    ull15_spec_bignum & operator-=(const ull15_spec_bignum & sbn);
    bool operator>=(const ull15_spec_bignum & sbn);
    ull15_spec_bignum & setToZero();
    bool testLowestBitNum() { return length ? (items[length - 1] & 1ULL) : 0; }
    ~ull15_spec_bignum();
    byteArray toByteArray(unsigned int arrayLength = 0) const;	
};

ull15_spec_bignum::ull15_spec_bignum(const byteArray & bytes)
{
    length = (bytes.length() / 15) + (((bytes.length() % 15) > 0) ? 1 : 0);

    items = length ? new uint128[length] : NULL;

    for (unsigned int i = 0, b = 0; i < length; i++, b += 15)
    {
        items[length - 1 - i] = 0;
        items[length - 1 - i] |= (uint128)bytes[bytes.length() - 1 - b];
        items[length - 1 - i] |= (uint128)bytes[bytes.length() - 1 - b - 1] << 8;
        items[length - 1 - i] |= (uint128)bytes[bytes.length() - 1 - b - 2] << 16;
        items[length - 1 - i] |= (uint128)bytes[bytes.length() - 1 - b - 3] << 24;
        items[length - 1 - i] |= (uint128)bytes[bytes.length() - 1 - b - 4] << 32;
        items[length - 1 - i] |= (uint128)bytes[bytes.length() - 1 - b - 5] << 40;
        items[length - 1 - i] |= (uint128)bytes[bytes.length() - 1 - b - 6] << 48;

        items[length - 1 - i] |= (uint128)bytes[bytes.length() - 1 - b - 7] << 56;
        items[length - 1 - i] |= (uint128)bytes[bytes.length() - 1 - b - 8] << 64;
        items[length - 1 - i] |= (uint128)bytes[bytes.length() - 1 - b - 9] << 72;
        items[length - 1 - i] |= (uint128)bytes[bytes.length() - 1 - b - 10] << 80;
        items[length - 1 - i] |= (uint128)bytes[bytes.length() - 1 - b - 11] << 88;
        items[length - 1 - i] |= (uint128)bytes[bytes.length() - 1 - b - 12] << 96;
        items[length - 1 - i] |= (uint128)bytes[bytes.length() - 1 - b - 13] << 104;
        items[length - 1 - i] |= (uint128)bytes[bytes.length() - 1 - b - 14] << 112;
    }
}

ull15_spec_bignum::ull15_spec_bignum(const ull15_spec_bignum & sbn)
{
    length = sbn.length;

    items = length ? new uint128[length] : NULL;

    for (unsigned int i = 0; i < length; i++)	
        items[i] = sbn.items[i];
}

ull15_spec_bignum & ull15_spec_bignum::operator=(const ull15_spec_bignum & sbn)
{
    delete[] items;

    length = sbn.length;

    items = length ? new uint128[length] : NULL;

    for (unsigned int i = 0; i < length; i++)
        items[i] = sbn.items[i];

    return *this;
}

ull15_spec_bignum & ull15_spec_bignum::shiftRigth()
{
    bool carry = false;

    for (unsigned int i = 0; i < length; i++)
    {
        if(carry)
            items[i] |= uint128(1) << 8 * 15;

        carry = bool(items[i] & 1);

        items[i] >>= 1;
    }

    return *this;
}

bool ull15_spec_bignum::testBit(unsigned int num) const
{
	bool flag = false;

	unsigned int itemNum = num / 120;
	num %= 120;

	if (itemNum < length)
	{
		switch (num)
		{
		case 0:
                        flag = bool(items[length - 1 - itemNum] & uint128(1));
                        break;
                case 1:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 1);
                        break;
                case 2:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 2);
                        break;
                case 3:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 3);
                        break;
                case 4:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 4);
                        break;
                case 5:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 5);
                        break;
                case 6:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 6);
                        break;
                case 7:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 7);
                        break;
                case 8:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 8);
                        break;
                case 9:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 9);
                        break;
                case 10:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 10);
                        break;
                case 11:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 11);
                        break;
                case 12:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 12);
                        break;
                case 13:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 13);
                        break;
                case 14:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 14);
                        break;
                case 15:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 15);
                        break;
                case 16:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 16);
                        break;
                case 17:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 17);
                        break;
                case 18:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 18);
                        break;
                case 19:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 19);
                        break;
                case 20:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 20);
                        break;
                case 21:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 21);
                        break;
                case 22:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 22);
                        break;
                case 23:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 23);
                        break;
                case 24:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 24);
                        break;
                case 25:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 25);
                        break;
                case 26:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 26);
                        break;
                case 27:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 27);
                        break;
                case 28:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 28);
                        break;
                case 29:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 29);
                        break;
                case 30:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 30);
                        break;
                case 31:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 31);
                        break;
                case 32:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 32);
                        break;
                case 33:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 33);
                        break;
                case 34:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 34);
                        break;
                case 35:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 35);
                        break;
                case 36:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 36);
                        break;
                case 37:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 37);
                        break;
                case 38:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 38);
                        break;
                case 39:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 39);
                        break;
                case 40:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 40);
                        break;
                case 41:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 41);
                        break;
                case 42:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 42);
                        break;
                case 43:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 43);
                        break;
                case 44:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 44);
                        break;
                case 45:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 45);
                        break;
                case 46:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 46);
                        break;
                case 47:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 47);
                        break;
                case 48:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 48);
                        break;
                case 49:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 49);
                        break;
                case 50:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 50);
                        break;
                case 51:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 51);
                        break;
                case 52:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 52);
                        break;
                case 53:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 53);
                        break;
                case 54:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 54);
                        break;
                case 55:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 55);
                        break;
                case 56:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 56);
                        break;
                case 57:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 57);
                        break;
                case 58:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 58);
                        break;
                case 59:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 59);
                        break;
                case 60:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 60);
                        break;
                case 61:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 61);
                        break;
                case 62:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 62);
                        break;
                case 63:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 63);
                        break;
                case 64:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 64);
                        break;
                case 65:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 65);
                        break;
                case 66:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 66);
                        break;
                case 67:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 67);
                        break;
                case 68:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 68);
                        break;
                case 69:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 69);
                        break;
                case 70:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 70);
                        break;
                case 71:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 71);
                        break;
                case 72:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 72);
                        break;
                case 73:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 73);
                        break;
                case 74:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 74);
                        break;
                case 75:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 75);
                        break;
                case 76:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 76);
                        break;
                case 77:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 77);
                        break;
                case 78:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 78);
                        break;
                case 79:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 79);
                        break;
                case 80:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 80);
                        break;
                case 81:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 81);
                        break;
                case 82:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 82);
                        break;
                case 83:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 83);
                        break;
                case 84:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 84);
                        break;
                case 85:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 85);
                        break;
                case 86:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 86);
                        break;
                case 87:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 87);
                        break;
                case 88:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 88);
                        break;
                case 89:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 89);
                        break;
                case 90:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 90);
                        break;
                case 91:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 91);
                        break;
                case 92:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 92);
                        break;
                case 93:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 93);
                        break;
                case 94:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 94);
                        break;
                case 95:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 95);
                        break;
                case 96:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 96);
                        break;
                case 97:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 97);
                        break;
                case 98:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 98);
                        break;
                case 99:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 99);
                        break;
                case 100:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 100);
                        break;
                case 101:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 101);
                        break;
                case 102:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 102);
                        break;
                case 103:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 103);
                        break;
                case 104:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 104);
                        break;
                case 105:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 105);
                        break;
                case 106:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 106);
                        break;
                case 107:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 107);
                        break;
                case 108:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 108);
                        break;
                case 109:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 109);
                        break;
                case 110:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 110);
                        break;
                case 111:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 111);
                        break;
                case 112:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 112);
                        break;
                case 113:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 113);
                        break;
                case 114:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 114);
                        break;
                case 115:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 115);
                        break;
                case 116:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 116);
                        break;
                case 117:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 117);
                        break;
                case 118:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 118);
                        break;
                case 119:
                        flag = bool(items[length - 1 - itemNum] & uint128(1) << 119);
                        break;

		default:
			break;
		}
	}

	return flag;
}

ull15_spec_bignum & ull15_spec_bignum::operator+=(const ull15_spec_bignum & sbn)
{
    bool carry = false;
    unsigned int i;

    for (i = 0; i < sbn.length; i++)
    {
        items[length - 1 - i] &= (uint128(1) << 15 * 8) - 1;
        items[length - 1 - i] += sbn.items[sbn.length - 1 - i];
        if (carry)
        {
            items[length - 1 - i] += 1;
            items[length - i] &= (uint128(1) << 15 * 8) - 1;
        }
        carry = bool(items[length - 1 - i] & (uint128(1) << 15 * 8));
    }

    for (i = sbn.length; i < length; i++)
    {
        items[length - 1 - i] &= (uint128(1) << 15 * 8) - 1;
        if (carry)
        {
            items[length - 1 - i] += 1;
            items[length - i] &= (uint128(1) << 15 * 8) - 1;
        }
        else
            break;
        carry = bool(items[length - 1 - i] & (uint128(1) << 15 * 8));
    }

    return *this;
}

ull15_spec_bignum & ull15_spec_bignum::operator-=(const ull15_spec_bignum & sbn)
{
    bool carry = false;
    unsigned int i;

    for (i = 0; i < sbn.length; i++)
    {
        if (carry)
        {
            if (items[length - 1 - i] == 0)
                items[length - 1 - i] = (uint128(1) << 15 * 8) - 1;							
            else
            {
                items[length - 1 - i]--;
                carry = false;
            }
        }

        if (items[length - 1 - i] < sbn.items[sbn.length - 1 - i])
        {
            items[length - 1 - i] |= (uint128(1) << 15 * 8);
            carry = true;
        }

        items[length - 1 - i] -= sbn.items[sbn.length - 1 - i];
    }

    for (i = sbn.length; i < length; i++)
    {
        if (carry)
        {
            if (items[length - 1 - i] == 0)
                items[length - 1 - i] = (uint128(1) << 15 * 8) - 1;
            else
            {
                items[length - 1 - i]--;
                break;
            }
        }
        else
            break;
    }

    return *this;
}

bool ull15_spec_bignum::operator>=(const ull15_spec_bignum & sbn)
{
    bool more = false, equal = true, cont___ = true;
    unsigned int i;

    if (length > sbn.length)
    {
        int delta = length - sbn.length;

        for (i = 0; i < delta; i++)
        {
            if (items[i] > 0)
            {
                more = true;
                equal = false;
                cont___ = false;
                break;
            }
        }

        if (cont___)
        {
            for (i = 0; i < sbn.length; i++)
            {
                if (items[i + delta] != sbn.items[i])
                {
                    more = items[i + delta] > sbn.items[i];
                    equal = false;
                    break;
                }
            }
        }
    }
    else if (length < sbn.length)
    {
        int delta = sbn.length - length;

        for (i = 0; i < delta; i++)
        {
            if (sbn.items[i] > 0)
            {
                more = false;
                equal = false;
                cont___ = false;
                break;
            }
        }

        if (cont___)
        {
            for (i = 0; i < length; i++)
            {
                if (sbn.items[i + delta] != items[i])
                {
                    more = items[i] > sbn.items[i + delta];
                    equal = false;
                    break;
                }
            }
        }
    }
    else
    {
        for (i = 0; i < length; i++)
        {
            if (sbn.items[i] != items[i])
            {
                more = items[i] > sbn.items[i];
                equal = false;
                break;
            }
        }
    }

    return more || equal;
}

ull15_spec_bignum & ull15_spec_bignum::setToZero()
{
    for (unsigned int i = 0; i < length; i++)
        items[i] = 0;

    return *this;
}

ull15_spec_bignum::~ull15_spec_bignum()
{
	delete[] items;
}

byteArray ull15_spec_bignum::toByteArray(unsigned int arrayLength) const
{
    byteArray outBytes(arrayLength ? arrayLength : length * 15);

    outBytes.setAllTo(0);

    for (unsigned int i = 0, b = 0; i < length; i++, b += 15)
    {
        if (b < outBytes.length())		
            outBytes[outBytes.length() - 1 - b] = items[length - 1 - i] & 255;		

        if (b < outBytes.length() - 1)
            outBytes[outBytes.length() - 1 - b - 1] = (items[length - 1 - i] >> 8) & 255;

        if (b < outBytes.length() - 2)
            outBytes[outBytes.length() - 1 - b - 2] = (items[length - 1 - i] >> 16) & 255;

        if (b < outBytes.length() - 3)
            outBytes[outBytes.length() - 1 - b - 3] = (items[length - 1 - i] >> 24) & 255;

        if (b < outBytes.length() - 4)
            outBytes[outBytes.length() - 1 - b - 4] = (items[length - 1 - i] >> 32) & 255;

        if (b < outBytes.length() - 5)
            outBytes[outBytes.length() - 1 - b - 5] = (items[length - 1 - i] >> 40) & 255;

        if (b < outBytes.length() - 6)
            outBytes[outBytes.length() - 1 - b - 6] = (items[length - 1 - i] >> 48) & 255;



        if (b < outBytes.length() - 7)
            outBytes[outBytes.length() - 1 - b - 7] = (items[length - 1 - i] >> 56) & 255;
        if (b < outBytes.length() - 8)
            outBytes[outBytes.length() - 1 - b - 8] = (items[length - 1 - i] >> 64) & 255;
        if (b < outBytes.length() - 9)
            outBytes[outBytes.length() - 1 - b - 9] = (items[length - 1 - i] >> 72) & 255;
        if (b < outBytes.length() - 10)
            outBytes[outBytes.length() - 1 - b - 10] = (items[length - 1 - i] >> 80) & 255;
        if (b < outBytes.length() - 11)
            outBytes[outBytes.length() - 1 - b - 11] = (items[length - 1 - i] >> 88) & 255;
        if (b < outBytes.length() - 12)
            outBytes[outBytes.length() - 1 - b - 12] = (items[length - 1 - i] >> 96) & 255;
        if (b < outBytes.length() - 13)
            outBytes[outBytes.length() - 1 - b - 13] = (items[length - 1 - i] >> 104) & 255;
        if (b < outBytes.length() - 14)
            outBytes[outBytes.length() - 1 - b - 14] = (items[length - 1 - i] >> 112) & 255;
    }

    return outBytes;
}

ull15_spec_bignum pseudomultiplicationByMod_Montgomery(const ull15_spec_bignum & num1, const ull15_spec_bignum & num2, const ull15_spec_bignum & mod, int powOfModR, bool muBit, ull15_spec_bignum & C)
{
    for (int i = 0; i < powOfModR; i++)
    {
        if (num1.testBit(i))		
            C += num2;					

        if (muBit && C.testLowestBitNum())
            C += mod;

        C.shiftRigth();
    }

    if (C >= mod)
        C -= mod;

    return C;
}

bignum powByMod_Montgomery_experimental15(const bignum & prepNum, const bignum & exp, const bignum & prepR, const bignum & mod, const bignum & modR, const bignum & mu)
{
    //cout << "experimental using..." << endl;
    
    ull15_spec_bignum x__(prepNum.toByteArray());
    ull15_spec_bignum p__(prepR.toByteArray());
    bignum ONE("01");

    ull15_spec_bignum mod_ull15sb(mod.toByteArray());	

    int D = exp.getHighestTrueBitNum() + 1, i;


    ull15_spec_bignum C___(byteArray(mod.getBytesLength() + 1).setAllTo(0));

    int powOfModR = modR.getHighestTrueBitNum();

    bool muBit = mu.testLowestBitNum();	

    for (int i = D - 1; i >= 0; i--)
    {
        p__ = pseudomultiplicationByMod_Montgomery(p__, p__, mod_ull15sb, powOfModR, muBit, C___);
        C___.setToZero();

        if (exp.testBit(i))
        {
            p__ = pseudomultiplicationByMod_Montgomery(p__, x__, mod_ull15sb, powOfModR, muBit, C___);
            C___.setToZero();
        }
    }

    bignum pbn___(p__.toByteArray());

    return pseudomultiplicationByMod_Montgomery(pbn___, ONE, mod, modR, mu);
}
