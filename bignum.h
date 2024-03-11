#ifndef BIGNUM_H
#define BIGNUM_H

#include <string>

#include "barray.h"

using barray::byteArray;

using std::string;

typedef unsigned char byte;

typedef char sbyte;

//  big-endian big number
class bignum
{
protected:
	bool isNegative;
	byte * bytes;
	unsigned int bytesLength;

	bool frontMore(const bignum & bn) const;
	bool frontLess(const bignum & bn) const;
	bool frontEqual(const bignum & bn) const;

	bignum absSumAbs(const bignum & bn) const;
	bignum absDeltaAbs(const bignum & bn) const;
	bignum & addAbs(const bignum & bn);
	bignum & deltaAbs(const bignum & bn);
	void setSizeAndClear(unsigned int size);
	void setBytesVal(byte val = 0);
	void setBytesFromHexString(const string & hexStr);
	bignum(unsigned int inBytesLength);
public:
	bignum();
	bignum(byte * inBytes, unsigned int inBytesLength, bool isNegativeNum = false);
	bignum(const bignum & bn);
	bignum(const string & hexStr);
	bignum(const byteArray & barr);
	bool absMore(const bignum & bn) const;
	bool absLess(const bignum & bn) const;
	bool absEqual(const bignum & bn) const;
	bignum & operator=(const bignum & bn);
	bignum & operator=(const string & hexStr);

	bignum & operator=(const byteArray & barr);

	bignum & operator=(long long num);
	bignum & operator=(sbyte num);
	bignum operator+(const bignum & bn) const;
	bignum operator-(const bignum & bn) const;
	bignum operator*(const bignum & bn) const;
	bignum operator/(const bignum & bn) const;
	bignum operator%(const bignum & bn) const;
	bignum operator>>(unsigned int bits) const;
	bignum operator<<(unsigned int bits) const;
	bool operator>(const bignum & bn) const;
	bool operator>(sbyte num) const;
	bool operator<(const bignum & bn) const;
	bool operator<(sbyte num) const;
	bool operator>=(const bignum & bn) const;
	bool operator<=(const bignum & bn) const;
	bool operator==(const bignum & bn) const;	
	bool operator==(sbyte num) const;
	bool operator!=(const bignum & bn) const;
	bool operator!=(sbyte num) const;
	bignum operator-() const;
	bignum abs() const;
	bignum & operator+=(const bignum & bn);
	bignum & operator-=(const bignum & bn);
	bignum & operator*=(const bignum & bn);
	bignum & operator/=(const bignum & bn);
	bignum & operator%=(const bignum & bn);
	bignum & operator>>=(unsigned int bits);
	bignum & operator<<=(unsigned int bits);
	~bignum();
	unsigned int empties() const;
	bignum & optimize();	
	long long getHighestTrueBitNum() const;
	bool testBit(unsigned int bitnum) const;
	bool testLowestBitNum() const;
	void setBit(unsigned int bitnum, bool val);
	void changeSign();	
	bool lessThanZero() const;
	bool isEmpty() const;
	string getHexString(bool showHighBytesZeros = false) const;
	string getBinaryString(bool showHighBytesZeros = false) const;
	unsigned int getBytesLength() const { return bytesLength; };
	byteArray toByteArray(unsigned int bytes_length = 0) const;
	bignum & truncateItselfToBitNum(unsigned int bitnum);
	bignum truncateToBitNum(unsigned int bitnum) const;
	byte operator[](unsigned int byteNum) const;        
        void setByte(unsigned int ByteNum, byte ByteValue);
        byte getByte(unsigned int ByteNum) const;
        void setBytesLengthIfNull(unsigned BytesLength);
	bignum operator*(const byte & b) const;
	//void fastAbsAdd(const bignum & bn);
};

#endif