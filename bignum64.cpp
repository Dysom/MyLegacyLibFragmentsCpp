#include "bignum64.h"

#include <exception>


const byte bignum64::cells_bytes_size = sizeof(unsigned long long);

const byte bignum64::cells_bits_size = bignum64::cells_bytes_size << 3;

const byte bignum64::maxbitnum_of_cell = bignum64::cells_bits_size - 1;

bignum64::bignum64(const bignum64 & B)
{            
    cells = new unsigned long long[B.cellsLength];
    
    cellsLength = B.cellsLength;
    
    s = B.s;
    
    for(unsigned c = 0; c < cellsLength; c++)        
        cells[c] = B.cells[c];    
}

bignum64::bignum64(bignum64 && B)
{        
    cells = B.cells;
    
    B.cells = NULL;
    
    cellsLength = B.cellsLength;
    hiddenCellsLength = B.hiddenCellsLength;    
    
    B.cellsLength = 0;
    B.hiddenCellsLength = 0;
    
    s = B.s;
}

bignum64::~bignum64()
{
    if(cellsLength || hiddenCellsLength)
        delete [] cells;
}

bignum64 & bignum64::operator=(const bignum64 & B)
{
    if(hiddenCellsLength)
        cellsLength = hiddenCellsLength;
    
    if(cellsLength < B.cellsLength)
    {
        if(cellsLength)
            delete [] cells;
            
        hiddenCellsLength = 0;
            
        cells = new unsigned long long[B.cellsLength];
    }
    else if(cellsLength > B.cellsLength)
        hiddenCellsLength = cellsLength;
    
    cellsLength = B.cellsLength;
    
    s = B.s;
    
    if(hiddenCellsLength && !B.cellsLength)
    {
        delete [] cells;
        
        cells = NULL;
        
        hiddenCellsLength = 0;
    }
    else
    {
        for(unsigned c = 0; c < cellsLength; c++)        
            cells[c] = B.cells[c];
    }
    
    return *this;
}

bignum64 & bignum64::operator=(bignum64 && B)
{
    if(cellsLength || hiddenCellsLength)
        delete [] cells;
            
    cells = B.cells;
    
    B.cells = NULL;
    
    cellsLength = B.cellsLength;
    hiddenCellsLength = B.hiddenCellsLength;    
    
    B.cellsLength = 0;
    B.hiddenCellsLength = 0;
    
    s = B.s;
    
    return *this;
}

bignum64::bignum64(const byteArray & Bytes)
{
    if(Bytes.length())
    {
        cellsLength = Bytes.length() / cells_bytes_size + ((Bytes.length() % cells_bytes_size) ? 1 : 0);
        
        cells = new unsigned long long[cellsLength];
        
        int inv_b;
        
        for(int b = Bytes.length() - 1; b >= 0; b--)
        {
            inv_b = Bytes.length() - b - 1;
            
            if(inv_b % cells_bytes_size)
                cells[inv_b / cells_bytes_size] |= ((unsigned long long)Bytes[b]) << ((inv_b % cells_bytes_size) * 8);
            else
                cells[inv_b / cells_bytes_size] = Bytes[b];
        }
    }
}

int bignum64::getMaxBitNum() const
{
    if(!cellsLength)
        return -1;
    
    int c = cellsLength - 1;
    
    for(; c > 0; c--)
    {
        if(cells[c])
            break;
    }
    
    unsigned bitNum = maxbitnum_of_cell;
    
    unsigned long long op = cells[c];    
    
    while(!(op | 0x8000000000000000ULL) && bitNum)
    {
        op <<= 1;

        bitNum--;
    }

    bitNum += ((cells_bytes_size * c) << 3);
    
    return bitNum;
}

byteArray bignum64::toByteArray() const
{
    byteArray outBytes;
    
    if(cellsLength)
    {
        unsigned maxBit = getMaxBitNum();
        
        unsigned bytesLength = (maxBit / 8) + 1;
        
        outBytes.setLength(bytesLength);
        
        int inv_b;
        
        for(int b = outBytes.length() - 1; b >= 0; b--)
        {
            inv_b = outBytes.length() - b - 1;
            
            if(inv_b % cells_bytes_size)            
                outBytes[b] = cells[inv_b / cells_bytes_size] >> ((inv_b % cells_bytes_size) * 8);            
            else            
                outBytes[b] = cells[inv_b / cells_bytes_size];            
        }
    }
    
    return outBytes;
}

bignum64 bignum64::operator-() const
{
    bignum64 bN = *this;
    
    bN.s = s < 0 ? 0 : -1;

    return bN;
}

bignum64 bignum64::abs() const
{
    bignum64 bN = *this;

    if(s < 0)    
        bN.s = 0;

    return bN;
}

unsigned bignum64::getObjectiveCellsLength() const
{
    if(!cellsLength)
        return 0;
    
    for(int c = cellsLength - 1; c >= 0; c--)
    {
        if(cells[c])
            return c + 1;
    }
    
    return 1;
}

class bn64compareWith_wocl : public std::exception
{
    virtual const char* what() const throw()
    {
        return "bignum64: cellsLength must be non zero...";
    }
} exc_cw_wocl;

char bignum64::compareWith(const bignum64 & BN) const
{
    if(cellsLength && BN.cellsLength)
    {
        if(s != BN.s)
            return s > BN.s ? 1 : -1;        
        
        unsigned cL = getObjectiveCellsLength(), bN_cL = BN.getObjectiveCellsLength();
        
        if(cL != bN_cL)
            return cL > bN_cL ? 1 : -1;
        
        for(int c = cL - 1; c >= 0; c--)
        {
            if(cells[c] != BN.cells[c])
                return cells[c] > BN.cells[c] ? 1 : -1;
        }
    }
    else
    {
        throw exc_cw_wocl;
    }
    
    return 0;
}
