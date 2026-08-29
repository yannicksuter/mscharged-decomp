#ifndef NL_CONFIG_H
#define NL_CONFIG_H

#include "NL/nlBasicString.h"
#include "NL/nlDebug.h"
#include "NL/nlFunction.h"
#include "NL/nlLexicalCast.h"
#include "types.h"

extern "C" double atof(const char* string);
extern "C" int strcmp(const char* first, const char* second);

namespace Detail
{
template <>
struct LexicalCastImpl<bool, const char*>
{
    static bool Do(const char* s) { return strcmp("true", s) == 0; }
};

template <>
struct LexicalCastImpl<int, const char*>
{
    static int Do(const char* s) { return (int)atof(s); }
};

template <>
struct LexicalCastImpl<float, const char*>
{
    static float Do(const char* s) { return (float)atof(s); }
};

template <>
struct LexicalCastImpl<const char*, const char*>
{
    static const char* Do(const char* const& s) { return s; }
};

template <typename To>
struct LexicalCastImpl<To, int>
{
    static To Do(int t);
};

template <typename To>
struct LexicalCastImpl<To, float>
{
    static To Do(float t);
};

template <typename To>
struct LexicalCastImpl<To, bool>
{
    static To Do(bool t);
};

template <>
struct LexicalCastImpl<BasicString<char, TempStringAllocator>, const char*>
{
    static BasicString<char, TempStringAllocator> Do(const char* s)
    {
        return BasicString<char, TempStringAllocator>(s);
    }
};

template <typename Allocator>
inline BasicString<char, Allocator> LexicalCastImpl<BasicString<char, Allocator>, int>::Do(int t)
{
    char s[0x40];
    nlSNPrintf(s, 0x40, "%i", t);
    return BasicString<char, Allocator>(s);
}

template <typename Allocator>
inline BasicString<char, Allocator> LexicalCastImpl<BasicString<char, Allocator>, float>::Do(float t)
{
    char s[0x40];
    nlSNPrintf(s, 0x40, "%f", t);
    return BasicString<char, Allocator>(s);
}

template <typename Allocator>
inline BasicString<char, Allocator> LexicalCastImpl<BasicString<char, Allocator>, bool>::Do(bool t)
{
    if (t)
    {
        return BasicString<char, Allocator>("true");
    }
    return BasicString<char, Allocator>("false");
}

template <typename To>
inline To LexicalCastImpl<To, int>::Do(int t)
{
    return (To)t;
}

template <typename To>
inline To LexicalCastImpl<To, float>::Do(float t)
{
    return (To)t;
}

template <typename To>
inline To LexicalCastImpl<To, bool>::Do(bool t)
{
    return t;
}

template <>
inline const char* LexicalCastImpl<const char*, bool>::Do(bool value)
{
    return value ? "true" : "false";
}

template <>
inline const char* LexicalCastImpl<const char*, int>::Do(int)
{
    nlBreak();
    return 0;
}

template <>
inline const char* LexicalCastImpl<const char*, float>::Do(float)
{
    nlBreak();
    return 0;
}
} // namespace Detail

enum ConfigType
{
    CONFIG_BOOL = 0,
    CONFIG_INT = 1,
    CONFIG_FLOAT = 2,
    CONFIG_STRING = 3,
};

union ConfigValue
{
    const char* stringValue;
    int intValue;
    bool boolValue;
    float floatValue;
};

class Config
{
public:
    typedef BasicString<char, Detail::TempStringAllocator> String;

    struct TagValuePair
    {
        TagValuePair();

        template <typename T>
        T Get() const
        {
            if (type == CONFIG_BOOL)
            {
                return LexicalCast<T, bool>(value.boolValue);
            }
            else if (type == CONFIG_INT)
            {
                return LexicalCast<T, int>(value.intValue);
            }
            else if (type == CONFIG_FLOAT)
            {
                return LexicalCast<T, float>(value.floatValue);
            }
            else if (type == CONFIG_STRING)
            {
                return LexicalCast<T, const char*>(value.stringValue);
            }
            return T();
        }

        const char* tag;
        ConfigType type;
        ConfigValue value;
    };

    struct Parser
    {
        virtual void EmptyLine() { }
        virtual void Comment(const char*, unsigned int) { }
        virtual void Section(const String&) { }
        virtual void TagValuePair(const String&, const String&) { }
    };

    enum AllocateWhere
    {
        ALLOCATE_LOW = 0,
        ALLOCATE_HIGH = 1,
    };

