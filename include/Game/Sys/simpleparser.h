#ifndef GAME_SYS_SIMPLEPARSER_H
#define GAME_SYS_SIMPLEPARSER_H

class SimpleLineReader
{
public:
    SimpleLineReader();
    void SetBuffer(char* data, int size);
    char* GetLine();
    unsigned int GetSize() const;

private:
    /* 0x000 */ char mLine[256];
    /* 0x100 */ char* mCurrent;
    /* 0x104 */ int mAmountLeft;
    /* 0x108 */ unsigned int mLineLength;
}; // size: 0x10C

class SimpleParser
{
public:
    SimpleParser();
    bool StartParsing(char* data, int size, const char* separators);
    char* NextToken(bool toLower);
    char* NextTokenOnLine(bool toLower);
    bool AdvanceLine();
    int GetTokenLength() const;

private:
    inline bool AdvanceEnd();
    inline bool AdvanceLineInternal();
    inline bool NextChar();
    inline bool SkipToEOL();
    inline bool IsSeparator(char value) const;

    /* 0x000 */ char mTokenBuffer[5][256];
    /* 0x500 */ char* mCurrent;
    /* 0x504 */ char* mEndOfLine;
    /* 0x508 */ const char* mSeparators;
    /* 0x50C */ int mAmountLeft;
    /* 0x510 */ int mCurrentBuffer;
    /* 0x514 */ int mTokenLength;
}; // size: 0x518

#endif // GAME_SYS_SIMPLEPARSER_H
