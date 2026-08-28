#include "NL/nlConfig.h"

#include "Game/Sys/simpleparser.h"
#include "NL/nlFile.h"
#include "NL/nlMemory.h"
#include "ctype.h"
#include "cstring"

extern "C" double atof(const char* string);
extern "C" void fn_8004F594(int category, const char* format, ...);
typedef Config::String BString;

static char sBoolTrue[] = "true";
static char sBoolYes[] = "yes";
static char sBoolOn[] = "on";
static char sBoolEnable[] = "enable";
static char sBoolFalse[] = "false";
static char sBoolNo[] = "no";
static char sBoolOff[] = "off";
static char sBoolDisable[] = "disable";
static char sLineEndCharacters[] = "\n\r";
static char sTagValueSeparator[] = "=";
static char sConfigWhitespace[] = " \t\"\r";
static char sSectionSeparator[] = "/";

struct SetTagValuePair : public Config::Parser
{
    SetTagValuePair(Config& config)
        : mConfig(config)
        , mTweaked(false)
    {
    }

    virtual void Comment(const char*, unsigned int);
    virtual void Section(const BString& section);
    virtual void TagValuePair(const BString& tag, const BString& value);

    BString mCurrentSection;
    Config& mConfig;
    bool mTweaked;
    float mTweakMinValue;
    float mTweakMaxValue;
    float mTweakIncrement;
};

struct ConfigLoadCallback
{
    ConfigLoadCallback(Config* owner, const Function<Config*>& completed)
        : config(owner)
        , callback(completed)
    {
    }

    Config* config;
    Function<Config*> callback;
};

unsigned int Config::Hash(const char* string) const
{
    unsigned int hash = 0x1505;
    while (*string != 0)
    {
        signed char c = (signed char)nlToUpper<char>(*string++);
        hash = hash + (hash << 5) + c;
    }
    return hash;
}

static inline unsigned int ConfigCapacity(const Config* config)
{
    return config->mTvpCapacity;
}

static inline Config::TagValuePair* FindConfigTvp(Config* config, const char* tag)
{
    Config::TagValuePair* pair;
    const char* stored;
    Config::TagValuePair* table;
    unsigned int index;
    const char* input;
    char inputChar;
    char storedChar;
    const char* cursor = tag;
    unsigned int hash = 0x1505;
    while (*cursor != 0)
    {
        signed char c = (signed char)nlToUpper<char>(*cursor++);
        hash = hash + (hash << 5) + c;
    }
    table = config->mTvpHash;
    index = hash % config->mTvpCapacity;
    while (true)
    {
        stored = table[index].tag;
        pair = &table[index];
        if (stored == 0)
        {
            goto found;
        }
        input = tag;
        do
        {
            storedChar = *stored++;
            if (storedChar >= 0x61 && storedChar <= 0x7A)
            {
                storedChar = (char)(storedChar & 0x5F);
            }
            inputChar = *input++;
            if (inputChar >= 0x61 && inputChar <= 0x7A)
            {
                inputChar = (char)(inputChar & 0x5F);
            }
        } while (storedChar != 0 && inputChar != 0 && storedChar == inputChar);
        if (storedChar - inputChar == 0)
        {
            goto found;
        }
        ++index;
        index %= ConfigCapacity(config);
    }
found:
    return pair;
}

// The parsed-value branches retain a distinct inlined lookup form.
static inline Config::TagValuePair* FindParsedConfigTvp(Config* config, const char* tag)
{
    Config::TagValuePair* table;
    char storedChar;
    char inputChar;
    const char* input;
    unsigned int index;
    const char* stored;
    Config::TagValuePair* pair;
    unsigned int hash = 0x1505;
    const char* cursor = tag;
    while (*cursor != 0)
    {
        signed char c = (signed char)nlToUpper<char>(*cursor++);
        hash = hash + (hash << 5) + c;
    }
    table = config->mTvpHash;
    index = hash % config->mTvpCapacity;
    while (true)
    {
        stored = table[index].tag;
        pair = &table[index];
        if (stored == 0)
        {
            goto found;
        }
        input = tag;
        do
        {
            storedChar = *stored++;
            if (storedChar >= 0x61 && storedChar <= 0x7A)
            {
                storedChar = (char)(storedChar & 0x5F);
            }
            inputChar = *input++;
            if (inputChar >= 0x61 && inputChar <= 0x7A)
            {
                inputChar = (char)(inputChar & 0x5F);
            }
        } while (storedChar != 0 && inputChar != 0 && storedChar == inputChar);
        if (storedChar - inputChar == 0)
        {
            goto found;
        }
        ++index;
        index %= ConfigCapacity(config);
    }
found:
    return pair;
}

