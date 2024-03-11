
#include "funcs.h"
#include "file.h"
#include "shell_output.h"
#include "sha.h"

#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <cctype>
#include <cmath>
#include <codecvt>
#include <regex>

#include <time.h>
#include <unistd.h>
#include <vector>
#include <locale>


using dpext::cout;
using std::endl;
using std::dec;
using std::hex;

using namespace std::chrono;

extern map<std::string, std::string> systemFolders;

static const std::string defaultNumericABC = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

static const std::string defaultABC64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZ-abcdefghijklmnopqrstuvwxyz_0123456789";

static const std::string numericABC64 = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!@";

const std::string hex_digs = "0123456789abcdef";



byte * newConcatBytes(const byte * firstBytes, unsigned int firstBytesLength, const byte * secondBytes, unsigned int secondBytesLength)
{
	byte * output = new byte[firstBytesLength + secondBytesLength];

	unsigned int b;

	for (b = 0; b < firstBytesLength; b++)
		output[b] = firstBytes[b];

	for (b = 0; b < secondBytesLength; b++)
		output[b + firstBytesLength] = secondBytes[b];

	return output;
}

std::string getRandomMixedABC64(unsigned int seed)
{
    std::string ABC = defaultABC64;
    
    std::string resultABC = "";
    
    byte num;
    
    char S;
    
    srand(seed ? seed : time(NULL));
    
    while(ABC.length())
    {
        num = rand() % ABC.length();
        
        S = ABC[num];
        
        ABC.erase(num, 1);
        
        resultABC += S;
    }
    
    return resultABC;
}

byte * newCopyBytes(const byte * sourceBytes, unsigned int sourceBytesLength)
{
	byte * output = new byte[sourceBytesLength];

	for (unsigned int b = 0; b < sourceBytesLength; b++)
		output[b] = sourceBytes[b];

	return output;
}

void copyFromTo(const byte * sourceBytes, byte * distBytes, unsigned int bytesLength)
{
	for (unsigned int b = 0; b < bytesLength; b++)
		distBytes[b] = sourceBytes[b];
}

void showHexView(const byte * bytes, unsigned int bytesLength, char separator)
{
	cout << hex;

	for (unsigned int byt = 0; byt < bytesLength; byt++)
	{
		cout << (bytes[byt] < 16 ? "0" : "") << int(bytes[byt]);
		if (separator)
			cout << separator;
	}	

	cout << dec << endl;
}

ostream & operator<<(ostream & os, const byteArray & barr)
{
	cout << hex;

	for (unsigned int byt = 0; byt < barr.length(); byt++)	
		cout << (byt > 0 ? " " : "") << (barr.bytesPTR()[byt] < 16 ? "0" : "") << short(barr.bytesPTR()[byt]);

	cout << dec;

	return os;
}

byteArray ULLtoByteArray(unsigned long long num, unsigned int output_bytes_count)
{
    byteArray outBytes(output_bytes_count);

    outBytes.setAllTo(0);

    for (unsigned int b = 0; (b < 8) && (b < outBytes.length()); b++)
        outBytes[outBytes.length() - 1 - b] = (num >> (b * 8)) & 255;

    return outBytes;
}

unsigned long long byteArrayToULL(const byteArray & bytes)
{
    unsigned long long num = 0;
    
    for(int b = 0; (b < 8) && (b < bytes.length()); b++)    
        num += ((unsigned long long)bytes[bytes.length() - 1 - b]) << (b * 8);    
    
    return num;
}

std::string ULLtoHexString(unsigned long long num, bool setPrefix)
{
        std::string outStr = (setPrefix ? "0x" : "");

        byte cByte;
        
        bool f = false;
        
	for (int b = 7; b >= 0; b--)
        {
                cByte = (num >> (b * 8)) & 255;
            
		if(cByte)
                {
                    outStr += defaultNumericABC[cByte / 16];
                    outStr += defaultNumericABC[cByte % 16];
                    
                    f = true;
                }
                else if(f || !b)                
                    outStr += "00";                
        }

	return outStr;
}

unsigned long long hexStringToULL(const std::string & hexStr)
{
    std::string hStr = string_ext::toLowerCase(hexStr);

    unsigned long long result = 0;

    if (hStr.length())
    {
        int pos = hStr.length() - 1, mult = 0, fpos;

        while (pos >= 0)
            if ((fpos = hex_digs.find(hStr[pos--])) != std::string::npos && mult < 16)
                result += (unsigned long long)fpos << (4 * mult++);
    }

    return result;
}

byteArray hexStringToByteArray(const std::string & HexStr)
{
    std::string hStr = string_ext::toLowerCase(HexStr);

    byteArray bytes;

    if (hStr.length())
    {
        int pos = 0, fPos;
        bool odd = false;

        for (int pos = hStr.length() - 1; pos >= 0; pos--)
        {
            if ((fPos = hex_digs.find(hStr[pos])) >= 0)
            {
                if(odd)
                {
                    bytes[0] += (byte)(fPos << 4);
                }
                else
                {
                    bytes = byteArray(1) + bytes;
                    
                    bytes[0] = (byte)fPos;
                }
                
                odd = !odd;
            }
            else if(!isspace(hStr[pos]))
            {
                bytes.clear();
                
                break;
            }
        }
    }

    return bytes;
}

std::string ULLtoStringByBase(unsigned long long num, byte base)
{
    std::string outStr = (num ? "" : "0");
    
    if(base <= defaultNumericABC.length())
    {
        while(num)
        {
            outStr = defaultNumericABC[num % base] + outStr;

            num = num / base;
        }
    }
    
    return outStr;
}

std::string byteToHexString(byte num)
{
    static const char digs[17] = "0123456789ABCDEF";
    
    std::string outStr = "";
    outStr += digs[num / 16];
    outStr += digs[num % 16];
    
    return outStr;
}

std::string byteArrayToHexString(const byteArray & bytes)
{
    std::string outStr = "";
    
    for(int b = 0; b < bytes.length(); b++)
    {
        outStr += defaultNumericABC[bytes[b] / 16];
        outStr += defaultNumericABC[bytes[b] % 16];
    }
    
    return outStr;
}

std::string numToStringByBase(const bignum & num, byte base)
{
    std::string outStr = "";
    
    if(base <= defaultNumericABC.length() && !num.isEmpty())
    {
        bignum tNum = num;
        
        bignum baseNum;
        
        baseNum = (sbyte)base;
        
        do
        {
            outStr = defaultNumericABC[(tNum % baseNum)[0]] + outStr;
            tNum = tNum / baseNum;            
        }
        while(tNum != 0);
    }
    
    return outStr;
}

