#ifndef IDALLOCATOR_H
#define IDALLOCATOR_H

#include <map>

namespace pou
{

template<class T> class IdAllocator
{
    public:
        IdAllocator();
        virtual ~IdAllocator();

        void setMax(int m);
        void enableIdReuse(bool = true);

        size_t  allocateId(T t);
        bool    insert(size_t id, T t);
        bool    freeId(size_t id);
        bool    freeElement(T t);
        void    clear();

        T       findElement(size_t id);
        size_t  findId(T t);

        size_t size();

        typename std::map<size_t, T>::iterator begin();
        typename std::map<size_t, T>::iterator end();

    protected:

    private:
        size_t  m_curId;

        int     m_max;
        bool    m_enableIdReuse;

        std::map<size_t, T> m_mapIdToElement;
        std::map<T, size_t> m_mapElementToId;

        std::list<size_t>   m_availableIds;
};

}

#include "../src/tools/IdAllocator.inc"


#endif // IDALLOCATOR_H