Config::Config(AllocateWhere where, unsigned int stringCapacity, unsigned int slotCount)
{
    mLoaded = false;
    mStringCapacity = stringCapacity;
    mTvpCapacity = slotCount;

    if (where == ALLOCATE_LOW)
    {
        mTvpHash = new (nlMalloc(sizeof(TagValuePair) * mTvpCapacity + 0x10, 8, false))
            TagValuePair[mTvpCapacity];
        mStringMemory = (char*)nlMalloc(stringCapacity, 8, false);
    }
    else
    {
        mTvpHash = new (nlMalloc(sizeof(TagValuePair) * mTvpCapacity + 0x10, 8, true))
            TagValuePair[mTvpCapacity];
        mStringMemory = (char*)nlMalloc(stringCapacity, 8, true);
    }

    mStringEnd = mStringMemory;
    mStringMemory[mStringCapacity - 1] = 0;
}

Config::~Config()
{
    if (mTvpHash != 0)
    {
        ::operator delete[]((char*)mTvpHash - 0x10);
    }
    ::operator delete[](mStringMemory);
}

Config& Config::Global()
{
    static Config global(ALLOCATE_LOW, 0x2800, 0x400);
    return global;
}

void Config::LoadFromFile(const char* filename)
{
    fn_8004F594(0x13, "reading config file: %s\n", filename);

    unsigned long size = 0;
    void* loaded = nlLoadEntireFile(filename, &size, 0x20, AllocateEnd, 0, 0, 0);
    char* buffer;
    if (loaded != 0)
    {
        buffer = (char*)loaded;
    }
    else
    {
        buffer = 0;
    }

    {
        SetTagValuePair parser(*this);
        Parse(buffer, size, parser);
        mLoaded = true;
    }
    nlFree(buffer);
}

static void ConfigLoadComplete(void* buffer, unsigned long size, void* userData);

void Config::LoadFromFileAsync(const char* filename, const Function<Config*>& callback)
{
    void* storage = nlMalloc(sizeof(ConfigLoadCallback), 8, true);
    ConfigLoadCallback* data = ::new (storage) ConfigLoadCallback(this, callback);
    nlLoadEntireFileAsync(filename, ConfigLoadComplete, data, 0x20, AllocateEnd, 0, 0, 0);
}

static void ConfigLoadComplete(void* buffer, unsigned long size, void* userData)
{
    Config* config = ((ConfigLoadCallback*)userData)->config;
    ConfigLoadCallback* data = (ConfigLoadCallback*)userData;
    {
        SetTagValuePair parser(*config);
        config->Parse((const char*)buffer, size, parser);
        config->mLoaded = true;
    }
    nlFree(buffer);

    if (data->callback)
    {
        data->callback(data->config);
    }
    delete data;
}

bool Config::Exists(const char* tag) const
{
    unsigned int start;
    const char* input;
    char inputChar;
    char storedChar;
    TagValuePair* table;
    const char* stored;
    unsigned int index;
    unsigned int hash = Hash(tag);
    table = mTvpHash;
    start = hash % mTvpCapacity;
    index = start;
    do
    {
        stored = table[index].tag;
        if (stored == 0)
        {
            return false;
        }
        input = tag;
        do
        {
            storedChar = *stored++;
            if (storedChar >= 0x61 && storedChar <= 0x7A)
            {
                storedChar = (char)(storedChar & 0x5F);
            }
            inputChar = *input++;
            if (inputChar >= 0x61 && inputChar <= 0x7A)
            {
                inputChar = (char)(inputChar & 0x5F);
            }
        } while (storedChar != 0 && inputChar != 0 && storedChar == inputChar);
        if (storedChar - inputChar == 0)
        {
            return true;
        }
        ++index;
        index %= mTvpCapacity;
    } while (index != start);
    return false;
}

