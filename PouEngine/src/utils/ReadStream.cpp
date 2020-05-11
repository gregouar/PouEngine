#include "PouEngine/utils/ReadStream.h"

#include <glm/glm.hpp>

namespace pou
{

/// BitReader

BitReader::BitReader(const uint8_t *buffer, int bytes) : m_buffer(buffer), m_bytes(bytes),
    m_scratch(0), m_scratch_bits(0), /*m_total_bits(bytes*8), m_num_bits_read(0),*/ m_word_index(0)
{
}

BitReader::~BitReader()
{

}

bool BitReader::wouldReadPastEnd(int bits)
{
    return (false);
}

uint32_t BitReader::readBits(int bits)
{
    if(m_scratch_bits - bits < 0)
    {
        assert(m_word_index < m_bytes);

        m_scratch |= (uint64_t)(m_buffer[m_word_index*4]) << m_scratch_bits;
        m_scratch |= (uint64_t)(m_buffer[m_word_index*4+1]) << (m_scratch_bits+8);
        m_scratch |= (uint64_t)(m_buffer[m_word_index*4+2]) << (m_scratch_bits+16);
        m_scratch |= (uint64_t)(m_buffer[m_word_index*4+3]) << (m_scratch_bits+24);
        m_word_index++;
        m_scratch_bits += 32;
    }

    uint32_t value = (uint32_t)((m_scratch << (64-bits)) >> (64-bits));
    m_scratch = m_scratch >> bits;
    m_scratch_bits -= bits;

    return value;
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

void ReadStream::setBuffer(const uint8_t *buffer, int bytes)
{
    m_reader = std::make_unique<BitReader> (buffer, bytes);
}

int ReadStream::computeBytes(int bits)
{
   // if(m_reader)
     //   m_reader.get()->flush();
    return (int)((bits/32) + ((bits % 32) ? 1 : 0))*4;
}

int ReadStream::bitsRequired(int32_t min, int32_t max)
{
    int bits = 1;
    uint32_t cur = max-min;

    while(cur/2 > 0)
    {
        cur = cur/2;
        bits++;
    }

    return bits;
}

int ReadStream::serializeBits(int32_t &value, int bits)
{
    if(!m_reader)
        return bits;

    if(m_reader.get()->wouldReadPastEnd(bits))
        return (0);

    value = m_reader.get()->readBits(bits);

    return (1);
}

int ReadStream::serializeInteger(int32_t &value, int32_t min, int32_t max)
{
    assert(min < max);
    const int bits = bitsRequired(min, max);

    if(!m_reader)
        return bits;

    if(m_reader.get()->wouldReadPastEnd(bits))
        return (0);

    uint32_t unsigned_value = m_reader.get()->readBits(bits);
    value = (int32_t)unsigned_value + min;
    return (1);
}

int ReadStream::serializeBool(bool &value)
{
    int32_t v = 0;
    int bits = this->serializeBits(v,1);
    value = (bool)v;
    return bits;
}

int ReadStream::serializeChar(char &value)
{
    int32_t v = 0;
    int bits = this->serializeBits(v,8);
    value = (char)v;
    return bits;
}



}
