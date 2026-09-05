#ifndef NL_GL_TU_80364020_H
#define NL_GL_TU_80364020_H

class Class_80364020
{
public:
    Class_80364020()
        : m_Unknown0C(0)
        , m_Unknown10(0)
        , m_Unknown14(0)
    {
    }

    virtual ~Class_80364020();

    void fn_803640DC();
    void fn_803640E0(const char*);
    void fn_803640E4(const char*);
    const char* fn_803640E8(unsigned short);
    const char* fn_803640F0(unsigned short);

    unsigned char m_Unknown04[8];
    unsigned int m_Unknown0C;
    unsigned int m_Unknown10;
    unsigned int m_Unknown14;
};

extern "C" Class_80364020* fn_80364020();

#endif // NL_GL_TU_80364020_H