bignum numericStringByBaseToNum(const std::string & numStr, byte base)
{
    bignum outNum;
    
    if(base <= defaultNumericABC.length())
    {
        outNum = (sbyte)0;
        
        int pos = numStr.length() - 1, s;
        
        bignum baseNum, multNum;
        
        baseNum = (sbyte)base;
        
        multNum = (sbyte)1;
        
        bignum digNum;
        
        while(pos >= 0)
        {
            digNum = (sbyte)base;
            
            for(s = 0; s < base; s++)
            {
                if(numStr[pos] == defaultNumericABC[s])
                {
                    digNum = (sbyte)s;
                    break;
                }
            }
            
            if(digNum != (sbyte)base)
            {
                outNum = outNum + (digNum * multNum);
                
                multNum = multNum * baseNum;
            }
            
            pos--;
        }        
    }
    
    return outNum;
}

std::string byteArrayToBinaryString(const byteArray & bytes, bool wsdivider)
{
    string output = "";

    for (unsigned long long b = 0; b < bytes.length(); b++)
    {
        if (b && wsdivider)
            output += " ";

        output += bytes[b] & 128 ? "1" : "0";
        output += bytes[b] & 64 ? "1" : "0";
        output += bytes[b] & 32 ? "1" : "0";
        output += bytes[b] & 16 ? "1" : "0";
        output += bytes[b] & 8 ? "1" : "0";
        output += bytes[b] & 4 ? "1" : "0";
        output += bytes[b] & 2 ? "1" : "0";
        output += bytes[b] & 1 ? "1" : "0";
    }

    return output;
}

unsigned long long decStringToULL(const std::string & decStr)
{
    unsigned long long result = 0;
    
    int pos = decStr.length() - 1;
    byte s;
        
    unsigned long long multNum = 1;

    byte digNum;

    while(pos >= 0)
    {
        digNum = 10;

        for(s = 0; s < 10; s++)
        {
            if(decStr[pos] == defaultNumericABC[s])
            {
                digNum = (sbyte)s;
                break;
            }
        }

        if(digNum != 10)
        {
            result += (digNum * multNum);

            multNum *= 10;
        }

        pos--;
    }        
    
    return result;
}

double decStringToDouble(const std::string & DecStr)
{
    double output = 0;
    
    int pointPos = DecStr.find('.');
    
    if(pointPos > 0)
    {
        output = double(decStringToULL(DecStr.substr(0, pointPos)));
        
        output += double(decStringToULL(DecStr.substr(pointPos + 1))) / std::pow(10, (DecStr.length() - pointPos - 1));
    }
    else
        output = double(decStringToULL(DecStr));
    
    return output;
}

bool byteArrayHasZeroByte(const byteArray & bytes)
{
    for(unsigned long long pos = 0; pos < bytes.length(); pos++)
    {
        if(!bytes[pos])        
            return true;        
    }
    
    return false;
}

byteArray vectorToByteArray(const vector<byteArray> & V)
{
    byteArray bytes;
    
    if(V.size())
    {
        unsigned long long bytesCount = 0;
        
        for(auto & i : V)
        {
            bytesCount += i.length();
        }
        
        bytes.setLength(bytesCount);
        
        bytesCount = 0;
        
        for(auto & i : V)
        {
            bytes.copyBytesFrom(i, i.length(), bytesCount);
            
            bytesCount += i.length();
        }
    }
    
    return bytes;
}

vector<byteArray> byteArrayToVector(const byteArray & Bytes, unsigned BlockLength, unsigned long long Shift)
{
    vector<byteArray> v;
    
    if(Shift < Bytes.length() && !((Bytes.length() - Shift) % BlockLength))
    {
        for(unsigned long long b = Shift; b < Bytes.length(); b += BlockLength)        
            v.push_back(Bytes.getBytes(BlockLength, b));        
    }
        
    return v;
}

byteArray & fillRandom(byteArray & bytes, unsigned int count, unsigned int pos)
{
    if(!count)
        count = bytes.length();
    
    srand(getMicrosecondsSinceEpoch1970() & 0xFFFFFFFF);
    
    for(unsigned int b = pos; b < bytes.length() && b < count; b++)    
        bytes[b] = rand() % 256;    
    
    return bytes;
}

std::string getStringFromVector(const vector<std::string> & V, const std::string & Separator)
{
    std::string outStr = "";
    
    for(auto & item : V)
    {
        if(outStr.length())        
            outStr += Separator;        
        
        outStr += item;
    }
            
    return outStr;
}

map<std::string, std::string> getEmlHeaders(const std::string & EmlDataText)
{
    TextLineReader textReader(EmlDataText);

    std::string lineStr, fnameStr = "";

    map<std::string, std::string> headersDict;

    int sepPos;

    while(textReader.canRead())
    {
        lineStr = textReader.readLine();

        if(lineStr.length())
        {
            if(lineStr == ".")
            {
                break;
            }
            else if(std::isspace(lineStr[0]))
            {
                if(fnameStr.length())
                {
                    headersDict[fnameStr] += " ";

                    headersDict[fnameStr] += string_ext::trim(lineStr);
                }
            }
            else
            {
                if((sepPos = int(lineStr.find(':'))))
                {
                    fnameStr = string_ext::toLowerCase(string_ext::trim(lineStr.substr(0, sepPos)));

                    headersDict[fnameStr] = string_ext::trim(lineStr.substr(sepPos + 1));
                }
            }
        }
        else
            break;
    }
    
    return headersDict;
}

namespace procserver_aux
{
    std::string normalize_system_folder_path(const std::string & FolderPath)
    {
        if(string_ext::textStartWith(FolderPath, "<"))
        {   
            for(auto & p : systemFolders)
            {
                if(string_ext::textStartWith(FolderPath, p.first))               
                    return p.second + FolderPath.substr(p.first.length());            
            }

            return "";
        }
        else
            return FolderPath;
    }
}

byteArray byteArrayLineReader::readLine()
{
	const byte CR = '\r';
	const byte LF = '\n';

	byteArray outBytes;
	
	if (pos < bytes.length())
	{
		unsigned int startReadPos = pos;

		for (; pos < bytes.length() - 1; pos++)
		{
			if (bytes[pos] == CR && bytes[pos + 1] == LF)
			{
				pos += 2;

				break;
			}
		}

		outBytes = bytes.getBytes(pos - startReadPos - (outputWithCRLF ? 0 : 2), startReadPos);
	}

	return outBytes;
}

