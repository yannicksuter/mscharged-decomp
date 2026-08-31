#ifndef NL_NL_LIST_CONTAINER_H
#define NL_NL_LIST_CONTAINER_H

template <typename T, typename Adapter>
class ListContainerBase
{
public:
    typedef void (ListContainerBase::*EntryCallback)(ListEntry<T>*);

    ListContainerBase()
        : m_Head(0)
        , m_Tail(0)
    {
    }

    ~ListContainerBase()
    {
        Clear();
    }

    void Clear()
    {
        EntryCallback callback = &ListContainerBase::DeleteEntry;
        nlWalkList(m_Head, this, callback);
        m_Head = 0;
        m_Tail = 0;
    }

    void DeleteEntry(ListEntry<T>* entry);

    void RemoveEntry(const T& data)
    {
        ListEntry<T>* currentEntry = m_Head;
        if (currentEntry != 0)
        {
            if (currentEntry->entry == data)
            {
                ListEntry<T>* newHead;
                if (currentEntry == m_Tail)
                {
                    m_Tail = 0;
                    newHead = 0;
                }
                else
                    newHead = currentEntry->next;
                DeleteEntry(m_Head);
                m_Head = newHead;
            }
            else
            {
                ListEntry<T>* previousEntry = currentEntry;
                nlListIterator<T> iterator(currentEntry->next);
                while (iterator.IsValid())
                {
                    ListEntry<T>* nextEntry = iterator.CurrentEntry();
                    if (iterator.Current() == data)
                    {
                        previousEntry->next = nextEntry->next;
                        if (nextEntry == m_Tail)
                            m_Tail = previousEntry;
                        DeleteEntry(nextEntry);
                        break;
                    }
                    previousEntry = nextEntry;
                    iterator.Next();
                }
            }
        }
    }

    void AddEntry(const T& value)
    {
        ListEntry<T> local(value);
        ListEntry<T>* entry = m_Allocator.New(local);
        nlListAddStart(&m_Head, entry, &m_Tail);
    }

    void AddStart(const T& data)
    {
        ListEntry<T> value(data);
        ListEntry<T>* entry = m_Allocator.Allocate();
        if (entry != 0)
        {
            *entry = value;
        }
        nlListAddStart(&m_Head, entry, &m_Tail);
    }

    void AddEnd(const T& data)
    {
        ListEntry<T>* entry = m_Allocator.New(ListEntry<T>(data));
        nlListAddEnd(&m_Head, &m_Tail, entry);
    }

    ListEntry<T>* RemoveStart()
    {
        return nlListRemoveStart(&m_Head, &m_Tail);
    }

    void RemoveStart(T* outData)
    {
        ListEntry<T>* entry = nlListRemoveStart(&m_Head, &m_Tail);
        Deallocate(entry, outData);
    }

    void Deallocate(ListEntry<T>* entry, T* outData)
    {
        if (outData != 0)
        {
            *outData = entry->entry;
        }
        m_Allocator.DeleteEntry(entry);
    }

    nlListIterator<T> Begin();

    /* 0x00 */ Adapter m_Allocator;
    /* 0x04 */ ListEntry<T>* m_Head;
    /* 0x08 */ ListEntry<T>* m_Tail;
};

template <typename T>
class nlListIterator
{
public:
    nlListIterator(ListEntry<T>* current)
        : m_Curr(current)
    {
    }

    bool IsValid() const
    {
        return m_Curr != 0;
    }

    T& Current() const
    {
        return m_Curr->entry;
    }

    void Next()
    {
        m_Curr = m_Curr->next;
    }

    ListEntry<T>* CurrentEntry() const
    {
        return m_Curr;
    }

private:
    ListEntry<T>* m_Curr;
};

template <typename T, typename Adapter>
inline nlListIterator<T> ListContainerBase<T, Adapter>::Begin()
{
    return nlListIterator<T>(m_Head);
}

template <typename T, typename Adapter>
inline void ListContainerBase<T, Adapter>::DeleteEntry(ListEntry<T>* entry)
{
    m_Allocator.DeleteEntry(entry);
}

template <typename T>
class nlListContainer
    : public ListContainerBase<T, NewAdapter<ListEntry<T> > >
{
};

#endif // NL_NL_LIST_CONTAINER_H
