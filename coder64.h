#ifndef CODER64_H
#define CODER64_H

#include <string>
#include "barray.h"

using barray::byteArray;
using std::string;

namespace coder64
{
    const std::string defaultABC = "ABCDEFGHIJKLMNOPQRSTUVWXYZ-abcdefghijklmnopqrstuvwxyz_0123456789";
    
    // "cut" 2 upper bits from each byte and replace them to specific byte (make from each 3 bytes -> 4 truncated bytes)
    byteArray TruncateFast(const byteArray & bytes);	

    byteArray UntruncateFast(const byteArray & bytes);
    
    std::string buildStringFromTruncatedBytes(const byteArray & bytes);
    std::string buildStringFromTruncatedBytes(const byteArray & bytes, const std::string & ABC);
        
    byteArray getTruncatedBytes(const std::string & inputString, const std::string & ABC = defaultABC);            
}

#endif

