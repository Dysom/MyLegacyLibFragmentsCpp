#include <string>
#include "barray.h"


namespace barray
{
    byte falseByte = 0;

    byteArray::byteArray(unsigned long long inBytesLength, const byte * inBytes)
    {
            bytesLength = inBytesLength;

            bytes = bytesLength > 0 ? new byte[bytesLength] : NULL;

            for (unsigned long long b = 0; b < bytesLength; b++)
                    bytes[b] = inBytes == NULL ? 0 : inBytes[b];

    }

    byteArray::byteArray(const byteArray & bA)
    {
            copyBytes(bA);
    }

    byteArray::byteArray(byteArray && bA)
    {
        bytesLength = bA.bytesLength;
        bytes = bA.bytes;

        bA.bytes = nullptr;
    }

    byteArray::byteArray(const std::string & str)
    {
        bytesLength = str.length();

        bytes = bytesLength ? new byte[bytesLength] : NULL;

        for (unsigned int byt = 0; byt < str.length(); byt++)		
            bytes[byt] = (const byte)str[byt];
    }

    byteArray::byteArray(const std::initializer_list<byte> & list)
    {
        bytesLength = list.size();

        bytes = bytesLength ? new byte[bytesLength] : NULL;

        unsigned int byt = 0;

        for (auto & val : list)
            bytes[byt++] = val;
    }

    byteArray::~byteArray()
    {
        delete[] bytes;

        bytes = NULL;
    }

    unsigned long long byteArray::length() const
    {
        return bytesLength;
    }

    byte & byteArray::operator[](int index) const
    {
            if (index < 0 || index >= bytesLength)
                    return falseByte;
            else
                    return bytes[index];
    }

    void byteArray::copyBytes(const byteArray & bA)
    {
            bytesLength = bA.bytesLength;

            bytes = new byte[bytesLength];

            for (int b = 0; b < bytesLength; b++)
                    bytes[b] = bA.bytes[b];
    }

    byteArray & byteArray::operator=(const byteArray & bA)
    {
            if (this != &bA)
            {
                    delete[] bytes;

                    copyBytes(bA);
            }

            return *this;
    }

    byteArray & byteArray::operator=(byteArray && bA)
    {
        if (this != &bA)
        {
            delete[] bytes;

            bytesLength = bA.bytesLength;

            bytes = bA.bytes;

            bA.bytes = nullptr;
            bA.bytesLength = 0;
        }

        return *this;
    }

    byteArray & byteArray::operator=(const std::string & str)
    {
            delete[] bytes;

            bytesLength = str.length();

            bytes = bytesLength ? new byte[bytesLength] : NULL;

            for (unsigned int byt = 0; byt < str.length(); byt++)
                    bytes[byt] = (const byte)str[byt];

            return *this;
    }

    void byteArray::copyBytesTo(byte * dist, unsigned long long length, unsigned long long startIndexSource, unsigned long long startIndexDist) const
    {
            for (int b = 0; b < length; b++)
                    if (bytesLength > b + startIndexSource)
                            dist[b + startIndexDist] = bytes[b + startIndexSource];
    }

    void byteArray::copyBytesTo(byteArray & dist, unsigned long long length, unsigned long long startIndexSource, unsigned long long startIndexDist) const
    {
            copyBytesTo(dist.bytes, length, startIndexSource, startIndexDist);
    }

    void byteArray::copyBytesFrom(const byte * source, unsigned long long length, unsigned long long startIndexDist, unsigned long long startIndexSource)
    {
            if (length > bytesLength - startIndexDist)
                    length = bytesLength - startIndexDist;

            for (int b = 0; b < length; b++)
                    bytes[b + startIndexDist] = source[b + startIndexSource];
    }

    void byteArray::copyBytesFrom(const byteArray & source, unsigned long long length, unsigned long long startIndexDist, unsigned long long startIndexSource)
    {
            copyBytesFrom(source.bytes, length, startIndexDist, startIndexSource);
    }

    void byteArray::clear()
    {
            delete[] bytes;

            bytes = NULL;

            bytesLength = 0;
    }

    void byteArray::setLength(unsigned long long inBytesLength)
    {
        if (bytes == NULL)
        {
            bytes = inBytesLength ? new byte[inBytesLength]{ 0 } : NULL;
        }
        else
        {
            byte * tempBytes = bytes;

            bytes = inBytesLength ? new byte[inBytesLength]{ 0 } : NULL;

            for (unsigned long long b = 0; b < inBytesLength && b < bytesLength; b++)
                bytes[b] = tempBytes[b];

            delete[] tempBytes;
        }

        bytesLength = inBytesLength;
    }

    byteArray byteArray::getBytes(unsigned long long cBytesLength, unsigned long long startIndex) const
    {
            byteArray outBytes(cBytesLength);

            copyBytesTo(outBytes, cBytesLength, startIndex, 0);

            return outBytes;
    }

    byteArray byteArray::getBytesBeginWith(unsigned long long index) const
    {
            return getBytes(length() - index, index);
    }

    byte * byteArray::bytesPTR()
    {
            return bytes;
    }

    const byte * byteArray::bytesPTR() const
    {
            return bytes;
    }

