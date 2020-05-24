#include "PouEngine/utils/IdAllocator.h"

namespace pou
{

template <class T>
IdAllocator<T>::IdAllocator() :
    m_curId(0)
{
    //ctor
}

template <class T>
IdAllocator<T>::~IdAllocator()
{
    //dtor
}

template <class T>
size_t IdAllocator<T>::allocateId(T t)
{
    m_mapIdToElement.insert({++m_curId, t});
    m_mapElementToId.insert({t,m_curId});
    return m_curId;
}

template <class T>
bool IdAllocator<T>::insert(size_t id, T t)
{
    m_mapElementToId.insert({t,id});
    return m_mapIdToElement.insert({id,t}).second;
}

template <class T>
bool IdAllocator<T>::freeId(size_t id)
{
    auto it = m_mapIdToElement.find(id);
    if(it == m_mapIdToElement.end())
        return (false);

    //Need to free m_mapElementToId

    m_mapIdToElement.erase(it);
    return (true);
}

template <class T>
void IdAllocator<T>::clear()
{
    m_mapIdToElement.clear();
    m_mapElementToId.clear();
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
typename  std::map<size_t, T>::iterator IdAllocator<T>::begin()
{
    return m_mapIdToElement.begin();
}

template <class T>
typename  std::map<size_t, T>::iterator IdAllocator<T>::end()
{
    return m_mapIdToElement.end();
}


}

