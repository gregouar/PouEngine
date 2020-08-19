#include "PouEngine/tools/IdPtrAllocator.h"

#include "PouEngine/tools/Logger.h"


namespace pou
{

template <class T>
IdPtrAllocator<T>::IdPtrAllocator() :
    m_curId(0), m_max(-1), m_enableIdReuse(true)
{
    //ctor
}

template <class T>
IdPtrAllocator<T>::~IdPtrAllocator()
{
    //dtor
}


template <class T>
void IdPtrAllocator<T>::setMax(int m)
{
    m_max = m;
    if(m_max < 0)
        m_max = -1;
}

template <class T>
void IdPtrAllocator<T>::enableIdReuse(bool b)
{
    m_enableIdReuse = b;
}


template <class T>
size_t IdPtrAllocator<T>::allocateId(std::shared_ptr<T> t)
{
    if(m_max != -1 && m_curId > (size_t)m_max)
    {
        Logger::warning("Tried to allocate ID bigger than max allowed");
        return 0;
    }

    int id = 0;

    if(m_enableIdReuse && !m_availableIds.empty())
    {
        id = m_availableIds.back();
        m_availableIds.pop_back();
    } else
        id = ++m_curId;

    m_mapIdToElement.insert({id, t});
    m_mapElementToId.insert({t.get(),id});
    return id;
}

template <class T>
bool IdPtrAllocator<T>::insert(size_t id, std::shared_ptr<T> t)
{
    if(m_max != -1 && id > (size_t)m_max)
    {
        Logger::warning("Tried to allocate ID "+std::to_string(id)+" bigger than max allowed: "+std::to_string(m_max));
        return 0;
    }

    m_mapElementToId.insert({t.get(),id});
    return m_mapIdToElement.insert({id,t}).second;
}

template <class T>
bool IdPtrAllocator<T>::freeId(size_t id)
{
    auto it = m_mapIdToElement.find(id);
    if(it == m_mapIdToElement.end())
        return (false);

    m_availableIds.push_back(id);

    auto it2 = m_mapElementToId.find(it->second.get());
    if(it2 != m_mapElementToId.end())
        m_mapElementToId.erase(it2);

    m_mapIdToElement.erase(it);
    return (true);
}

template <class T>
bool IdPtrAllocator<T>::freeElement(std::shared_ptr<T> t)
{
    return this->freeElement(t.get());
}

template <class T>
bool IdPtrAllocator<T>::freeElement(T *t)
{
    auto it = m_mapElementToId.find(t);
    if(it == m_mapElementToId.end())
        return (false);

    m_availableIds.push_back(it->second);

    auto it2 = m_mapIdToElement.find(it->second);
    if(it2 != m_mapIdToElement.end())
        m_mapIdToElement.erase(it2);

    m_mapElementToId.erase(it);
    return (true);
}

template <class T>
void IdPtrAllocator<T>::clear()
{
    m_mapIdToElement.clear();
    m_mapElementToId.clear();
    m_availableIds.clear();
}

template <class T>
std::shared_ptr<T> IdPtrAllocator<T>::findElement(size_t id)
{
    auto it = m_mapIdToElement.find(id);
    if(it == m_mapIdToElement.end())
        return (nullptr);

    return it->second;
}


template <class T>
size_t  IdPtrAllocator<T>::findId(std::shared_ptr<T> t)
{
    return this->findId(t.get());
}

template <class T>
size_t  IdPtrAllocator<T>::findId(const T *t)
{
    auto it = m_mapElementToId.find(t);
    if(it == m_mapElementToId.end())
        return 0;

    return it->second;
}


template <class T>
size_t IdPtrAllocator<T>::size()
{
    return m_mapIdToElement.size();
}


template <class T>
typename  std::unordered_map<size_t, std::shared_ptr<T> >::iterator IdPtrAllocator<T>::begin()
{
    return m_mapIdToElement.begin();
}

template <class T>
typename  std::unordered_map<size_t, std::shared_ptr<T> >::iterator IdPtrAllocator<T>::end()
{
    return m_mapIdToElement.end();
}



}

