#include "Game/Sys/simpleparser.h"

#include "NL/nlString.h"

static inline bool Contains(const char* characters, char value)
{
    while (*characters != 0)
    {
        if (*characters == value)
        {
            return true;
        }
        ++characters;
    }
    return false;
}

SimpleLineReader::SimpleLineReader()
{
    mAmountLeft = 0;
}

void SimpleLineReader::SetBuffer(char* data, int size)
{
    mCurrent = data;
    mAmountLeft = size;
}

char* SimpleLineReader::GetLine()
{
    if (mAmountLeft == 0)
    {
        return 0;
    }

    for (;;)
    {
        if (!Contains(" \r\t\n", *mCurrent))
        {
            break;
        }
        ++mCurrent;
        if (--mAmountLeft == 0)
        {
            return 0;
        }
    }

    mLineLength = 0;
    for (;;)
    {
        mLine[mLineLength] = *mCurrent;
        ++mLineLength;
        --mAmountLeft;
        ++mCurrent;
        if (mAmountLeft == 0 || mLineLength == 0xFE)
        {
            break;
        }
        if (*mCurrent == '\n')
        {
            ++mCurrent;
            --mAmountLeft;
            break;
        }
    }

    for (;;)
    {
        if (!Contains(" \r\t", mLine[mLineLength - 1]))
        {
            break;
        }
        --mLineLength;
    }

    mLine[mLineLength] = 0;
    return mLine;
}

SimpleParser::SimpleParser()
{
    mEndOfLine = 0;
    mCurrentBuffer = 0;
}

inline bool SimpleParser::AdvanceEnd()
{
    if (mAmountLeft <= 1)
    {
        return false;
    }

    ++mEndOfLine;
    --mAmountLeft;
    return true;
}

inline bool SimpleParser::NextChar()
{
    if (mCurrent == mEndOfLine)
    {
        return false;
    }

    ++mCurrent;
    return true;
}

inline bool SimpleParser::SkipToEOL()
{
    do
    {
        if (!AdvanceEnd())
        {
            return false;
        }
    } while (*mEndOfLine != '\n');
    return true;
}

inline bool SimpleParser::IsSeparator(char value) const
{
    return Contains(mSeparators, value);
}

bool SimpleParser::StartParsing(char* data, int size, const char* separators)
{
    mEndOfLine = data;
    mAmountLeft = size;
    mSeparators = separators;

    if (mAmountLeft <= 1)
    {
        return false;
    }

    for (;;)
    {
        if (IsSeparator(*mEndOfLine))
        {
            if (!AdvanceEnd())
            {
                return false;
            }
        }
        else if (*mEndOfLine == '#')
        {
            if (!SkipToEOL())
            {
                return false;
            }
        }
        else
        {
            break;
        }
    }

    mCurrent = mEndOfLine;
    while (AdvanceEnd() && *mEndOfLine != '\n')
    {
    }
    return true;
}

inline bool SimpleParser::AdvanceLineInternal()
{
    if (mAmountLeft <= 1)
    {
        return false;
    }

    for (;;)
    {
        if (IsSeparator(*mEndOfLine))
        {
            if (!AdvanceEnd())
            {
                return false;
            }
        }
        else if (*mEndOfLine == '#')
        {
            if (!SkipToEOL())
            {
                return false;
            }
        }
        else
        {
            break;
        }
    }

    mCurrent = mEndOfLine;
    while (AdvanceEnd() && *mEndOfLine != '\n')
    {
    }
    return true;
}

char* SimpleParser::NextToken(bool toLower)
{
    char* token = NextTokenOnLine(toLower);
    if (token == 0)
    {
        if (!AdvanceLineInternal())
        {
            return 0;
        }
        token = NextTokenOnLine(toLower);
    }
    return token;
}

char* SimpleParser::NextTokenOnLine(bool toLower)
{
    if (mCurrent == mEndOfLine)
    {
        return 0;
    }

    for (;;)
    {
        if (*mCurrent == '#')
        {
            mCurrent = mEndOfLine;
            return 0;
        }

        if (!IsSeparator(*mCurrent))
        {
            break;
        }

        if (!NextChar())
        {
            return 0;
        }
    }

    mTokenLength = 0;
    for (;;)
    {
        if (toLower)
        {
            mTokenBuffer[mCurrentBuffer][mTokenLength] = nlToLower<char>(*mCurrent);
        }
        else
        {
            mTokenBuffer[mCurrentBuffer][mTokenLength] = *mCurrent;
        }
        ++mTokenLength;

        if (!NextChar())
        {
            break;
        }

        if (*mCurrent == 0)
        {
            mEndOfLine = mCurrent;
            mAmountLeft = 0;
            break;
        }

        if (IsSeparator(*mCurrent))
        {
            break;
        }
    }

    mTokenBuffer[mCurrentBuffer][mTokenLength] = 0;
    int previousBuffer = mCurrentBuffer;
    mCurrentBuffer = (previousBuffer + 1) % 5;
    return mTokenBuffer[previousBuffer];
}

bool SimpleParser::AdvanceLine()
{
    return AdvanceLineInternal();
}
