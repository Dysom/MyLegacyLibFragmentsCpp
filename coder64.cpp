#include "coder64.h"

namespace coder64
{
    byteArray TruncateFast(const byteArray & bytes)
    {
        int fullSteps = bytes.length() / 3;
        int lastBytesCount = bytes.length() % 3;
        int endingInPosition = fullSteps * 3;
        int endingOutPosition = fullSteps * 4;

        int outputBytesLength = endingOutPosition + (lastBytesCount < 1 ? lastBytesCount : (lastBytesCount + 1));

        byteArray outputBytes(outputBytesLength);

        for (int s = 0; s < fullSteps; s++)
        {
            int inPosition = s * 3;
            int outPosition = s * 4;

            outputBytes[outPosition] = (bytes[inPosition] & 0x3F);
            outputBytes[outPosition + 1] = (bytes[inPosition + 1] & 0x3F);
            outputBytes[outPosition + 2] = (bytes[inPosition + 2] & 0x3F);
            outputBytes[outPosition + 3] = (((bytes[inPosition] & 0xC0) >> 2) | ((bytes[inPosition + 1] & 0xC0) >> 4) | ((bytes[inPosition + 2] & 0xC0) >> 6));
        }

        switch (lastBytesCount)
        {
            case 0:
                break;
            case 1:
                outputBytes[endingOutPosition] = (bytes[endingInPosition] & 0x3F);
                outputBytes[endingOutPosition + 1] = ((bytes[endingInPosition] & 0xC0) >> 2);
                break;
            case 2:
                outputBytes[endingOutPosition] = (bytes[endingInPosition] & 0x3F);
                outputBytes[endingOutPosition + 1] = (bytes[endingInPosition + 1] & 0x3F);
                outputBytes[endingOutPosition + 2] = (((bytes[endingInPosition] & 0xC0) >> 2) | ((bytes[endingInPosition + 1] & 0xC0) >> 4));
                break;
            default:
                break;
        }

        return outputBytes;
    }

    byteArray UntruncateFast(const byteArray & bytes)
    {
        int fullSteps = bytes.length() / 4;
        int lastBytesCount = bytes.length() % 4;
        int endingInPosition = fullSteps * 4;
        int endingOutPosition = fullSteps * 3;

        int outputBytesLength = endingOutPosition + (lastBytesCount <= 1 ? lastBytesCount : (lastBytesCount - 1));

        byteArray outputBytes(outputBytesLength);

        for (int s = 0; s < fullSteps; s++)
        {
                int inPosition = s * 4;
                int outPosition = s * 3;

                outputBytes[outPosition] = ((bytes[inPosition] & 0x3F) | ((bytes[inPosition + 3] << 2) & 0xC0));
                outputBytes[outPosition + 1] = ((bytes[inPosition + 1] & 0x3F) | ((bytes[inPosition + 3] << 4) & 0xC0));
                outputBytes[outPosition + 2] = ((bytes[inPosition + 2] & 0x3F) | ((bytes[inPosition + 3] << 6) & 0xC0));
        }

        switch (lastBytesCount)
        {
        case 0:
                break;
        case 1:
                outputBytes[endingOutPosition] = (bytes[endingInPosition] & 0x3F);
                break;
        case 2:
                outputBytes[endingOutPosition] = ((bytes[endingInPosition] & 0x3F) | ((bytes[endingInPosition + 1] << 2) & 0xC0));
                break;
        case 3:
                outputBytes[endingOutPosition] = ((bytes[endingInPosition] & 0x3F) | ((bytes[endingInPosition + 2] << 2) & 0xC0));
                outputBytes[endingOutPosition + 1] = ((bytes[endingInPosition + 1] & 0x3F) | ((bytes[endingInPosition + 2] << 4) & 0xC0));
                break;
        default:
                break;
        }

        return outputBytes;
    }

    string buildStringFromTruncatedBytes(const byteArray & bytes, const string & ABC)
    {
        string outputString;

        outputString.resize(bytes.length());

        if (ABC.length() >= 64)
            for (int b = 0; b < bytes.length(); b++)
                outputString[b] = ABC[bytes[b]];

        return outputString;
    }

    string buildStringFromTruncatedBytes(const byteArray & bytes)
    {
        return buildStringFromTruncatedBytes(bytes, defaultABC);
    }

    byteArray getTruncatedBytes(const string & inputString, const string & ABC)
    {
        int ABCLength = ABC.length();
        int inputStringLength = inputString.length();

        byteArray outputBytes(inputStringLength);

        for (int b = 0; b < inputStringLength; b++)
        {
            outputBytes[b] = 0;

            for (int ch = 0; ch < ABCLength; ch++)
            {
                if (inputString[b] == ABC[ch])
                {
                    outputBytes[b] = ch;

                    break;
                }
            }
        }

        return outputBytes;
    }
}