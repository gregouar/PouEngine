#include "PouEngine/utils/ReadStream.h"

#include <glm/glm.hpp>

#include "PouEngine/utils/Logger.h"

namespace pou
{

/// BitReader

BitReader::BitReader(const uint8_t *buffer, int bytes) : m_buffer(buffer), m_bytes(bytes),
    m_scratch(0), m_scratch_bits(0), /*m_total_bits(bytes*8), m_num_bits_read(0),*/ m_byte_index(0)
{
}

BitReader::~BitReader()
{

}

bool BitReader::wouldReadPastEnd(int bits)
{
    if(m_scratch_bits - bits < 0 && m_byte_index+1 >= m_bytes)
        return (true);
    return (false);
}

uint32_t BitReader::readBits(int bits)
{
    if(m_scratch_bits - bits < 0)
    {
        assert(m_byte_index < m_bytes);

        //m_scratch <<= 32;

        m_scratch |= (uint64_t)(m_buffer[m_byte_index]) << m_scratch_bits ;
        if(m_byte_index+1 < m_bytes)
            m_scratch |= (uint64_t)(m_buffer[m_byte_index+1]) << (m_scratch_bits+8);
        if(m_byte_index+2 < m_bytes)
            m_scratch |= (uint64_t)(m_buffer[m_byte_index+2]) << (m_scratch_bits+16);
        if(m_byte_index+3 < m_bytes)
            m_scratch |= (uint64_t)(m_buffer[m_byte_index+3]) << (m_scratch_bits+24);
        m_byte_index += 4;
        m_scratch_bits += 32;
    }

    uint32_t value = (uint32_t)((m_scratch << (64-bits)) >> (64-bits));
    m_scratch >>= bits;
    m_scratch_bits -= bits;

    return value;
}

void BitReader::memcpy(uint8_t *data, int data_size, int bytes_shift)
{
    m_byte_index = m_byte_index - 4 * (bytes_shift == 0 ? 0 : 1) + bytes_shift ;
    //std::cout<<"Read Index:"<<index * 4 + bytes_shift<<std::endl;
    //std::cout<<"Read:"<<index * 4 + bytes_shift + data_size<<" VS "<<m_bytes<<"(with bytes shift="<<bytes_shift<<std::endl;

    assert(m_byte_index + data_size <= m_bytes);
    ///This secure buffer but not data !!! => maybe replace data by vector
    std::memcpy(data, m_buffer + m_byte_index, data_size);

    m_byte_index += data_size;
    m_scratch = 0;
    m_scratch_bits = 0;
}

/// ReadStream

ReadStream::ReadStream()
{
    //ctor
}

ReadStream::~ReadStream()
{
    //dtor
}

bool ReadStream::isWriting()
{
    return false;
}

bool ReadStream::isReading()
{
    return m_reader ? 1 : 0;
}

void ReadStream::setBuffer(std::vector<uint8_t> &buffer)
{
    this->setBuffer(buffer.data(), buffer.size());
}

void ReadStream::setBuffer(uint8_t *buffer, int bytes)
{
    m_reader = std::make_unique<BitReader> (buffer, bytes);
}

void ReadStream::memcpy(uint8_t *data, int data_size)
{
    Stream::padZeroes();

    if(m_reader)
        m_reader->memcpy(data, data_size, (m_bits%32)/8);

    m_bits += data_size*8;
}

void ReadStream::serializeBits(int32_t &value, int bits)
{
    m_bits += bits;

    if(!m_reader)
        return;

    if(m_reader->wouldReadPastEnd(bits))
        return;

    value = m_reader->readBits(bits);
}

void ReadStream::serializeInt(int32_t &value, int32_t min, int32_t max)
{
    assert(min < max);
    const int bits = bitsRequired(min, max);
    int32_t unsigned_value = 0;
    this->serializeBits(unsigned_value,bits);
    value = (int32_t)unsigned_value + min;
}

void ReadStream::serializeFloat(float &value, float min, float max, uint8_t decimals)
{
    assert(min < max);

    decimals = pow(10,decimals);

    int32_t minInt = min*decimals;
    int32_t maxInt = max*decimals;
    const int bits = bitsRequired(minInt, maxInt);

    int32_t unsigned_value = 0;
    this->serializeBits(unsigned_value,bits);

    value = ((float)unsigned_value)/decimals + min;
}

void ReadStream::serializeBool(bool &value)
{
    int32_t v = 0;
    this->serializeBits(v,1);
    value = (bool)v;
}

void ReadStream::serializeChar(char &value)
{
    int32_t v = 0;
    this->serializeBits(v,8);
    value = (char)v;
}

void ReadStream::serializeString(std::string &str)
{
    int strSize = 0;
    this->serializeBits(strSize, 8);

    std::vector<uint8_t> temp(strSize+1);
    this->memcpy(temp.data(), strSize);
    temp.back() = '\0';

    str = std::string(reinterpret_cast<const char *>(temp.data()));
}



}
