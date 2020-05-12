#ifndef HASHER_H
#define HASHER_H

#include "PouEngine/utils/Singleton.h"

#include "CRCpp/CRC.h"

namespace pou
{

class Hasher : public Singleton<Hasher>
{
    friend class Singleton<Hasher>;

    public:
        Hasher();
        virtual ~Hasher();

        static uint32_t crc32(const std::string &string, uint32_t crc = 0);
        static uint32_t crc32(const void* data, size_t data_size, uint32_t crc = 0);

    protected:

    private:
        CRC::Table<std::uint32_t, 32> m_crc32Table;
};

}

#endif // HASHER_H