    byteArray byteArray::operator+(const byteArray & addBytes) const
    {
            byteArray rBytes(bytesLength + addBytes.bytesLength);

            for (int b = 0; b < bytesLength; b++)
                    rBytes[b] = bytes[b];

            for (int b = bytesLength, bs = 0; b < rBytes.bytesLength; b++, bs++)
                    rBytes[b] = addBytes[bs];

            return rBytes;
    }

    byteArray byteArray::operator^(const byteArray & bA) const
    {
        byteArray rBytes = (bytesLength > bA.bytesLength) ? *this : bA;

        auto & minlBytes = (bytesLength > bA.bytesLength) ? bA : *this;

        for(unsigned long long byt = 0; byt < minlBytes.length(); byt++)            
            rBytes[byt] ^= minlBytes[byt];

        return rBytes;
    }

    byteArray byteArray::operator&(const byteArray & bA) const
    {
        byteArray rBytes = (bytesLength > bA.bytesLength) ? *this : bA;

        auto & minlBytes = (bytesLength > bA.bytesLength) ? bA : *this;

        for(unsigned long long byt = 0; byt < minlBytes.length(); byt++)            
            rBytes[byt] &= minlBytes[byt];

        return rBytes;
    }

    byteArray byteArray::operator|(const byteArray & bA) const
    {
        byteArray rBytes = (bytesLength > bA.bytesLength) ? *this : bA;

        auto & minlBytes = (bytesLength > bA.bytesLength) ? bA : *this;

        for(unsigned long long byt = 0; byt < minlBytes.length(); byt++)            
            rBytes[byt] |= minlBytes[byt];

        return rBytes;
    }

    byteArray & byteArray::operator+=(const byteArray & bytes)
    {
        *this = *this + bytes;

        return *this;
    }

    std::string byteArray::ToString() const
    {
        std::string str = "";
        
        if (bytes != NULL)
        {
            unsigned strLength = bytesLength;
            
            unsigned byt;
            
            for (byt = 0; byt < bytesLength; byt++)
            {
                if(!bytes[byt])
                {
                    strLength = byt;
                    
                    break;
                }
            }
            
            str.resize(strLength);
            
            for (byt = 0; byt < strLength; byt++)
            {                
                str[byt] = bytes[byt];             
            }
        }

        return str;
    }

    bool byteArray::ToBool() const
    {
        bool flag = false;

        if (bytes != NULL)	
        {
            for (unsigned int byt = 0; byt < bytesLength; byt++)
            {
                if(bytes[byt])
                {
                    flag = true;

                    break;
                }
            }
        }

        return flag;
    }

    byteArray::operator std::string() const
    {
            return ToString();
    }

    byteArray & byteArray::setAllTo(byte val)
    {
            for (unsigned long long byt = 0; byt < bytesLength; byt++)
                    bytes[byt] = val;

            return *this;
    }

    bool byteArray::operator==(const byteArray & bA) const
    {
            bool flag = bytesLength == bA.bytesLength;

            if (flag)
            {
                    for (unsigned long long byt = 0; byt < bytesLength; byt++)
                    {
                            if (bytes[byt] != bA.bytes[byt])
                            {
                                    flag = false;
                                    break;
                            }
                    }
            }

            return flag;
    }

    bool byteArray::operator!=(const byteArray & bA) const
    {
            return !operator==(bA);
    }

    bool byteArray::operator<(const byteArray & bA) const
    {
        bool flag, br = false;

        for(unsigned long long byt = 0; byt < bytesLength && byt < bA.bytesLength; byt++)
        {
            if(bytes[byt] == bA.bytes[byt])
                continue;
            else
            {
                br = true;
                flag = bytes[byt] < bA.bytes[byt];
                break;
            }
        }

        if(!br)
            flag = bytesLength < bA.bytesLength;

        return flag;
    }

    byteArray & byteArray::setTo(byte val, unsigned long long length, unsigned long long startIndex)
    {
        if(startIndex < bytesLength)
        {
            unsigned long long pos = startIndex;

            while(pos < bytesLength && pos < length)                
                bytes[pos++] = val;
        }

        return *this;
    }
    
    long long byteArray::firstPosOf(byte Val, long long Shift) const
    {
        long long pos = -1;
        
        for(long long b = Shift; b < bytesLength; b++)
        {
            if(bytes[b] == Val)
            {
                pos = b;
                
                break;
            }
        }
        
        return pos;
    }
    
    bool byteArray::isEqualRange(const byteArray & Compared, unsigned long long ComparedStartIndex, unsigned long long Length, unsigned long long StartIndex) const
    {
        bool flag = false;
        
        if(!Length)
            Length = bytesLength;
        
        if(ComparedStartIndex + Length <= Compared.length() && StartIndex + Length <= bytesLength)
        {
            flag = true;
            
            for(unsigned long long cPos = ComparedStartIndex, pos = StartIndex; cPos < Compared.length() && pos < bytesLength; cPos++, pos++)
            {
                if(Compared[cPos] != bytes[pos])
                {
                    flag = false;
                    
                    break;
                }
            }
        }
        
        return flag;
    }
}