std::string getexepath()
{
    #ifdef WIN

    #else
    char result[ PATH_MAX ];
    ssize_t count = readlink( "/proc/self/exe", result, PATH_MAX );
    return std::string( result, (count > 0) ? count : 0 );
    #endif
}

#ifdef WIN
void zsleep(int ms)
{
	Sleep(ms);
}
#else
const timespec sleepValue1ms = {0, 1000000};
const timespec sleepValue5ms = {0, 5000000};
const timespec sleepValue10ms = {0, 10000000};
const timespec sleepValue25ms = {0, 25000000};
const timespec sleepValue50ms = {0, 50000000};
const timespec sleepValue55ms = {0, 55000000};
const timespec sleepValue60ms = {0, 60000000};
const timespec sleepValue65ms = {0, 65000000};
const timespec sleepValue70ms = {0, 70000000};
const timespec sleepValue75ms = {0, 75000000};
const timespec sleepValue100ms = {0, 100000000};
const timespec sleepValue150ms = {0, 150000000};
const timespec sleepValue250ms = {0, 250000000};
const timespec sleepValue300ms = {0, 300000000};

void zsleep(int ms)
{
    switch(ms)
    {
        case 10:
            nanosleep(&sleepValue10ms, NULL);
            break;
        case 25:
            nanosleep(&sleepValue25ms, NULL);
            break;
        case 100:
            nanosleep(&sleepValue100ms, NULL);
            break;
        case 50:
            nanosleep(&sleepValue50ms, NULL);
            break;
        case 75:
            nanosleep(&sleepValue75ms, NULL);
            break;
        case 150:
            nanosleep(&sleepValue150ms, NULL);
            break;            
        case 250:
            nanosleep(&sleepValue250ms, NULL);
            break;
        case 300:
            nanosleep(&sleepValue300ms, NULL);
            break;            
        case 55:
            nanosleep(&sleepValue55ms, NULL);
            break;
        case 60:
            nanosleep(&sleepValue60ms, NULL);
            break;
        case 65:
            nanosleep(&sleepValue65ms, NULL);
            break;
        case 70:
            nanosleep(&sleepValue70ms, NULL);
            break;        
        case 5:
            nanosleep(&sleepValue5ms, NULL);
            break;
        case 1:
            nanosleep(&sleepValue1ms, NULL);
            break;
        case 0:
            break;
        default:
            {
                while(ms > 0)
                {
                    if(ms >= 100)
                    {
                        nanosleep(&sleepValue100ms, NULL);
                        ms -= 100;
                    }
                    else if(ms >= 50)
                    {
                        nanosleep(&sleepValue50ms, NULL);
                        ms -= 50;
                    }
                    else if(ms >= 10)
                    {
                        nanosleep(&sleepValue10ms, NULL);
                        ms -= 10;
                    }
                    else if(ms >= 5)
                    {
                        nanosleep(&sleepValue5ms, NULL);
                        ms -= 5;
                    }
                    else
                    {
                        nanosleep(&sleepValue1ms, NULL);
                        ms -= 1;
                    }   
                }
            }
            break;
    }
}
#endif

std::string intToDecStr(int num)
{
    static const char digs[11] = "0123456789";
    
    bool subZero = num < 0;
    
    if(subZero)    
        num = -num;    
    
    std::string outStr = "";
    
    do
    {
        outStr += digs[num % 10];
        
        num /= 10;
    }
    while(num);
    
    int outStrLength = outStr.length();
    int outStrHalfLength = outStrLength / 2;
    
    for(int c = 0; c < outStrHalfLength; c++)
    {
        char t = outStr[c];
        outStr[c] = outStr[outStrLength - 1 - c];
        outStr[outStrLength - 1 - c] = t;
    }
    
    if(subZero)
        outStr = "-" + outStr;
    
    return outStr;
}

std::string streamToHexStr(std::istream * stream, bool Prefix)
{
    static const char digs[17] = "0123456789ABCDEF";
    
    stream->seekg(0, std::ios::end);

    int streamLength = stream->tellg();

    stream->seekg(0, std::ios::beg);

    std::string outString = "";
    
    if(streamLength > 0)
    {
        unsigned char * byteArr = new unsigned char[streamLength];

        stream->read((char *)byteArr, streamLength);

        if(Prefix)
            outString += "0x";        
        
        char ddigs[3]{digs[0]};        
        ddigs[2] = 0;

        for(int b = 0; b < streamLength; b++)
        {
            ddigs[0] = digs[((int)byteArr[b]) / 16];
            ddigs[1] = digs[((int)byteArr[b]) % 16];
            
            outString += ddigs;
        }

        delete byteArr;
    }
    
    return outString;
}

byteArray streamToByteArray(std::istream * stream)
{
    stream->seekg(0, std::ios::end);

    int streamLength = stream->tellg();

    stream->seekg(0, std::ios::beg);

    byteArray outBytes;

    if(streamLength > 0)
    {
        outBytes.setLength(streamLength);

        stream->read((char *)outBytes.bytesPTR(), streamLength);
    }
    
    return outBytes;
}

unsigned long long getMicrosecondsSinceEpoch1970()
{
    return duration_cast<std::chrono::microseconds>(system_clock::now().time_since_epoch()).count();
}

unsigned int getSecondsSinceEpoch1970()
{
    return time(NULL);
}

namespace string_ext
{
    static byte from_hex(byte ch) 
    {
        if (ch <= '9' && ch >= '0')
            ch -= '0';
        else if (ch <= 'f' && ch >= 'a')
            ch -= 'a' - 10;
        else if (ch <= 'F' && ch >= 'A')
            ch -= 'A' - 10;
        else 
            ch = 0;
        
        return ch;
    }

    std::string urldecode(const std::string & str) 
    {
        std::string result;

        std::string::size_type i;

        for (i = 0; i < str.size(); ++i)
        {
            if (str[i] == '+')
            {
                result += ' ';
            }
            else if (str[i] == '%' && str.size() > i+2)
            {
                const byte ch1 = from_hex(str[i+1]);
                const byte ch2 = from_hex(str[i+2]);
                const byte ch = (ch1 << 4) | ch2;
                result += ch;
                i += 2;
            }
            else
            {
                result += str[i];
            }
        }
        return result;
    }
    
    void hexchar(unsigned char c, unsigned char & hex1, unsigned char & hex2)
    {
        hex1 = c / 16;
        hex2 = c % 16;
        hex1 += hex1 <= 9 ? '0' : 'a' - 10;
        hex2 += hex2 <= 9 ? '0' : 'a' - 10;
    }
    
