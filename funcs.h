#ifndef FUNCS_H
#define FUNCS_H

#include <iostream>
#include <string>
#include <chrono>
#include <map>
#include <vector>
#include <set>

#include "barray.h"
#include "bignum.h"

using std::ostream;
using std::string;
using std::chrono::high_resolution_clock;
using std::map;
using std::vector;
using std::set;

using barray::byteArray;

typedef unsigned char byte;

byte * newConcatBytes(const byte * firstBytes, unsigned int firstBytesLength, const byte * secondBytes, unsigned int secondBytesLength);

byte * newCopyBytes(const byte * sourceBytes, unsigned int sourceBytesLength);

void copyFromTo(const byte * sourceBytes, byte * distBytes, unsigned int bytesLength);

void showHexView(const byte * bytes, unsigned int bytesLength, char separator = ' ');

ostream & operator<<(ostream & os, const byteArray & barr);

byteArray ULLtoByteArray(unsigned long long num, unsigned int output_bytes_count = 8);

unsigned long long byteArrayToULL(const byteArray & bytes);

std::string ULLtoHexString(unsigned long long num, bool setPrefix = true);

unsigned long long hexStringToULL(const std::string & hexStr);

byteArray hexStringToByteArray(const std::string & HexStr);

std::string ULLtoStringByBase(unsigned long long num, byte base);

std::string byteToHexString(byte num);

std::string byteArrayToHexString(const byteArray & bytes);

std::string numToStringByBase(const bignum & num, byte base);

bignum numericStringByBaseToNum(const std::string & numStr, byte base);

std::string byteArrayToBinaryString(const byteArray & bytes, bool wsdivider = true );

byteArray vectorToByteArray(const vector<byteArray> & V);

vector<byteArray> byteArrayToVector(const byteArray & Bytes, unsigned BlockLength, unsigned long long Shift = 0);

unsigned long long decStringToULL(const std::string & decStr);

double decStringToDouble(const std::string & DecStr);

unsigned long long getMicrosecondsSinceEpoch1970();

unsigned int getSecondsSinceEpoch1970();

bool byteArrayHasZeroByte(const byteArray & bytes);

std::string getRandomMixedABC64(unsigned int seed = 0);

byteArray & fillRandom(byteArray & bytes, unsigned int count = 0, unsigned int pos = 0);

std::string wstring_to_string(const std::wstring wstr);

std::wstring string_to_wstring(const std::string & str);

byteArray getHashSha256OfFile(const std::string & FilePath);

std::string getStringFromVector(const vector<std::string> & V, const std::string & Separator);

map<std::string, std::string> getEmlHeaders(const std::string & EmlDataText);

namespace procserver_aux
{
    std::string normalize_system_folder_path(const std::string & FolderPath);
}

template <typename T>
bool vectorHasValue(vector<T> V, T Val)
{
    bool flag= false;
    
    for(auto & v : V)
    {
        if(v == Val)
        {
            flag = true;
            break;
        }
    }
    
    return flag;
}

template <typename T>
set<T> vectorToSet(vector<T> V)
{
    set<T> S;
    
    for(auto & v : V)    
        S.insert(v);    
    
    return S;
}

template <typename T>
vector<T> setToVector(set<T> S)
{
    vector<T> v;
    
    for(auto & s : S)
        v.push_back(s);        
    
    return v;
}

class byteArrayLineReader
{
private:
    unsigned int pos;
    const byteArray & bytes;
public:
    bool outputWithCRLF;
    byteArrayLineReader(const byteArray & inBytes) : bytes(inBytes), pos(0), outputWithCRLF(true){}
    bool canRead() const { return pos < bytes.length(); }
    void reset() { pos = 0; }
    byteArray readLine();
    unsigned getPos() const { return pos; }
};

class TextLineReader
{
private:
    unsigned pos = 0;
    const std::string & text;
    const std::string textR;
public:
    TextLineReader(const std::string & Text) : text(Text) {};
    TextLineReader(std::string && Text) : textR(Text), text(textR) {};
    bool canRead() const {return pos < text.length();}
    void reset() {pos = 0;}
    std::string readLine();
    vector<std::string> getAllLines() const;
};

std::string getexepath();

void zsleep(int ms);

std::string intToDecStr(int num);

std::string streamToHexStr(std::istream * stream, bool Prefix = true);

byteArray streamToByteArray(std::istream * stream);

namespace string_ext
{
    int textStartWith(const std::string & text, const std::string & fragment, bool ignoreCase = false, bool trimStart = false, int offset = 0);
    
    bool textEndWith(const std::string & Text, const std::string & Fragment);
    
    int getPosFromTextAfterFragment(const std::string & text, const std::string & fragment, bool ignoreCase = false, bool ignoreWhiteSpaceOnStartRowInText = false, int offset = 0);
    
    std::string urldecode(const std::string & str);
    
    std::string urlencode(const std::string & str);
    
    std::string removeAllSymbolsInText(char symbol, const std::string & text);
    
    std::string replaceAllFragmentsInText(const std::string & fragment, const std::string & toFragment, const std::string & text);
    
    map<std::string, std::string> getDictionaryFromText(const std::string & text, const std::string & pairSeparator, const std::string & nameValueSeparator);    
    
    vector<std::string> getVectorFromText(const std::string & text, const std::string & separator, bool relaxed = true);
    
    std::string toLowerCase(const std::string & str);
    
    std::string toUpperCase(const std::string & str);
    
    std::string trimStart(const std::string & str);
    
    std::string trimEnd(const std::string & str);
    
    std::string trim(const std::string & str);
    
    std::string reverse(const std::string & str);
    
    std::string substrBeforeSymbol(const std::string & str, char sym);
    
