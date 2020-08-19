#include "PouEngine/tools/Hasher.h"

namespace pou
{

Hasher::Hasher() :
    m_crc32Table(CRC::CRC_32()),
    m_curHash(0)
{
    //ctor
}

Hasher::~Hasher()
{
    //dtor
}


uint32_t Hasher::crc32(const std::string &string, uint32_t crc)
{
    return crc32(string.c_str(), string.size(), crc);
}

uint32_t Hasher::crc32(const void* data, size_t data_size, uint32_t crc)
{
    return CRC::Calculate(data, data_size, instance()->m_crc32Table, crc);
}


HashedString Hasher::hash(const std::string &s)
{
    return std::hash<std::string>{}(s);
}


HashedString Hasher::unique_hash(const std::string &s)
{
    if(s.empty())
        return 0;

    std::lock_guard<std::mutex> guard(instance()->m_hashedStringMutex);

    auto it = instance()->m_hashedStrings.find(s);
    if(it == instance()->m_hashedStrings.end())
    {
        instance()->m_hashedStrings[s] = ++instance()->m_curHash;
        return instance()->m_curHash;
    }
    else
        return it->second;
}


}




