#include "PouEngine/utils/WriteStream.h"

#include <glm/glm.hpp>
#include <iostream>

namespace pou
{

/// BitWriter


BitWriter::BitWriter(uint8_t *buffer, int bytes) : m_buffer(buffer), m_bytes(bytes),
    m_scratch(0), m_scratch_bits(0), m_word_index(0)
{

}

BitWriter::~BitWriter()
{

}


bool BitWriter::writeBits(uint32_t unsigned_value, int bits)
{
    if(m_word_index*4 >= m_bytes)
        return (false);

    unsigned_value = (unsigned_value << (32 - bits)) >> (32 - bits);

    m_scratch |= (uint64_t)(unsigned_value) << m_scratch_bits ;
    m_scratch_bits += bits;

    if(m_scratch_bits >= 32)
    {
        m_buffer[m_word_index*4] = (uint8_t)m_scratch;
        if(m_word_index*4+1 < m_bytes)
            m_buffer[m_word_index*4+1] = (uint8_t)(m_scratch>>8);
        if(m_word_index*4+2 < m_bytes)
            m_buffer[m_word_index*4+2] = (uint8_t)(m_scratch>>16);
        if(m_word_index*4+3 < m_bytes)
            m_buffer[m_word_index*4+3] = (uint8_t)(m_scratch>>24);
        m_word_index++;

        m_scratch_bits -= 32;
        m_scratch = m_scratch >> 32;

        if(m_word_index*4 >= m_bytes && m_scratch_bits > 0)
            return (false);
    }

    return (true);
}

void BitWriter::memcpy(uint8_t *data, int data_size, int bytes_shift)
{
    assert(m_word_index*4 + bytes_shift + data_size <= m_bytes);
    ///This secure buffer but not data !!! => maybe replace data by vector

    std::memcpy( m_buffer + m_word_index * 4 + bytes_shift, data, data_size);
}

void BitWriter::flush()
{
    m_scratch = m_scratch << (32 - m_scratch_bits);
    m_scratch = m_scratch >> (32 - m_scratch_bits);

    m_buffer[m_word_index*4] = (uint8_t)m_scratch;
    if(m_word_index*4+1 < m_bytes)
        m_buffer[m_word_index*4+1] = (uint8_t)(m_scratch>>8);
    if(m_word_index*4+2 < m_bytes)
        m_buffer[m_word_index*4+2] = (uint8_t)(m_scratch>>16);
    if(m_word_index*4+3 < m_bytes)
        m_buffer[m_word_index*4+3] = (uint8_t)(m_scratch>>24);
}

void BitWriter::printBitCode(uint8_t v)
{
    std::cout<<(bool)(v & (uint8_t)(128))
        <<(bool)(v & (uint8_t)(64))
        <<(bool)(v & (uint8_t)(32))
        <<(bool)(v & (uint8_t)(16))
        <<(bool)(v & (uint8_t)(8))
        <<(bool)(v & (uint8_t)(4))
        <<(bool)(v & (uint8_t)(2))
        <<(bool)(v & (uint8_t)(1));
}

void BitWriter::printBitCode(uint16_t v)
{
    printBitCode((uint8_t)(v>>8));
    printBitCode((uint8_t)v);
}

void BitWriter::printBitCode(uint32_t v)
{
    printBitCode((uint16_t)(v>>16));
    printBitCode((uint16_t)v);
}

void BitWriter::printBitCode(uint64_t v)
{
    printBitCode((uint32_t)(v>>32));
    printBitCode((uint32_t)v);
}


/// WriteStream

WriteStream::WriteStream()
{
}

WriteStream::~WriteStream()
{
    //dtor
}


bool WriteStream::isWriting()
{
    return m_writer ? 1 : 0;
}

bool WriteStream::isReading()
{
    return (false);
}


void WriteStream::setBuffer(uint8_t *buffer, int bytes)
{
    m_writer = std::make_unique<BitWriter> (buffer, bytes);
}


void WriteStream::flush()
{
    if(m_writer)
        m_writer.get()->flush();
}


void WriteStream::memcpy(uint8_t *data, int data_size)
{
    Stream::padZeroes();
    this->flush();
    m_bits += data_size*8;
    if(m_writer)
        m_writer.get()->memcpy(data, data_size, (m_bits%32)/8);
}

void WriteStream::serializeBits(int32_t &value, int bits)
{
    m_bits += bits;

    if(m_writer)
        m_writer.get()->writeBits(value, bits);
}

void WriteStream::serializeInteger(int32_t &value, int32_t min, int32_t max)
{
    assert(min < max);
    assert(value >= min);
    assert(value <= max);

    const int bits = bitsRequired(min, max);

    m_bits += bits;

    uint32_t unsigned_value = value - min;

    if(m_writer)
        m_writer.get()->writeBits(unsigned_value, bits);
}

void WriteStream::serializeBool(bool &value)
{
    int32_t temp = (int32_t)value;
    this->serializeBits(temp, 1);
}

void WriteStream::serializeChar(char &value)
{
    int32_t temp = (int32_t)value;
    this->serializeBits(temp, 8);
}



}
