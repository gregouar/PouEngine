#include "PouEngine/utils/Hasher.h"

namespace pou
{

Hasher::Hasher() : m_crc32Table(CRC::CRC_32())
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

}