    std::string urlencode(const std::string & s)
    {
        static const char lookup[]= "0123456789ABCDEF";
        
        std::stringstream e;
        
        for(int i=0, ix=s.length(); i<ix; i++)
        {
            const char& c = s[i];
            
            if ( (48 <= c && c <= 57) ||//0-9
                 (65 <= c && c <= 90) ||//abc...xyz
                 (97 <= c && c <= 122) || //ABC...XYZ
                 (c=='-' || c=='_' || c=='.' || c=='~') 
            )
            {
                e << c;
            }
            else
            {
                e << '%';
                e << lookup[ (c&0xF0)>>4 ];
                e << lookup[ (c&0x0F) ];
            }
        }
        return e.str();        
    }
    
    int textStartWith(const std::string & text, const std::string & fragment, bool ignoreCase, bool trimStart, int offset)
    {
        int complete_index = 0;
        
        if(text.length() && fragment.length())
        {
            int text_pos = offset, fragment_pos = 0;
            
            if(trimStart)
            {
                while(fragment_pos < fragment.length() && isspace(fragment[fragment_pos]))
                    fragment_pos++;
                
                while(text_pos < text.length() && isspace(text[text_pos]))
                    text_pos++;
            }
            
            bool flag = fragment_pos < fragment.length() && text_pos < text.length();
            
            while(fragment_pos < fragment.length() && text_pos < text.length())
            {
                if(ignoreCase)                
                {
                    if(tolower(fragment[fragment_pos]) != tolower(text[text_pos]))                    
                        flag = false;
                }
                else if(fragment[fragment_pos] != text[text_pos])
                    flag = false;
                
                fragment_pos++;
                text_pos++;
                
                if(!flag)
                    break;
            }
            
            if(flag)
                complete_index = text_pos;
        }
        
        return complete_index;
    }
    
    int getPosFromTextAfterFragment(const std::string & text, const std::string & fragment, bool ignoreCase, bool ignoreWhiteSpaceOnStartRowInText, int offset)
    {
        int complete_index = 0;
        
        if(text.length() && fragment.length())
        {
            int text_pos = offset, fragment_pos = 0;
            
            bool rowStarted = true;
            
            while(fragment_pos < fragment.length() && text_pos < text.length())
            {   
                if(ignoreWhiteSpaceOnStartRowInText)
                {
                    if(rowStarted)
                    {
                        if(text[text_pos] == ' ')
                        {
                            text_pos++;
                            continue;
                        }
                        else
                            rowStarted = false;
                    }
                    else
                    {
                        if(text[text_pos] == '\n' && text[text_pos - 1] == '\r')
                            rowStarted = true;
                    }
                }
                
                if(ignoreCase)
                {
                    if(tolower(fragment[fragment_pos]) == tolower(text[text_pos]))                    
                        fragment_pos++;
                    else
                        fragment_pos = 0;
                }
                else
                {
                    if(fragment[fragment_pos] == text[text_pos])
                        fragment_pos++;
                    else
                        fragment_pos = 0;
                }
                
                text_pos++;
            }
            
            if(fragment_pos == fragment.length())
                complete_index = text_pos;
        }
        
        return complete_index;
    }
    
    std::string removeAllSymbolsInText(char symbol, const std::string & text)
    {
        std::string outStr = "";
        
        for(int pos = 0; pos < text.length(); pos++)
        {
            if(text[pos] != symbol)
                outStr += text[pos];
        }        
        
        return outStr;
    }
    
    std::string replaceAllFragmentsInText(const std::string & fragment, const std::string & toFragment, const std::string & text)
    {
        std::string outStr = "";
        
        int fPos = 0;
        
        for(int pos = 0; pos < text.length(); pos++)
        {
            if(text[pos] == fragment[fPos])                     
                fPos++;
            else
            {
                if(fPos)
                    outStr += fragment.substr(0, fPos);
                
                fPos = 0;
                
                outStr += text[pos];
            }
            
            if(fPos == fragment.length())
            {
                fPos = 0;
                
                outStr += toFragment;
            }
        }
        
        return outStr;
    }
    
    map<std::string, std::string> getDictionaryFromText(const std::string & text, const std::string & pairSeparator, const std::string & nameValueSeparator)
    {
        map<std::string, std::string> outDict;
        
        int startPos = 0;
        
        int endPos = getPosFromTextAfterFragment(text, nameValueSeparator, false, false, startPos);
                
        while(endPos)
        {
            std::string name = text.substr(startPos, endPos - startPos - 1);
            
            startPos = endPos;
            
            if(endPos = getPosFromTextAfterFragment(text, pairSeparator, false, false, startPos))
            {
                outDict[name] = text.substr(startPos, endPos - startPos - 1);
                
                startPos = endPos;
                
                endPos = getPosFromTextAfterFragment(text, nameValueSeparator, false, false, startPos);
            }
            else
                outDict[name] = text.substr(startPos);
        }
        
        return outDict;
    }
    
    vector<std::string> getVectorFromText(const std::string & text, const std::string & separator, bool relaxed)
    {
        vector<std::string> strs;
        
        int pos = 0, ppos = 0;
        
        if(text.length() && separator.length())
        {
            std::string str;

            while(pos = getPosFromTextAfterFragment(text, separator, false, false, pos))
            {
                str = text.substr(ppos, pos - ppos - 1);

                if(relaxed)
                    str = trimEnd(trimStart(str));

                strs.push_back(str);

                ppos = pos;
            }

            str = text.substr(ppos);

            if(relaxed)
                str = trimEnd(trimStart(str));

            strs.push_back(str);
        }
        
        return strs;
    }
    
    std::string toLowerCase(const std::string & str)
    {
        std::string outStr = str;
        
        for(int s = 0; s < str.length(); s++)
        {
            if(isalpha(str[s]))            
                outStr[s] = tolower(str[s]);
        }
        
        return outStr;
    }
    
    std::string toUpperCase(const std::string & str)
    {
        std::string outStr = str;
        
        for(int s = 0; s < str.length(); s++)
        {
            if(isalpha(str[s]))            
                outStr[s] = toupper(str[s]);
        }
        
        return outStr;
    }
    
    std::string trimStart(const std::string & str)
    {
        int s;
        
        for(s = 0; s < str.length(); s++)     
            if(!isspace(str[s]))
                break;
        
        return str.substr(s);
    }
    
    std::string trimEnd(const std::string & str)
    {
        int s;
        
        for(s = str.length() - 1; s >= 0; s--)
            if(!isspace(str[s]))
                break;
        
        return str.substr(0, s + 1);
    }
    
