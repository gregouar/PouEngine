#ifndef IDPTRALLOCATOR_H
#define IDPTRALLOCATOR_H

#include <map>
#include <memory>

namespace pou
{

template<class T> class IdPtrAllocator
{
    public:
        IdPtrAllocator();
        virtual ~IdPtrAllocator();

        void setMax(int m);
        void enableIdReuse(bool = true);

        size_t  allocateId(std::shared_ptr<T> t);
        bool    insert(size_t id, std::shared_ptr<T>  t);
        bool    freeId(size_t id);
        bool    freeElement(std::shared_ptr<T> t);
        bool    freeElement(T* t);
        void    clear();

        std::shared_ptr<T> findElement(size_t id);
        size_t  findId(std::shared_ptr<T> t);
        size_t  findId(const T* t);

        size_t size();

        typename std::unordered_map<size_t, std::shared_ptr<T> >::iterator begin();
        typename std::unordered_map<size_t, std::shared_ptr<T> >::iterator end();

    protected:

    private:
        size_t  m_curId;

        int     m_max;
        bool    m_enableIdReuse;

        std::unordered_map<size_t, std::shared_ptr<T> > m_mapIdToElement;
        std::unordered_map<const T*, size_t> m_mapElementToId;

        std::list<size_t>   m_availableIds;
};

}

#include "../src/tools/IdPtrAllocator.tpp"

#endif // IDPTRALLOCATOR_H
