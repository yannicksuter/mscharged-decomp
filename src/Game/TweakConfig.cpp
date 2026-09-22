#include "Game/Sys/simpleparser.h"
#include "Game/Sys/debug.h"
#include "Game/TweakRegistry.h"
#include "Game/TweakValue.h"

#include "Game/TweakConfig.h"
#include "NL/nlConfig.h"
#include "NL/nlFile.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"

#include <cstring>
#include <stdlib.h>
#include "NL/nlstring_tmpl.h"

typedef Config::String BString;

class TweakConfigParser : public Config::Parser
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

void TweakConfigParser::Comment(
    const char* comment, unsigned int size)
{
    char* copy;
    const char* minimum = "min";
    const char* maximum = "max";
    const char* increment = "inc";
    const char* marker = "tweak";

    mTweaked = false;

    copy = (char*)nlMalloc(size + 1, 8, true);
    memcpy(copy, comment, size);
    copy[size] = 0;

    SimpleParser tokens;
    if (!tokens.StartParsing(copy, size + 1, "# =\t"))
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
            if (nlStrICmp(token, maximum) == 0 && next != 0)
            {
                mTweakMaxValue = (float)atof(token);
            }
            else if (nlStrICmp(token, minimum) == 0 && next != 0)
            {
                mTweakMinValue = (float)atof(token);
            }
            else if (nlStrICmp(token, increment) == 0 && next != 0)
            {
                mTweakIncrement = (float)atof(token);
            }
        }
        else if (nlStrICmp(token, marker) == 0)
        {
            mTweaked = true;
        }
        token = next;
    }

    delete copy;
}

void TweakConfigParser::Section(const BString& section)
{
    BString separator("/");
    mCurrentSection = mCategory.Append(separator.Append(section));
}

void TweakConfigParser::TagValuePair(
    const BString& tag, const BString& value)
{
    TweakEntry* entry
        = FindOrCreateTweakPath(GetTweakRoot(), mCurrentSection.c_str(), false);
    TweakNode* node = FindTweakChild(entry, tag.c_str());
    if (node == 0)
    {
        CreateTweakValueFromString(entry, tag.c_str(), value.c_str());
    }
    else
    {
        node->m_Value->ParseValue(value.c_str());
    }
}

static char sLineEndCharacters[] = "\n\r";
static char sTagValueSeparator[] = "=";
static char sConfigWhitespace[] = " \t\"\r";

#pragma dont_inline on
void ParseTweakConfigData(
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

char* ReadTweakConfigFile(
    const char* filename, unsigned long* outSize)
{
    tDebugPrintManager::Print(DC_CONFIG_SYSTEM, "reading config file: %s\n", filename);

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

void LoadTweakConfigFile(
    const char* filename, const char* category, bool reload)
{
    unsigned long size;
    char* data = ReadTweakConfigFile(filename, &size);

    {
        TweakConfigParser parser;
        if (category != 0)
        {
            parser.mCategory = category;
            parser.mCurrentSection = category;
        }

        ParseTweakConfigData(data, size, &parser);
    }
    nlFree(data);
}

void LoadTweakConfigBuffer(void* context, char* data,
    unsigned long size, const char* category)
{
    TweakConfigParser parser;
    if (category != 0)
    {
        parser.mCategory = category;
        parser.mCurrentSection = category;
    }
    ParseTweakConfigData(data, size, &parser);
}

void TweakConfigParser::EmptyLine()
{
}

bool g_bSupportReloading;
const char* gLastTweakCategory;

static TweakBoolBinding sSupportReloading(
    "g_bSupportReloading", "General", &g_bSupportReloading, true);
