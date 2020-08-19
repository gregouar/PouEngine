#include "PouEngine/tools/IdAllocator.h"

#include "PouEngine/tools/Logger.h"

namespace pou
{

template <class T>
IdAllocator<T>::IdAllocator() :
    m_curId(0), m_max(-1), m_enableIdReuse(true)
{
    //ctor
}

template <class T>
IdAllocator<T>::~IdAllocator()
{
    //dtor
}


template <class T>
void IdAllocator<T>::setMax(int m)
{
    m_max = m;
    if(m_max < 0)
        m_max = -1;
}

template <class T>
void IdAllocator<T>::enableIdReuse(bool b)
{
    m_enableIdReuse = b;
}


template <class T>
size_t IdAllocator<T>::allocateId(T t)
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
    m_mapElementToId.insert({t,id});
    return id;
}

template <class T>
bool IdAllocator<T>::insert(size_t id, T t)
{
    if(m_max != -1 && id > (size_t)m_max)
    {
        Logger::warning("Tried to allocate ID bigger than max allowed");
        return 0;
    }

    m_mapElementToId.insert({t,id});
    return m_mapIdToElement.insert({id,t}).second;
}

template <class T>
bool IdAllocator<T>::freeId(size_t id)
{
    auto it = m_mapIdToElement.find(id);
    if(it == m_mapIdToElement.end())
        return (false);

    m_availableIds.push_back(id);

    auto it2 = m_mapElementToId.find(it->second);
    if(it2 != m_mapElementToId.end())
        m_mapElementToId.erase(it2);

    m_mapIdToElement.erase(it);
    return (true);
}

template <class T>
bool IdAllocator<T>::freeElement(T t)
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
void IdAllocator<T>::clear()
{
    m_mapIdToElement.clear();
    m_mapElementToId.clear();
    m_availableIds.clear();
}

template <class T>
T IdAllocator<T>::findElement(size_t id)
{
    auto it = m_mapIdToElement.find(id);
    if(it == m_mapIdToElement.end())
        return (nullptr);

    return it->second;
}

template <class T>
size_t  IdAllocator<T>::findId(const T t)
{
    auto it = m_mapElementToId.find(t);
    if(it == m_mapElementToId.end())
        return 0;

    return it->second;

    /*for(auto it : m_mapIdToElement)
        if(it.second == t)
            return it.first;
    return 0;*/
}


template <class T>
size_t IdAllocator<T>::size()
{
    return m_mapIdToElement.size();
}


template <class T>
typename  std::unordered_map<size_t, T>::iterator IdAllocator<T>::begin()
{
    return m_mapIdToElement.begin();
}

template <class T>
typename  std::unordered_map<size_t, T>::iterator IdAllocator<T>::end()
{
    return m_mapIdToElement.end();
}


}