Config::TagValuePair& Config::FindTvp(const char* tag)
{
    TagValuePair* pair;
    const char* storedTag;
    unsigned int index = Hash(tag) % mTvpCapacity;
    while (true)
    {
        pair = &mTvpHash[index];
        storedTag = pair->tag;
        if (storedTag == 0 || nlStrICmp<char>(storedTag, tag) == 0)
        {
            return *pair;
        }
        ++index;
        index %= ConfigCapacity(this);
    }
}

char* Config::CopyString(const char* string, bool makeUpperCase)
{
    char* result = mStringEnd;
    while (*string != 0)
    {
        if (mStringEnd + nlStrLen<char>(string) - mStringMemory >= mStringCapacity - 1)
        {
            return result;
        }

        if (makeUpperCase)
        {
            *mStringEnd = nlToUpper<char>(*string);
            ++string;
            ++mStringEnd;
        }
        else
        {
            *mStringEnd = *string;
            ++string;
            ++mStringEnd;
        }
    }

    *mStringEnd = 0;
    ++mStringEnd;
    return result;
}

void Config::Set(const char* tag, int value)
{
    TagValuePair* pair = FindConfigTvp(this, tag);
    pair->type = CONFIG_INT;
    pair->value.intValue = value;
    if (pair->tag == 0)
    {
        pair->tag = CopyString(tag, true);
    }
}

void Config::Set(const char* tag, bool value)
{
    TagValuePair* pair = FindConfigTvp(this, tag);
    pair->type = CONFIG_BOOL;
    pair->value.boolValue = value;
    if (pair->tag == 0)
    {
        pair->tag = CopyString(tag, true);
    }
}

void Config::Set(const char* tag, float value)
{
    TagValuePair* pair = FindConfigTvp(this, tag);
    pair->type = CONFIG_FLOAT;
    pair->value.floatValue = value;
    if (pair->tag == 0)
    {
        pair->tag = CopyString(tag, true);
    }
}

static inline bool IsIntegerValue(const char* string, int& value)
{
    const char* current = string;
    while (*current != 0)
    {
        char c = *current;
        if (!isdigit(c) && *current != '-')
        {
            return false;
        }
        current++;
    }
    value = (int)atof(string);
    return true;
}

static inline bool IsFloatValue(const char* string, float& value)
{
    bool seenPeriod = false;
    const char* current = string;
    while (*current != 0)
    {
        if (*current == '.' || *current == ',')
        {
            seenPeriod = true;
        }
        else if (!isdigit(*current) && *current != '-')
        {
            return false;
        }
        current++;
    }
    value = (float)atof(string);
    return seenPeriod;
}

void Config::Set(const char* tag, const char* value)
{
    TagValuePair* pair;
    bool boolValue = false;
    float floatValue = 0.0f;
    int intValue;

    if (IsIntegerValue(value, intValue))
    {
        pair = FindParsedConfigTvp(this, tag);
        pair->type = CONFIG_INT;
        pair->value.intValue = intValue;
        if (pair->tag == 0)
        {
            pair->tag = CopyString(tag, true);
        }
    }
    else if (IsFloatValue(value, floatValue))
    {
        pair = FindParsedConfigTvp(this, tag);
        pair->type = CONFIG_FLOAT;
        pair->value.floatValue = floatValue;
        if (pair->tag == 0)
        {
            pair->tag = CopyString(tag, true);
        }
    }
    else if (IsBool(value, boolValue))
    {
        const bool parsedValue = boolValue;
        pair = FindParsedConfigTvp(this, tag);
        pair->type = CONFIG_BOOL;
        pair->value.boolValue = parsedValue;
        if (pair->tag == 0)
        {
            pair->tag = CopyString(tag, true);
        }
    }
    else
    {
        pair = FindConfigTvp(this, tag);
        pair->type = CONFIG_STRING;
        pair->value.stringValue = CopyString(value, false);
        if (pair->tag == 0)
        {
            pair->tag = CopyString(tag, true);
        }
    }
}

void Config::Set(const char* tag, const String& value)
{
    Set(tag, value.c_str());
}

void* ConfigParserAllocate(unsigned long size, unsigned int alignment, bool fromEnd);

