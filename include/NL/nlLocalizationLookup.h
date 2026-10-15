#ifndef NL_LOCALIZATION_LOOKUP_H
#define NL_LOCALIZATION_LOOKUP_H

#include "NL/nlAlgorithm.h"
#include "NL/nlLocalization.h"
#include "NL/nlString.h"
#if defined(VERSION_R4QK01)
#include "NL/nlFormat.h"
#include "NL/nlKoreanParticle.h"
#endif

inline const unsigned short* nlLocalization::GetString(const char* string) const
{
    unsigned long hash = nlStringLowerHash(string);
    if (m_LookupTable == 0)
        return LocalizationTableNotFound;
    StringLookup* found = nlBSearch<StringLookup, unsigned long>(hash, m_LookupTable, m_pFile->StringCount);
    if (found != 0)
        return m_FirstString + found->StringOffset;
    return MissingLocString;
}

inline const unsigned short* LookupLocString(const char* id)
{
    return g_pLocalization->GetString(id);
}

#if defined(VERSION_R4QK01)
template <typename StringType>
class KoreanFormatImpl : public FormatImpl<StringType>
{
    int mCurrentParticle;
    const char* mKey;

public:
    KoreanFormatImpl(const StringType& string, const char* key)
        : FormatImpl<StringType>(string)
        , mCurrentParticle(0)
        , mKey(key)
    {
    }

    operator StringType() const
    {
        return this->mString;
    }

    using FormatImpl<StringType>::operator%;

    KoreanFormatImpl& operator%(const KoreanParticle& particle)
    {
        ++mCurrentParticle;
        char key[128];
        nlSNPrintf(key, sizeof(key), "%s_PART_%d_%s", mKey, mCurrentParticle,
            particle == KOREAN_PARTICLE_WITHOUT ? "WITHOUT" : "WITH");
        const unsigned short* particleString = g_pLocalization->GetString(key);
        StringType insert = LexicalCast<StringType>(particleString);

        // Particle placeholders are numbered from one: {p1}, {p2}, ...
        for (int i = 0; i < this->mString.size(); i++)
        {
            if (this->mString[i] == (typename StringType::value_type)'{'
                && i + 1 < this->mString.size()
                && this->mString[i + 1] == 'p'
                && this->mString[i + 2] - '0' == mCurrentParticle
                && i + 4 < this->mString.size()
                && this->mString[i + 3] == (typename StringType::value_type)'}')
            {
                this->mString.erase(this->mString.begin() + i, this->mString.begin() + i + 4);
                this->mString.insert(this->mString.begin() + i, insert.begin(), insert.end());
            }
        }
        return *this;
    }
};

template <typename T0, typename T1, typename T2, typename T3>
BasicString<unsigned short, Detail::TempStringAllocator> FormatKorean(
    const char* key, const T0& t0, const T1& t1, const T2& t2, const T3& t3)
{
    typedef BasicString<unsigned short, Detail::TempStringAllocator> StringType;
    typedef KoreanFormatImpl<StringType> Formatter;
    // Positional substitutions return the base type; keep the Korean formatter
    // available for particle arguments and for the final string conversion.
    return static_cast<Formatter&>(
        static_cast<Formatter&>(
            static_cast<Formatter&>(
                static_cast<Formatter&>(Formatter(StringType(g_pLocalization->GetString(key)), key) % t0) % t1)
            % t2)
        % t3);
}

template <typename T0, typename T1, typename T2, typename T3,
    typename T4, typename T5, typename T6, typename T7>
BasicString<unsigned short, Detail::TempStringAllocator> FormatKorean(
    const char* key, const T0& t0, const T1& t1, const T2& t2, const T3& t3,
    const T4& t4, const T5& t5, const T6& t6, const T7& t7)
{
    typedef BasicString<unsigned short, Detail::TempStringAllocator> StringType;
    typedef KoreanFormatImpl<StringType> Formatter;
    return static_cast<Formatter&>(
        static_cast<Formatter&>(
            static_cast<Formatter&>(
                static_cast<Formatter&>(
                    static_cast<Formatter&>(
                        static_cast<Formatter&>(
                            static_cast<Formatter&>(
                                static_cast<Formatter&>(
                                    Formatter(StringType(g_pLocalization->GetString(key)), key) % t0)
                                % t1)
                            % t2)
                        % t3)
                    % t4)
                % t5)
            % t6)
        % t7);
}

template <typename T0, typename T1, typename T2, typename T3,
    typename T4, typename T5>
BasicString<unsigned short, Detail::TempStringAllocator> FormatKorean(
    const char* key, const T0& t0, const T1& t1, const T2& t2, const T3& t3,
    const T4& t4, const T5& t5)
{
    typedef BasicString<unsigned short, Detail::TempStringAllocator> StringType;
    typedef KoreanFormatImpl<StringType> Formatter;
    return static_cast<Formatter&>(
        static_cast<Formatter&>(
            static_cast<Formatter&>(
                static_cast<Formatter&>(
                    static_cast<Formatter&>(
                        static_cast<Formatter&>(
                            Formatter(StringType(g_pLocalization->GetString(key)), key) % t0)
                        % t1)
                    % t2)
                % t3)
            % t4)
        % t5);
}

template <typename T0, typename T1, typename T2, typename T3,
    typename T4, typename T5, typename T6>
BasicString<unsigned short, Detail::TempStringAllocator> FormatKorean(
    const char* key, const T0& t0, const T1& t1, const T2& t2, const T3& t3,
    const T4& t4, const T5& t5, const T6& t6)
{
    typedef BasicString<unsigned short, Detail::TempStringAllocator> StringType;
    typedef KoreanFormatImpl<StringType> Formatter;
    return static_cast<Formatter&>(
        static_cast<Formatter&>(
            static_cast<Formatter&>(
                static_cast<Formatter&>(
                    static_cast<Formatter&>(
                        static_cast<Formatter&>(
                            static_cast<Formatter&>(
                                Formatter(StringType(g_pLocalization->GetString(key)), key) % t0)
                            % t1)
                        % t2)
                    % t3)
                % t4)
            % t5)
        % t6);
}

template <typename T0, typename T1>
BasicString<unsigned short, Detail::TempStringAllocator> FormatKorean(
    const char* key, const T0& t0, const T1& t1)
{
    typedef BasicString<unsigned short, Detail::TempStringAllocator> StringType;
    typedef KoreanFormatImpl<StringType> Formatter;
    return static_cast<Formatter&>(
        static_cast<Formatter&>(Formatter(StringType(g_pLocalization->GetString(key)), key) % t0) % t1);
}

template <typename T0, typename T1, typename T2>
BasicString<unsigned short, Detail::TempStringAllocator> FormatKorean(
    const char* key, const T0& t0, const T1& t1, const T2& t2)
{
    typedef BasicString<unsigned short, Detail::TempStringAllocator> StringType;
    typedef KoreanFormatImpl<StringType> Formatter;
    return static_cast<Formatter&>(
        static_cast<Formatter&>(
            static_cast<Formatter&>(Formatter(StringType(g_pLocalization->GetString(key)), key) % t0) % t1)
        % t2);
}

#endif

#endif // NL_LOCALIZATION_LOOKUP_H