    std::string substrAfterSymbol(const std::string & str, char sym);
    
    bool textContainsWord(const std::string & text, const std::string & word);
    
    std::string getFileNameFromPath(const std::string & FullPath);
    
    std::string getDirectoryPathFromPath(const std::string & FullPath);
    
    bool validateDomainName(const std::string & DomainName);
    
    std::string getExtensionFromFileName(const std::string & FileName);
    
    bool isFolderPath(const std::string & FullPath);
    
    std::string normalizeWhiteSpace(const std::string & InStr);
    
    std::string setPathAsFolderPath(const std::string & Path);
    
    bool isMailBoxName(const std::string & MailBoxName);
    
    bool hasMailBoxName(const std::string & MailBoxName);
    
    std::string getMailBoxName(const std::string & TextWithMailBoxName);
    
    std::string byteArrayToHexadecimalString(const byteArray & Bytes, const std::string & Splitter = " ");
    
    char getSymOfNumericABC64byNum(byte Num);
    
    byte getNumBySymOfNumericABC64(char Sym);
    
    const std::string & getNumericABC64();
}

template <typename T>
class IntegerIncrementatorObj
{
private:
    T & ivar;
public:
    IntegerIncrementatorObj(T & Ivar) : ivar(Ivar)
    {
        ivar++;
    }    
    ~IntegerIncrementatorObj()
    {
        ivar--;
    }
};

namespace aux_funcs
{
    unsigned int get_length(byte byt0, byte byt1, byte byt2, byte byt3);

    void set_length(unsigned int length, byte & byt0, byte & byt1, byte & byt2, byte & byt3);
    
    std::string getFolderStringRepresentation(const std::string & FolderPath, bool L = false);
}

class PrecTimer
{
private:
    bool measures;
    bool measured;
    high_resolution_clock::time_point startTime;
    high_resolution_clock::time_point stopTime;
public:
    PrecTimer() : measures(false), measured(false) {};
    PrecTimer & Start();
    PrecTimer & Stop();
    PrecTimer & Reset();
    double getSeconds();
};

class TimeInterval;

class DateTime
{
public:
    enum format {typical, adesk, mysql, shortdate, shorttime, classic, mail};
    enum month {jun = 1, feb = 2, mar = 3, apr = 4, may = 5, june = 6, july = 7, aug = 8, sep = 9, oct = 10, nov = 11, dec = 12};
private:    
    friend class TimeInterval;
    long long secondsSinceEpoch1970;    
    int DaysSinceBeginYear() const;
    bool isBissextileYear() const {return ((Year() - 1972) % 4 == 0);};
    month getNumMonthFromString(const std::string & str);
public:
    DateTime(const std::string & DateTime, format DateTimeFormat = adesk);
    DateTime(long long SecondsSinceEpoch1970) : secondsSinceEpoch1970(SecondsSinceEpoch1970) {};
    DateTime() : secondsSinceEpoch1970(time(NULL)) {};
    DateTime & Year(int year);
    int Year() const;
    int GetDays() const;
    long long GetSeconds() const;
    long long GetMinutes() const;
    long long GetHours() const;
    int Seconds() const;
    int Minutes() const;
    int Hours() const;
    DateTime & Seconds(int seconds);
    DateTime & Minutes(int minutes);
    DateTime & Hours(int hours);
    int NumDayOfMonth() const;
    month Month() const;
    DateTime & SetDate(month numMonth, int numDay);
    std::string ToString(format Format = typical) const;
    DateTime & AddHours(int hours);
    DateTime & SubtractHours(int hours);
    long long differenceInSeconds(const DateTime & DT) {return secondsSinceEpoch1970 - DT.secondsSinceEpoch1970;}
    int differenceInMinutes(const DateTime & DT) {return differenceInSeconds(DT) / 60;}
    int differenceInHours(const DateTime & DT) {return differenceInSeconds(DT) / 3600;}
    int differenceInDays(const DateTime & DT) {return differenceInSeconds(DT) / 86400;}
    bool operator>(const DateTime & DT) {return secondsSinceEpoch1970 > DT.secondsSinceEpoch1970;};
    bool operator<(const DateTime & DT) {return secondsSinceEpoch1970 < DT.secondsSinceEpoch1970;};
    bool operator==(const DateTime & DT) {return secondsSinceEpoch1970 == DT.secondsSinceEpoch1970;};
    bool operator>=(const DateTime & DT) {return *this > DT || *this == DT;};
    bool operator<=(const DateTime & DT) {return *this < DT || *this == DT;};
    bool operator!=(const DateTime & DT) {return !(*this == DT);};
    TimeInterval operator-(const DateTime & DT);
    DateTime UTC(int H) const {return DateTime(secondsSinceEpoch1970 + 3600 * H);};
    long long toLL() const {return secondsSinceEpoch1970;};
};

class TimeInterval
{
private:
    friend class DateTime;
    DateTime beginDT;
    DateTime endDT;
    TimeInterval() {};
public:
    TimeInterval(const DateTime & BeginDT, const DateTime & EndDT) : beginDT(BeginDT), endDT(EndDT) {};
    TimeInterval(unsigned BeginSecondsSinceEpoch1970, unsigned EndSecondsSinceEpoch1970) : beginDT(BeginSecondsSinceEpoch1970), endDT(EndSecondsSinceEpoch1970) {};
    long long durationInSeconds() const;
    int durationInMinutes() const;
    int durationInHours() const;
    int durationInDays() const;
    DateTime & beginTime() {return beginDT;}
    DateTime & endTime() {return endDT;}
    const DateTime & beginTime() const {return beginDT;}
    const DateTime & endTime() const {return endDT;}
};

#endif