    std::string trim(const std::string & str)
    {
        return trimEnd(trimStart(str));
    }
    
    std::string reverse(const std::string & str)
    {
        std::string outStr = "";
        
        for(int s = str.length() - 1; s >= 0; s--)        
            outStr += str[s];
        
        return outStr;
    }
    
    std::string substrBeforeSymbol(const std::string & str, char sym)
    {
        std::string outStr = "";

        int fpos;

        if (str.length() && (fpos = str.find(sym)) != std::string::npos)
            outStr = str.substr(0, fpos);

        return outStr;
    }

    std::string substrAfterSymbol(const std::string & str, char sym)
    {
        std::string outStr = "";

        int fpos;

        if (str.length() && (fpos = str.find(sym)) != std::string::npos)
            outStr = str.substr(fpos + 1);

        return outStr;
    }
    
    bool textContainsWord(const std::string & text, const std::string & word)
    {
        bool flag = false;
        
        if(!text.size() || !word.size())
            return flag;
        
        int m = 0;
        
        int p = 0;
        
        for(; p < text.size() && !flag; p++)
        {
            if((m > 0 || (p == 0 || !isalpha(text[p - 1]))) && m < word.size() && text[p] == word[m])
                m++;
            else
                m = 0;
            
            if(m == word.size())
                flag = true;
        }
        
        if(flag && p < text.size() && isalpha(text[p]))
            flag = false;
        
        return flag;
    }
    
    std::string getFileNameFromPath(const std::string & FullPath)
    {
        std::string fileName = FullPath;
        
        if(FullPath.length() > 1)
        {
            for(int i = FullPath.length() - 2; i >= 0; i--)
            {
                if(FullPath[i] == '/')
                {
                    fileName = FullPath.substr(i + 1);
                    
                    break;
                }
            }
        }
        
        return fileName;
    }
    
    std::string getDirectoryPathFromPath(const std::string & FullPath)
    {
        std::string dirPath = "";
        
        if(FullPath.length())
        {
            for(int i = FullPath.length() - 1; i >= 0; i--)
            {
                if(FullPath[i] == '/')
                {
                    dirPath = FullPath.substr(0, i + 1);
                    
                    break;
                }
            }
        }
        
        return dirPath;
    }
    
    bool textEndWith(const std::string & Text, const std::string & Fragment)
    {
        bool flag = false;
        
        if(Fragment.length() && (int)Text.length() >= (int)Fragment.length())
        {
            flag = true;
            
            for(int p = Fragment.length() - 1, t = Text.length() - 1; p >= 0; p--, t--)
            {
                if(Fragment[p] != Text[t])
                {
                    flag = false;
                    
                    break;
                }
            }
        }
        
        return flag;
    }
    
    bool validateDomainName(const std::string & DomainName)
    {
        const std::regex dRegex("[\\w\\-]+(\\.[\\w\\-]+)+");
        
        return std::regex_match(DomainName.begin(), DomainName.end(), dRegex);
    }
    
    bool isMailBoxName(const std::string & MailBoxName)
    {
        const std::regex mRegex("[\\w\\-]+@[\\w\\-]+(\\.[\\w\\-]+)+");
        
        return std::regex_match(MailBoxName.begin(), MailBoxName.end(), mRegex);
    }
    
    bool hasMailBoxName(const std::string & MailBoxName)
    {
        const std::regex mRegex("[\\w\\-]+@[\\w\\-]+(\\.[\\w\\-]+)+");
        
        return std::regex_search(MailBoxName.begin(), MailBoxName.end(), mRegex);
    }
    
    std::string getMailBoxName(const std::string & TextWithMailBoxName)
    {
        const std::regex mRegex("[\\w\\-]+@[\\w\\-]+(\\.[\\w\\-]+)+");
        
        std::smatch sm;
        
        if(std::regex_search(TextWithMailBoxName.begin(), TextWithMailBoxName.end(), sm, mRegex))
        {
            return sm.str();
        }
        
        return "";
    }
    
    std::string getExtensionFromFileName(const std::string & FileName)
    {
        std::string ext = "";
        
        int pPos = int(FileName.find_last_of('.'));
        
        if(pPos >= 0 && pPos < FileName.length() - 1)
            ext = FileName.substr(pPos + 1);
        
        return ext;
    }
    
    bool isFolderPath(const std::string & FullPath)
    {
        return FullPath.length() && FullPath[FullPath.length() - 1] == '/';
    }
    
    std::string normalizeWhiteSpace(const std::string & InStr)
    {
        std::string inStr = trim(InStr);
        
        std::string outStr = "";
        
        if(inStr.length())
        {
            bool spaceStarted = false;
            
            for(int c = 0; c < inStr.length(); c++)
            {
                if(isspace(inStr[c]))
                {
                    if(!spaceStarted)
                    {
                        spaceStarted = true;
                        
                        outStr += ' ';
                    }
                }
                else
                {
                    spaceStarted = false;
                    
                    outStr += inStr[c];
                }
            }
        }
        
        return outStr;
    }
    
    std::string setPathAsFolderPath(const std::string & Path)
    {
        if(Path.length())
        {
            if(Path[Path.length() - 1] != '/')
                return Path + "/";
            else
                return Path;
        }
        
        return "/";
    }
    
    std::string byteArrayToHexadecimalString(const byteArray & Bytes, const std::string & Splitter)
    {
        std::string outStr = "";
        
        for(unsigned byt = 0; byt < Bytes.length(); byt++)
        {
            if(outStr.length())
                outStr += Splitter;
            
            outStr += byteToHexString(Bytes[byt]);
        }
        
        return outStr;
    }
    
    char getSymOfNumericABC64byNum(byte Num)
    {
        return numericABC64[Num];
    }
    
    const std::string & getNumericABC64()
    {
        return numericABC64;
    }
    
    byte getNumBySymOfNumericABC64(char Sym)
    {
        return numericABC64.find(Sym);        
    }
}

namespace aux_funcs
{
    unsigned int get_length(byte byt0, byte byt1, byte byt2, byte byt3)
    {
        return ((unsigned int)byt0 << 24) + ((unsigned int)byt1 << 16) + ((unsigned int)byt2 << 8) + byt3;
    }

    void set_length(unsigned int length, byte & byt0, byte & byt1, byte & byt2, byte & byt3)
    {
        byt0 = (length >> 24) & 255;
        byt1 = (length >> 16) & 255;
        byt2 = (length >> 8) & 255;
        byt3 = length & 255;
    }
    
