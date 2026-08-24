#ifndef NL_SINGLETON_H
#define NL_SINGLETON_H

template <class T>
class nlSingleton
{
protected:
    ~nlSingleton() { }

public:
    static T* Instance() { return s_pInstance; }
    static T* GetInstance() { return s_pInstance; }

    static T* s_pInstance;
};

#endif // NL_SINGLETON_H
