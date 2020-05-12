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

void BitReader::memcpy(uint8_t *data, int data_size)
{
    assert(m_word_index + data_size < m_bytes);
    ///This secure buffer but not data !!! => maybe replace data by vector
    std::memcpy(data, m_buffer + m_word_index, data_size);
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


void ReadStream::setBuffer(uint8_t *buffer, int bytes)
{
    m_reader = std::make_unique<BitReader> (buffer, bytes);
}

void ReadStream::memcpy(uint8_t *data, int data_size)
{
    Stream::padZeroes();

    m_bits += data_size*8;

    if(m_reader)
        m_reader.get()->memcpy(data, data_size);
}

void ReadStream::serializeBits(int32_t &value, int bits)
{
    m_bits += bits;

    if(!m_reader)
        return;

    if(m_reader.get()->wouldReadPastEnd(bits))
        return;

    value = m_reader.get()->readBits(bits);
}

void ReadStream::serializeInteger(int32_t &value, int32_t min, int32_t max)
{
    assert(min < max);
    const int bits = bitsRequired(min, max);

    /*m_bits += bits;

    if(!m_reader)
        return;

    if(m_reader.get()->wouldReadPastEnd(bits))
        return;*/

    int32_t unsigned_value = 0;
    this->serializeBits(unsigned_value,bits);
    value = (int32_t)unsigned_value + min;
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



}