    std::string getFolderStringRepresentation(const std::string & FolderPath, bool L)
    {
        vector<std::string> items = filer::readDirectory(FolderPath);

        std::string outStr = "";

        if(!items.size())
            return outStr;

        std::sort(items.begin(), items.end(), [] (const std::string & First, const std::string & Second) -> bool
        {
            if(First[First.length() - 1] == '/' || Second[Second.length() - 1] == '/')
            {
                if(First[First.length() - 1] != Second[Second.length() - 1])
                {
                    return First[First.length() - 1] == '/';
                }
            }

            return string_ext::toLowerCase(First) < string_ext::toLowerCase(Second);
        });

        if(L)
            outStr += "<";

        bool f = false;

        for(auto & item : items)
        {
            if(f)
                outStr += "|";
            else
                f = true;

            outStr += item;

            if(item[item.length() - 1] == '/')            
                outStr += getFolderStringRepresentation(FolderPath + item, true);

        }

        if(L)
            outStr += ">";

        return outStr;
    }
}

PrecTimer & PrecTimer::Start()
{
    measures = true;
    
    measured = false;
    
    startTime = high_resolution_clock::now();
    
    return *this;
}

PrecTimer & PrecTimer::Stop()
{
    if(measures)
    {
        measured = true;
        
        measures = false;
        
        stopTime = high_resolution_clock::now();
    }
    
    return *this;
}

PrecTimer & PrecTimer::Reset()
{
    if(measures)    
        startTime = high_resolution_clock::now();    
    else
        measured = false;
    
    return *this;
}

double PrecTimer::getSeconds()
{
    double outSecs = 0;
    
    if(measured)
    {
        outSecs = duration_cast<duration<double>>(stopTime - startTime).count();
    }
    else if(measures)
    {
        outSecs = duration_cast<duration<double>>(high_resolution_clock::now() - startTime).count();
    }
    
    return outSecs;
}

static const int SecondsPerYear = 31536000;
static const int SecondsPerDay = 86400;
static const int SecondsPerHour = 3600;
static const int SecondsPerMinute = 60;
static const int DaysPer4Years = 1461;


DateTime::DateTime(const std::string& DateTime, format DateTimeFormat) : secondsSinceEpoch1970(0)
{
    if(DateTimeFormat == adesk)
    {
        std::string dtStr = DateTime;
    
        if(int(dtStr.find('%')) >= 0)        
            dtStr = string_ext::urldecode(dtStr);
        
        dtStr = string_ext::toLowerCase(dtStr);
        
        int pos = 0;
        int len = string_ext::getPosFromTextAfterFragment(dtStr, ":") - 1;
        
        bool flag = true;
        
        if(len <= 2)        
            Hours(decStringToULL(dtStr.substr(pos, len)));
        else
            flag = false;
        
        pos += len + 1;
        
        len = string_ext::getPosFromTextAfterFragment(dtStr, ":", false, false, pos) - 1 - pos;
        
        if(flag && len <= 2)
        {
            Minutes(decStringToULL(dtStr.substr(pos, len)));
        }
        else
            flag = false;
        
        pos += len + 1;
        
        len = string_ext::getPosFromTextAfterFragment(dtStr, " ", false, false, pos) - 1 - pos;
        
        if(flag && len <= 2)
        {
            Seconds(decStringToULL(dtStr.substr(pos, len)));        
        }
        else
            flag = false;
        
        pos += len + 1;
        
        len = string_ext::getPosFromTextAfterFragment(dtStr, " ", false, false, pos) - 1 - pos;
        
        month dMonth;
        
        if(flag && len <= 12)
        {
            dMonth = getNumMonthFromString(dtStr.substr(pos, len));            
        }
        else
            flag = false;
        
        pos += len + 1;
        
        len = string_ext::getPosFromTextAfterFragment(dtStr, ", ", false, false, pos) - 2 - pos;
        
        if(flag && len <= 2)
        {
            SetDate(dMonth, decStringToULL(dtStr.substr(pos, len)));            
        }
        else
            flag = false;
        
        pos += len + 2;
        
        len = string_ext::getPosFromTextAfterFragment(dtStr, " ", false, false, pos) - 1 - pos;
        
        if(flag && len <= 4)
        {
            Year(decStringToULL(dtStr.substr(pos, len)));            
        }
        else
            flag = false;
        
        pos += len + 1;
        
        if(pos < dtStr.length())
        {
            std::string kindtime = dtStr.substr(pos);
            
            if(kindtime == "pst")
            {
                AddHours(8);
            }                
        }
    }
}

DateTime & DateTime::Year(int year)
{
    int Year_ = Year() - 1970;
    
    int bissextileDays = Year_ / 4;
    
    Year_ %= 4;
    
    if(Year_ == 3)
        bissextileDays ++;
    
    int days = (GetDays() - bissextileDays) % 365;
    
    Year_ = year - 1970;
    
    bissextileDays = Year_ / 4;
    
    if(Year_ % 4 == 3)
        bissextileDays ++;
    
    days += bissextileDays;
    
    days += 365 * Year_;
    
    secondsSinceEpoch1970 %= SecondsPerDay;
    
    secondsSinceEpoch1970 += days * SecondsPerDay;
    
    return *this;
}

int DateTime::Year() const
{
    int days = GetDays();
    
    int years = (days / DaysPer4Years) * 4;
    
    int rest = days % DaysPer4Years;
    
    if(rest)
    {
        if(rest >= 1096)        
            years += 3;
        else if(rest >= 730)
            years += 2;
        else if(rest >= 365)
            years ++;
    }
    
    return years + 1970;
}

int DateTime::GetDays() const
{
    return secondsSinceEpoch1970 / SecondsPerDay;
}

long long DateTime::GetSeconds() const
{
    return secondsSinceEpoch1970;
}

long long DateTime::GetMinutes() const
{
    return secondsSinceEpoch1970 / SecondsPerMinute;
}

long long DateTime::GetHours() const
{
    return secondsSinceEpoch1970 / SecondsPerHour;
}

int DateTime::Seconds() const
{
    return secondsSinceEpoch1970 % SecondsPerMinute;
}

int DateTime::Minutes() const
{
    return (secondsSinceEpoch1970 % SecondsPerHour) / SecondsPerMinute;
}

int DateTime::Hours() const
{
    return (secondsSinceEpoch1970 % SecondsPerDay) / SecondsPerHour;
}

DateTime & DateTime::Seconds(int seconds)
{
    secondsSinceEpoch1970 -= Seconds();
    secondsSinceEpoch1970 += seconds;
    
    return *this;
}