    Config(AllocateWhere where, unsigned int stringCapacity, unsigned int slotCount);
    ~Config();

    static Config& Global();

    void LoadFromFile(const char* filename);
    void LoadFromFileAsync(const char* filename, const Function<Config*>& callback);
    unsigned int Hash(const char* string) const;
    bool Exists(const char* tag) const;
    TagValuePair& FindTvp(const char* tag);
    char* CopyString(const char* string, bool makeUpperCase);
    void Set(const char* tag, int value);
    void Set(const char* tag, bool value);
    void Set(const char* tag, float value);
    void Set(const char* tag, const char* value);
    void Set(const char* tag, const String& value);
    template <typename T>
    void Set(const char* tag, T value);
    template <typename T>
    T Get(const char* key, T defaultValue)
    {
        TagValuePair& tvp = FindTvp(key);
        if (tvp.tag == 0)
        {
            Set(key, defaultValue);
            return defaultValue;
        }
        return tvp.Get<T>();
    }
    void Parse(const char* data, int size, Parser& parser);
    bool IsBool(const char* string, bool& value) const;

    TagValuePair* mTvpHash;
    char* mStringMemory;
    char* mStringEnd;
    int mStringCapacity;
    unsigned int mTvpCapacity;
    bool mLoaded;
};

typedef Config::TagValuePair TagValuePair;

template <>
inline Config::String Config::Get<Config::String>(const char* key, Config::String defaultValue)
{
    TagValuePair& tvp = FindTvp(key);
    if (tvp.tag == 0)
    {
        Set(key, defaultValue);
        Config::String::Data* data = defaultValue.mData;
        if (data != 0)
        {
            ++data->mRefCount;
        }
        else
        {
            data = 0;
        }
        return Config::String(data);
    }
    return tvp.Get<Config::String>();
}

inline float GetConfigFloat(Config& cfg, const char* key, float defaultValue)
{
    TagValuePair& tvp = cfg.FindTvp(key);
    if (tvp.tag == 0)
    {
        cfg.Set(key, defaultValue);
        return defaultValue;
    }

    float val;
    if (tvp.type == CONFIG_BOOL)
    {
        val = LexicalCast<float, bool>(tvp.value.boolValue);
    }
    else if (tvp.type == CONFIG_INT)
    {
        val = LexicalCast<float, int>(tvp.value.intValue);
    }
    else if (tvp.type == CONFIG_FLOAT)
    {
        val = LexicalCast<float, float>(tvp.value.floatValue);
    }
    else if (tvp.type == CONFIG_STRING)
    {
        val = LexicalCast<float, const char*>(tvp.value.stringValue);
    }
    else
    {
        val = 0.0f;
    }
    return val;
}

inline int GetConfigInt(Config& cfg, const char* key, int defaultValue)
{
    TagValuePair& tvp = cfg.FindTvp(key);
    if (tvp.tag == 0)
    {
        cfg.Set(key, defaultValue);
        return defaultValue;
    }

    int val;
    if (tvp.type == CONFIG_BOOL)
    {
        val = LexicalCast<int, bool>(tvp.value.boolValue);
    }
    else if (tvp.type == CONFIG_INT)
    {
        val = LexicalCast<int, int>(tvp.value.intValue);
    }
    else if (tvp.type == CONFIG_FLOAT)
    {
        val = LexicalCast<int, float>(tvp.value.floatValue);
    }
    else if (tvp.type == CONFIG_STRING)
    {
        val = LexicalCast<int, const char*>(tvp.value.stringValue);
    }
    else
    {
        val = 0;
    }
    return val;
}

inline bool GetConfigBool(Config& cfg, const char* key, bool defaultValue)
{
    TagValuePair& tvp = cfg.FindTvp(key);
    bool val;
    if (tvp.tag == 0)
    {
        cfg.Set(key, defaultValue);
        return defaultValue;
    }

    if (tvp.type == CONFIG_BOOL)
    {
        val = tvp.value.boolValue;
    }
    else if (tvp.type == CONFIG_INT)
    {
        val = tvp.value.intValue;
    }
    else if (tvp.type == CONFIG_FLOAT)
    {
        val = tvp.value.floatValue;
    }
    else if (tvp.type == CONFIG_STRING)
    {
        val = Detail::LexicalCastImpl<bool, const char*>::Do(
            tvp.value.stringValue);
    }
    else
    {
        val = 0.0f;
    }
    return val;
}

#endif // NL_CONFIG_H
