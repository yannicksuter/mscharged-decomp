#include "Game/Sys/simpleparser.h"
#include "Game/TweakRegistry.h"
#include "Game/TweakValue.h"

#include "NL/nlConfig.h"
#include "NL/nlFile.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"

#include <cstring>
#include <stdlib.h>

typedef Config::String BString;

extern "C" void fn_8004F594(int category, const char* format, ...);

class UnidentifiedTweakParser_8052BF40 : public Config::Parser
{
public:
    virtual void EmptyLine();
    virtual void Comment(const char* comment, unsigned int size);
    virtual void Section(const BString& section);
    virtual void TagValuePair(const BString& tag, const BString& value);

    BString mCategory;
    BString mCurrentSection;
    bool mTweaked;
    float mTweakMinValue;
    float mTweakMaxValue;
    float mTweakIncrement;
};

static char sTweakMinimum[] = "min";
static char sTweakMaximum[] = "max";
static char sTweakIncrement[] = "inc";
static char sTweakMarker[] = "tweak";
static char sCommentSeparators[] = "# =\t";
static char sSectionSeparator[] = "/";
static char sLineEndCharacters[] = "\n\r";
static char sTagValueSeparator[] = "=";
static char sConfigWhitespace[] = " \t\"\r";

void UnidentifiedTweakParser_8052BF40::Comment(
    const char* comment, unsigned int size)
{
    mTweaked = false;

    char* copy = (char*)nlMalloc(size + 1, 8, true);
    memcpy(copy, comment, size);
    copy[size] = 0;

    SimpleParser tokens;
    if (!tokens.StartParsing(copy, size + 1, sCommentSeparators))
    {
        delete copy;
        return;
    }

    char* token = tokens.NextToken(false);
    while (token != 0)
    {
        char* next = tokens.NextToken(false);
        if (mTweaked)
        {
            if (nlStrICmp(token, sTweakMaximum) == 0 && next != 0)
            {
                mTweakMaxValue = (float)atof(token);
            }
            else if (nlStrICmp(token, sTweakMinimum) == 0 && next != 0)
            {
                mTweakMinValue = (float)atof(token);
            }
            else if (nlStrICmp(token, sTweakIncrement) == 0 && next != 0)
            {
                mTweakIncrement = (float)atof(token);
            }
        }
        else if (nlStrICmp(token, sTweakMarker) == 0)
        {
            mTweaked = true;
        }
        token = next;
    }

    delete copy;
}

void UnidentifiedTweakParser_8052BF40::Section(const BString& section)
{
    BString separator(sSectionSeparator);
    mCurrentSection = mCategory.Append(separator.Append(section));
}

void UnidentifiedTweakParser_8052BF40::TagValuePair(
    const BString& tag, const BString& value)
{
    TweakEntry_8052BF00* entry
        = fn_802C4504(fn_802C0E30(), mCurrentSection.c_str(), false);
    TweakNode_8052BEB0* node = fn_802C5884(entry, tag.c_str());
    if (node == 0)
    {
        fn_802C2080(entry, tag.c_str(), value.c_str());
    }
    else
    {
        node->m_Value->UnidentifiedVirtual28(value.c_str());
    }
}

#pragma dont_inline on
extern "C" void fn_802C6828(
    char* data, int size, Config::Parser* parser)
{
    if (size <= 0)
    {
        return;
    }

    SimpleLineReader lines;
    SimpleParser tokens;
    lines.SetBuffer(data, size);
    char* line = lines.GetLine();
    BString tag;
    BString value;
    BString section;

    while (line != 0)
    {
        if (line[0] == '#')
        {
            parser->Comment(line, lines.GetSize());
            line = lines.GetLine();
            continue;
        }

        lines.GetSize();
        if (line[0] == '[' && line[lines.GetSize() - 1] == ']')
        {
            char sectionMarkers[3] = "[]";
            section.erase(section.begin(), section.end());
            section.insert(section.begin(), line, line + lines.GetSize());
            section.TrimInPlace(sLineEndCharacters);
            section.TrimInPlace(sectionMarkers);
            parser->Section(section);
            line = lines.GetLine();
            continue;
        }

        if (!tokens.StartParsing(line, lines.GetSize(), sTagValueSeparator))
        {
            line = lines.GetLine();
            continue;
        }

        char* token = tokens.NextToken(false);
        tag.erase(tag.begin(), tag.end());
        tag.insert(tag.begin(), token, token + tokens.GetTokenLength());
        tag.TrimInPlace(sConfigWhitespace);

        token = tokens.NextToken(false);
        if (token != 0)
        {
            value.erase(value.begin(), value.end());
            value.insert(value.begin(), token, token + tokens.GetTokenLength());
            value.TrimInPlace(sConfigWhitespace);

            for (int i = 0; i < value.size(); ++i)
            {
                if (value[i] == '#')
                {
                    value[i] = 0;
                    value = BString(value.c_str());
                    value.TrimInPlace(sConfigWhitespace);
                }
            }

            parser->TagValuePair(tag, value);
        }
        line = lines.GetLine();
    }
}
#pragma dont_inline reset

extern "C" char* fn_802C6BB0(
    const char* filename, unsigned long* outSize)
{
    fn_8004F594(0x13, "reading config file: %s\n", filename);

    char* buffer = 0;
    nlFile* file = nlOpen(filename);
    unsigned int bufferSize = 0;
    unsigned long dataSize = 0;
    if (file != 0)
    {
        dataSize = nlFileSize(file, &bufferSize);
        nlClose(file);
    }

    bufferSize += bufferSize == dataSize ? 0x20 : 0;
    *outSize = dataSize;

    if (dataSize != 0)
    {
        buffer = (char*)nlMalloc(bufferSize, 0x20, true);
        nlLoadEntireFile(filename, outSize, 0x20, AllocateEnd, buffer,
            bufferSize, 0);
        buffer[*outSize] = 0;
    }
    if (buffer != 0)
    {
        return buffer;
    }
    return 0;
}

extern "C" void fn_802C6CAC(
    const char* filename, const char* category, bool reload)
{
    unsigned long size;
    char* data = fn_802C6BB0(filename, &size);

    UnidentifiedTweakParser_8052BF40 parser;
    if (category != 0)
    {
        parser.mCategory = category;
        parser.mCurrentSection = category;
    }

    fn_802C6828(data, size, &parser);
    nlFree(data);
}

extern "C" void fn_802C7018(void* context, char* data,
    unsigned long size, const char* category)
{
    UnidentifiedTweakParser_8052BF40 parser;
    if (category != 0)
    {
        parser.mCategory = category;
        parser.mCurrentSection = category;
    }
    fn_802C6828(data, size, &parser);
}

void UnidentifiedTweakParser_8052BF40::EmptyLine()
{
}

bool g_bSupportReloading;
const char* lbl_806E1E90;

static TweakValueBoolImpl_804F4538 sSupportReloading(
    "g_bSupportReloading", "General", &g_bSupportReloading, true);
