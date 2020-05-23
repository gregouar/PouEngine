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

        size_t  allocateId(T *t);
        bool    freeId(size_t id);
        T*      findElement(size_t id);

    protected:

    private:
        size_t              m_curId;
        std::map<size_t, T*> m_map;
};

}

#include "../src/utils/IdAllocator.inc"


#endif // IDALLOCATOR_H