#pragma dont_inline on
void Config::Parse(const char* data, int size, Parser& parser)
{
    if (size == 0)
    {
        return;
    }

    char* copy = (char*)ConfigParserAllocate(size + 1, 8, false);
    memcpy(copy, data, size);
    copy[size] = 0;

    SimpleLineReader lines;
    SimpleParser tokens;
    lines.SetBuffer(copy, size);
    char* line = lines.GetLine();
    BString tag;
    BString value;
    BString section;
    int pairCount = 0;

    while (line != 0)
    {
        if (pairCount == (int)mTvpCapacity)
        {
            break;
        }

        if (line[0] == '#')
        {
            parser.Comment(line, lines.GetSize());
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
            parser.Section(section);
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

            if (tag.size() > 0 && value.size() > 0)
            {
                parser.TagValuePair(tag, value);
                ++pairCount;
            }
        }
        line = lines.GetLine();
    }

    nlFree(copy);
}
#pragma dont_inline reset

#pragma inline_depth(8)
#pragma inline_max_size(0x10000)
void SetTagValuePair::TagValuePair(const BString& tag, const BString& value)
{
    BString tagWithSection(tag);
    int sectionLength = mCurrentSection.mData != 0 ? mCurrentSection.mData->mData.mSize - 1 : 0;
    if (sectionLength > 0)
    {
        tagWithSection = mCurrentSection.Append(sSectionSeparator).Append(tag);
    }
    mConfig.Set<BString>(tagWithSection.c_str(), value);
}

void SetTagValuePair::Section(const BString& section)
{
    mCurrentSection = section;
}

void SetTagValuePair::Comment(const char*, unsigned int)
{
}
#pragma inline_depth()
#pragma inline_max_size()

void* ConfigParserAllocate(unsigned long size, unsigned int alignment, bool fromEnd)
{
    return nlMalloc(size, alignment, fromEnd);
}

inline Config::TagValuePair::TagValuePair()
    : tag(0)
{
}

template <typename T>
void Config::Set(const char* tag, T value)
{
    Set(tag, value);
}

template void Config::Set<BString>(const char*, BString);

#pragma inline_depth(8)
#pragma inline_max_size(0x10000)
static inline char& ConfigStringAt(BString& string, int index)
{
    if (string.mData == 0)
    {
        string.mData = new BString::Data((const char*)0, (const char*)0);
    }
    else
    {
        string.mData = string.mData->Cow();
    }
    return string.mData->mData.mData[index];
}

static inline bool ConfigStringEquals(const BString& lhs, const char* rhs, int dataIsNull)
{
    unsigned int c;
    BString::Data* data = lhs.mData;
    int i = 0;
    while (i < (dataIsNull == 0 ? data->mData.mSize - 1 : 0))
    {
        c = (u8)*rhs;
        if ((char)c == 0)
        {
            return false;
        }
        if ((char)c != data->mData.mData[i])
        {
            return false;
        }
        ++rhs;
        ++i;
    }
    return *rhs == 0;
}

bool Config::IsBool(const char* string, bool& value) const
{
    BString lowered(string);
    int dataIsNull;
    for (int i = 0;
        i < ((dataIsNull = lowered.mData == 0) == 0 ? lowered.mData->mData.mSize - 1 : 0);
        ++i)
    {
        ConfigStringAt(lowered, i) = tolower(ConfigStringAt(lowered, i));
    }

    if (ConfigStringEquals(lowered, sBoolTrue, dataIsNull)
        || ConfigStringEquals(lowered, sBoolYes, dataIsNull)
        || ConfigStringEquals(lowered, sBoolOn, dataIsNull)
        || ConfigStringEquals(lowered, sBoolEnable, dataIsNull))
    {
        value = true;
        return true;
    }
    if (ConfigStringEquals(lowered, sBoolFalse, dataIsNull)
        || ConfigStringEquals(lowered, sBoolNo, dataIsNull)
        || ConfigStringEquals(lowered, sBoolOff, dataIsNull)
        || ConfigStringEquals(lowered, sBoolDisable, dataIsNull))
    {
        value = false;
        return false;
    }
    return false;
}
#pragma inline_depth()
#pragma inline_max_size()

unsigned int SimpleLineReader::GetSize() const
{
    return mLineLength;
}

int SimpleParser::GetTokenLength() const
{
    return mTokenLength;
}
