#ifndef NL_NL_LIST_H
#define NL_NL_LIST_H

template <typename T>
class ListEntry
{
public:
    /* 0x00 */ ListEntry<T>* next;
    /* 0x04 */ T entry;
};

template <typename T>
class nlListContainer
{
public:
    /* 0x00 */ unsigned char m_Allocator[4];
    /* 0x04 */ ListEntry<T>* m_Head;
    /* 0x08 */ ListEntry<T>* m_Tail;
};

#endif
