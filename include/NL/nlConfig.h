#ifndef NL_CONFIG_H
#define NL_CONFIG_H

#include "NL/nlBasicString.h"
#include "NL/nlFunction.h"
#include "types.h"

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
    void Parse(const char* data, int size, Parser& parser);
    bool IsBool(const char* string, bool& value) const;

    TagValuePair* mTvpHash;
    char* mStringMemory;
    char* mStringEnd;
    int mStringCapacity;
    unsigned int mTvpCapacity;
    bool mLoaded;
};

#endif // NL_CONFIG_H