DateTime & DateTime::Minutes(int minutes)
{
    secondsSinceEpoch1970 -= Minutes() * SecondsPerMinute;
    secondsSinceEpoch1970 += minutes * SecondsPerMinute;
    
    return *this;
}

DateTime & DateTime::Hours(int hours)
{
    secondsSinceEpoch1970 -= Hours() * SecondsPerHour;
    secondsSinceEpoch1970 += hours * SecondsPerHour;
    
    return *this;
}

int DateTime::DaysSinceBeginYear() const
{
    int Year_ = Year() - 1970;
    
    int bissextileDays = Year_ / 4;
    
    Year_ %= 4;
    
    if(Year_ == 3)
        bissextileDays ++;
    
    return (GetDays() - bissextileDays) % 365;
}

int DateTime::NumDayOfMonth() const
{
    int bissextile = isBissextileYear() ? 1 : 0;
    
    int days = DaysSinceBeginYear();
    
    int dayNum;
    
    if(days < 31)
    {
        dayNum = days + 1;
    }
    else if(days < 59 + bissextile)
    {
        dayNum = days - 31 + 1;
    }
    else if(days < 90 + bissextile)
    {
        dayNum = days - (59 + bissextile) + 1;
    }
    else if(days < 120 + bissextile)
    {
        dayNum = days - (90 + bissextile) + 1;
    }
    else if(days < 151 + bissextile)
    {
        dayNum = days - (120 + bissextile) + 1;
    }
    else if(days < 181 + bissextile)
    {
        dayNum = days - (151 + bissextile) + 1;
    }
    else if(days < 212 + bissextile)
    {
        dayNum = days - (181 + bissextile) + 1;
    }
    else if(days < 243 + bissextile)
    {
        dayNum = days - (212 + bissextile) + 1;
    }
    else if(days < 273 + bissextile)
    {
        dayNum = days - (243 + bissextile) + 1;
    }
    else if(days < 304 + bissextile)
    {
        dayNum = days - (273 + bissextile) + 1;
    }
    else if(days < 334 + bissextile)
    {
        dayNum = days - (304 + bissextile) + 1;
    }
    else
    {
        dayNum = days - (334 + bissextile) + 1;
    }
    
    return dayNum;
}

DateTime::month DateTime::Month() const
{
    int bissextile = isBissextileYear() ? 1 : 0;
    
    int days = DaysSinceBeginYear();
    
    month currMonth = dec;
    
    if(days < 31)
    {
        currMonth = jun;
    }
    else if(days < 59 + bissextile)
    {
        currMonth = feb;
    }
    else if(days < 90 + bissextile)
    {
        currMonth = mar;
    }
    else if(days < 120 + bissextile)
    {
        currMonth = apr;
    }
    else if(days < 151 + bissextile)
    {
        currMonth = may;
    }
    else if(days < 181 + bissextile)
    {
        currMonth = june;
    }
    else if(days < 212 + bissextile)
    {
        currMonth = july;
    }
    else if(days < 243 + bissextile)
    {
        currMonth = aug;
    }
    else if(days < 273 + bissextile)
    {
        currMonth = sep;
    }
    else if(days < 304 + bissextile)
    {
        currMonth = oct;
    }
    else if(days < 334 + bissextile)
    {
        currMonth = nov;
    }   
    
    return currMonth;
}

DateTime & DateTime::SetDate(month numMonth, int numDay)
{
    int bissextile = isBissextileYear() ? 1 : 0;
    
    secondsSinceEpoch1970 -= DaysSinceBeginYear() * SecondsPerDay;
    
    int days = numDay - 1;
    
    switch(numMonth)
    {
        case jun:            
            break;
        case feb:
            days += 31 * SecondsPerDay;
            break;
        case mar:
            days += (59 + bissextile) * SecondsPerDay;
            break;
        case apr:
            days += (90 + bissextile) * SecondsPerDay;
            break;
        case may:
            days += (120 + bissextile) * SecondsPerDay;
            break;
        case june:
            days += (151 + bissextile) * SecondsPerDay;
            break;
        case july:
            days += (181 + bissextile) * SecondsPerDay;
            break;
        case aug:
            days += (212 + bissextile) * SecondsPerDay;
            break;
        case sep:
            days += (243 + bissextile) * SecondsPerDay;
            break;
        case oct:
            days += (273 + bissextile) * SecondsPerDay;
            break;
        case nov:
            days += (304 + bissextile) * SecondsPerDay;
            break;
        case dec:
            days += (334+ bissextile) * SecondsPerDay;
            break;
        default:
            break;
    }
    
    secondsSinceEpoch1970 += days * SecondsPerDay;
    
    return *this;
}

