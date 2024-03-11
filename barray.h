/*
* To change this license header, choose License Headers in Project Properties.
* To change this template file, choose Tools | Templates
* and open the template in the editor.
*/

/*
* File:   barray.h
* Author: ned
*
* Created on November 10, 2017, 12:49 PM
*/

#ifndef BARRAY_H
#define BARRAY_H

#include <iostream>
#include <string>
#include <initializer_list>

typedef unsigned char byte;

namespace barray
{
    class byteArray
    {
    private:
        byte * bytes;
        unsigned long long bytesLength;
        void copyBytes(const byteArray & bA);
    public:
        byteArray(unsigned long long inBytesLength = 0, const byte * inBytes = NULL);
        byteArray(const byteArray & bA);
        byteArray(byteArray && bA);
        byteArray(const std::string & str);
        byteArray(const std::initializer_list<byte> & list);
        ~byteArray();
        unsigned long long length() const;
        byte & operator[](int) const;
        byteArray & operator=(const byteArray &);
        byteArray & operator=(byteArray &&);
        byteArray & operator=(const std::string & str);
        byteArray operator+(const byteArray &) const;
        byteArray operator^(const byteArray &) const;
        byteArray operator&(const byteArray &) const;
        byteArray operator|(const byteArray &) const;
        byteArray & operator+=(const byteArray &);
        bool operator==(const byteArray &) const;
        bool operator!=(const byteArray &) const;
        bool operator<(const byteArray &) const;
        std::string ToString() const;   
        bool ToBool() const;
        explicit operator std::string() const;
        //void getBytesFrom(const byte * source, int length);
        //void getBytesFrom(const byteArray & source);
        void copyBytesTo(byte * dist, unsigned long long length, unsigned long long startIndexSource = 0, unsigned long long startIndexDist = 0) const;
        void copyBytesTo(byteArray & dist, unsigned long long length, unsigned long long startIndexSource = 0, unsigned long long startIndexDist = 0) const;
        void copyBytesFrom(const byte * source, unsigned long long length, unsigned long long startIndexDist = 0, unsigned long long startIndexSource = 0);
        void copyBytesFrom(const byteArray & source, unsigned long long length, unsigned long long startIndexDist = 0, unsigned long long startIndexSource = 0);
        void clear();
        void setLength(unsigned long long inBytesLength);
        byteArray getBytes(unsigned long long cBytesLength, unsigned long long startIndex = 0) const;
        byteArray getBytesBeginWith(unsigned long long index) const;
        byte * bytesPTR();
        const byte * bytesPTR() const;
        //void releasePtr() {bytes = NULL; bytesLength = 0;};
        byteArray & setAllTo(byte val);
        byteArray & setTo(byte val, unsigned long long length, unsigned long long startIndex = 0);
        long long firstPosOf(byte Val, long long Shift = 0) const;
        bool isEqualRange(const byteArray & Compared, unsigned long long ComparedStartIndex = 0, unsigned long long Length = 0, unsigned long long StartIndex = 0) const;
    };
}

#endif

