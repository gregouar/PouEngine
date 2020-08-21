#ifndef HASHER_H
#define HASHER_H

#include "PouEngine/core/Singleton.h"
#include "PouEngine/Types.h"

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

        static HashedString hash(const std::string &);
        static HashedString unique_hash(const char* );
        static HashedString unique_hash(const std::string &);
        //const std::string& recoverFromUniqueHash(HashedString );

    protected:

    private:
        CRC::Table<std::uint32_t, 32> m_crc32Table;

        HashedString m_curHash;
        std::mutex m_hashedStringMutex;
        std::unordered_map<std::string, HashedString> m_hashedStrings;
};

}

#endif // HASHER_H