std::string DateTime::ToString(format Format) const
{
    static const vector<string> monthsAbbr = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    
    static const vector<string> weekDaysAbbr = {"Thu", "Fri", "Sat", "Sun", "Mon", "Tue", "Wed"};
    
    std::string outStr = "";
    
    switch(Format)
    {
        case format::mysql:
            {
                int temp;
                outStr = intToDecStr(Year()) + "-";
                temp = Month();
                if(temp < 10)
                    outStr += "0";
                outStr += intToDecStr(temp) + "-";
                temp = NumDayOfMonth();
                if(temp < 10)
                    outStr += "0";
                outStr += intToDecStr(temp) + " ";
                temp = Hours();
                if(temp < 10)
                    outStr += "0";
                outStr += intToDecStr(temp) + ":";
                temp = Minutes();
                if(temp < 10)
                    outStr += "0";
                outStr += intToDecStr(temp) + ":";
                temp = Seconds();
                if(temp < 10)
                    outStr += "0";
                outStr += intToDecStr(temp);
            }
            break;
        case format::shortdate:
            {
                int temp;
                outStr = intToDecStr(Year()) + "_";
                temp = Month();
                if(temp < 10)
                    outStr += "0";
                outStr += intToDecStr(temp) + "_";
                temp = NumDayOfMonth();
                if(temp < 10)
                    outStr += "0";
                outStr += intToDecStr(temp);                
            }
            break;
        case format::shorttime:
            {
                int temp;
                temp = Hours();
                if(temp < 10)
                    outStr += "0";
                outStr += intToDecStr(temp) + ":";
                temp = Minutes();
                if(temp < 10)
                    outStr += "0";
                outStr += intToDecStr(temp) + ":";
                temp = Seconds();
                if(temp < 10)
                    outStr += "0";
                outStr += intToDecStr(temp);
            }
            break;
        case format::mail:
        {
            int temp;
            outStr = weekDaysAbbr[GetDays() % 7] + ", ";
            temp = NumDayOfMonth();
            if(temp < 10)
                outStr += "0";
            outStr += intToDecStr(temp) + " ";
            outStr += monthsAbbr[Month() - 1] + " ";
            outStr += intToDecStr(Year()) + " ";            
            
            temp = Hours();
            if(temp < 10)
                outStr += "0";
            outStr += intToDecStr(temp) + ":";
            temp = Minutes();
            if(temp < 10)
                outStr += "0";
            outStr += intToDecStr(temp) + ":";
            temp = Seconds();
            if(temp < 10)
                outStr += "0";
            outStr += intToDecStr(temp) + " +0000";
        }
            break;
        case format::classic:
        {
            int temp;
            outStr = "";
            temp = NumDayOfMonth();
            if(temp < 10)
                outStr += "0";
            outStr += intToDecStr(temp) + "-";
            temp = Month();
            if(temp < 10)
                outStr += "0";
            outStr += intToDecStr(temp) + "-";
            outStr += intToDecStr(Year()) + " ";            
            
            temp = Hours();
            if(temp < 10)
                outStr += "0";
            outStr += intToDecStr(temp) + ":";
            temp = Minutes();
            if(temp < 10)
                outStr += "0";
            outStr += intToDecStr(temp) + ":";
            temp = Seconds();
            if(temp < 10)
                outStr += "0";
            outStr += intToDecStr(temp);
        }
            break;
        case format::typical:
        default:
            {
                int temp;
                outStr = intToDecStr(Year()) + "-";
                temp = Month();
                if(temp < 10)
                    outStr += "0";
                outStr += intToDecStr(temp) + "-";
                temp = NumDayOfMonth();
                if(temp < 10)
                    outStr += "0";
                outStr += intToDecStr(temp) + " ";
                temp = Hours();
                if(temp < 10)
                    outStr += "0";
                outStr += intToDecStr(temp) + ":";
                temp = Minutes();
                if(temp < 10)
                    outStr += "0";
                outStr += intToDecStr(temp) + ":";
                temp = Seconds();
                if(temp < 10)
                    outStr += "0";
                outStr += intToDecStr(temp) + " UTC";
            }        
            break;
    }
    
    return outStr;
}

DateTime::month DateTime::getNumMonthFromString(const std::string & str)
{
    month Num = jun;
    
    if(string_ext::textStartWith(str, "june", true, true))    
        Num = june;
    else if(string_ext::textStartWith(str, "july", true, true))    
        Num = july;
    else if(string_ext::textStartWith(str, "jun", true, true))
        Num = jun;
    else if(string_ext::textStartWith(str, "feb", true, true))    
        Num = feb;
    else if(string_ext::textStartWith(str, "mar", true, true))
        Num = mar;
    else if(string_ext::textStartWith(str, "apr", true, true))
        Num = apr;
    else if(string_ext::textStartWith(str, "may", true, true))
        Num = may;
    else if(string_ext::textStartWith(str, "aug", true, true))
        Num = aug;
    else if(string_ext::textStartWith(str, "sep", true, true))
        Num = sep;
    else if(string_ext::textStartWith(str, "oct", true, true))
        Num = oct;
    else if(string_ext::textStartWith(str, "nov", true, true))
        Num = nov;
    else if(string_ext::textStartWith(str, "dec", true, true))
        Num = dec;    
    
    return Num;
}

DateTime & DateTime::AddHours(int hours)
{
    secondsSinceEpoch1970 += hours * SecondsPerHour;
    
    return *this;
}

DateTime & DateTime::SubtractHours(int hours)
{
    secondsSinceEpoch1970 -= hours * SecondsPerHour;
    
    return *this;
}

std::string wstring_to_string(const std::wstring wstr)
{
    return std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(wstr);
}

std::wstring string_to_wstring(const std::string & str)
{    
    return std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(str);
}

TimeInterval DateTime::operator-(const DateTime & DT)
{
    TimeInterval TI;
    
    if(secondsSinceEpoch1970 >= DT.secondsSinceEpoch1970)
    {
        TI.beginDT = DT;
        
        TI.endDT = *this;
    }
    else
    {
        TI.beginDT = *this;
        
        TI.endDT = DT;
    }
    
    return TI;
}

long long TimeInterval::durationInSeconds() const
{
    return endDT.secondsSinceEpoch1970 - beginDT.secondsSinceEpoch1970;
}

int TimeInterval::durationInMinutes() const
{
    return (endDT.secondsSinceEpoch1970 - beginDT.secondsSinceEpoch1970) / 60;
}

int TimeInterval::durationInHours() const
{
    return (endDT.secondsSinceEpoch1970 - beginDT.secondsSinceEpoch1970) / 3600;
}

int TimeInterval::durationInDays() const
{
    return (endDT.secondsSinceEpoch1970 - beginDT.secondsSinceEpoch1970) / 86400;
}

std::string TextLineReader::readLine()
{
    std::string line = "";
    
    if(pos < text.length())
    {
        unsigned tPos = pos;
        
        bool lL = true;
        
        while(pos < text.length())
        {
            if(text[pos++] == '\n')
            {
                lL = false;
                break;
            }
        }
        
        bool prevR = pos > 1 && text[pos - 2] == '\r';
        
        line = lL ? text.substr(tPos, pos - tPos) : text.substr(tPos, pos - tPos - (prevR ? 2 : 1));
    }
    
    return line;
}

vector<std::string> TextLineReader::getAllLines() const
{
    vector<std::string> lines;
    
    unsigned ePos = 0;
    
    while(ePos < text.length())
    {
        unsigned tPos = ePos;
        
        bool lL = true;
        
        while(ePos < text.length())
        {
            if(text[ePos++] == '\n')
            {
                lL = false;
                break;
            }
        }
        
        bool prevR = ePos > 1 && text[ePos - 2] == '\r';
        
        lines.push_back(lL ? text.substr(tPos, ePos - tPos) : text.substr(tPos, ePos - tPos - (prevR ? 2 : 1)));
    }
    
    return lines;
}

byteArray getHashSha256OfFile(const std::string & FilePath)
{
    unsigned long long fSize = filer::getFileSize(FilePath);
    
    if(fSize)
    {
        unsigned blockSize = 100000;
        
        unsigned long long fPos = 0;
        
        sha256Digest hasher;
        
        while(fPos < fSize)
        {
            if(fSize < fPos + blockSize)
                blockSize = fSize - fPos;
            
            hasher.Update(filer::readPartOfFile(FilePath, fPos, blockSize));
            
            fPos += blockSize;
        }
        
        return hasher.Final();
    }
    
    return byteArray(0);
}
