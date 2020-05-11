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
        m_buffer[m_word_index*4+1] = (uint8_t)(m_scratch>>8);
        m_buffer[m_word_index*4+2] = (uint8_t)(m_scratch>>16);
        m_buffer[m_word_index*4+3] = (uint8_t)(m_scratch>>24);
        m_word_index++;

        m_scratch_bits -= 32;
        m_scratch = m_scratch >> 32;

        if(m_word_index*4 >= m_bytes && m_scratch_bits > 0)
            return (false);
    }

    return (true);
}

void BitWriter::flush()
{
    m_scratch = m_scratch << (32 - m_scratch_bits);
    m_scratch = m_scratch >> (32 - m_scratch_bits);

    m_buffer[m_word_index*4] = (uint8_t)m_scratch;
    m_buffer[m_word_index*4+1] = (uint8_t)(m_scratch>>8);
    m_buffer[m_word_index*4+2] = (uint8_t)(m_scratch>>16);
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
    //ctor
}

WriteStream::~WriteStream()
{
    //dtor
}

void WriteStream::setBuffer(uint8_t *buffer, int bytes)
{
    m_writer = std::make_unique<BitWriter> (buffer, bytes);
}

int WriteStream::computeBytes(int bits)
{
    if(m_writer)
        m_writer.get()->flush();

    return (int)((bits/32) + ((bits % 32) ? 1 : 0))*4;
}

int WriteStream::bitsRequired(int32_t min, int32_t max)
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


int WriteStream::serializeBits(int32_t value, int bits)
{
    if(!m_writer)
        return bits;

    m_writer.get()->writeBits(value, bits);
    return (1);
}

int WriteStream::serializeInteger(int32_t value, int32_t min, int32_t max)
{
    assert(min < max);
    assert(value >= min);
    assert(value <= max);

    const int bits = bitsRequired(min, max);

    if(!m_writer)
        return bits;

    uint32_t unsigned_value = value - min;
    m_writer.get()->writeBits(unsigned_value, bits);
    return (1);
}

int WriteStream::serializeBool(bool value)
{
    return this->serializeBits((int32_t)value, 1);
}

int WriteStream::serializeChar(char value)
{
    return this->serializeBits((int32_t)value, 8);
}



}
