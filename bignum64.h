#ifndef BIGNUM64_H
#define BIGNUM64_H

#include "barray.h";

using barray::byteArray;

class bignum64
{
private:
    unsigned long long * cells = NULL;
    unsigned cellsLength = 0;
    unsigned hiddenCellsLength = 0;
    char s = 0;
    static const byte cells_bytes_size;
    static const byte cells_bits_size;
    static const byte maxbitnum_of_cell;    
    //unsigned long long op;
    unsigned getObjectiveCellsLength() const;
    char compareWith(const bignum64 & BN) const;
public:
    bignum64() {};
    bignum64(const bignum64 & B);
    bignum64(bignum64 && B);
    ~bignum64();
    bignum64 & operator=(const bignum64 & B);
    bignum64 & operator=(bignum64 && B);
    bignum64(const byteArray & Bytes);
    byteArray toByteArray() const;
    int getMaxBitNum() const;
    bignum64 operator-() const;
    bignum64 abs() const;
};

#endif

