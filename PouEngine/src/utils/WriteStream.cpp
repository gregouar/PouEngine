#include "PouEngine/utils/WriteStream.h"

#include "PouEngine/utils/Logger.h"

#include <glm/glm.hpp>

#include <string>
#include <iostream>

namespace pou
{

/// BitWriter


BitWriter::BitWriter(uint8_t *buffer, int bytes) : m_buffer(buffer), m_bytes(bytes),
    m_scratch(0), m_scratch_bits(0), m_byte_index(0)
{

}

BitWriter::~BitWriter()
{

}


bool BitWriter::writeBits(uint32_t unsigned_value, int bits)
{
    if(m_byte_index >= m_bytes)
        return (false);

    unsigned_value = (unsigned_value << (32 - bits)) >> (32 - bits);

    m_scratch |= (uint64_t)(unsigned_value) << m_scratch_bits ;
    m_scratch_bits += bits;

    if(m_scratch_bits >= 32)
    {
        m_buffer[m_byte_index] = (uint8_t)m_scratch;
        if(m_byte_index+1 < m_bytes)
            m_buffer[m_byte_index+1] = (uint8_t)(m_scratch>>8);
        if(m_byte_index+2 < m_bytes)
            m_buffer[m_byte_index+2] = (uint8_t)(m_scratch>>16);
        if(m_byte_index+3 < m_bytes)
            m_buffer[m_byte_index+3] = (uint8_t)(m_scratch>>24);
        m_byte_index += 4;

        m_scratch_bits -= 32;
        m_scratch = m_scratch >> 32;

        if(m_byte_index >= m_bytes && m_scratch_bits > 0)
            return (false);
    }

    return (true);
}

bool BitWriter::memcpy(const uint8_t *data, int data_size)
{
    int bytes_shift = (m_scratch_bits/8);

    m_byte_index += bytes_shift;
    assert(m_byte_index + data_size <= m_bytes);
    std::memcpy( m_buffer + m_byte_index, data, data_size);

    m_byte_index += data_size;
    m_scratch_bits = 0;
    m_scratch = 0;

    return (true);
}

void BitWriter::flush()
{
    m_scratch = m_scratch << (32 - m_scratch_bits);
    m_scratch = m_scratch >> (32 - m_scratch_bits);

    m_buffer[m_byte_index] = (uint8_t)m_scratch;
    if(m_byte_index+1 < m_bytes)
        m_buffer[m_byte_index+1] = (uint8_t)(m_scratch>>8);
    if(m_byte_index+2 < m_bytes)
        m_buffer[m_byte_index+2] = (uint8_t)(m_scratch>>16);
    if(m_byte_index+3 < m_bytes)
        m_buffer[m_byte_index+3] = (uint8_t)(m_scratch>>24);
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


void WriteStream::setBuffer(std::vector<uint8_t> &buffer)
{
    this->setBuffer(buffer.data(), buffer.size());
}

void WriteStream::setBuffer(uint8_t *buffer, int bytes)
{
    m_writer = std::make_unique<BitWriter> (buffer, bytes);
}


void WriteStream::flush()
{
    if(m_writer)
        m_writer->flush();
}

bool WriteStream::memcpy(uint8_t *data, int data_size)
{
    return this->const_memcpy(data,data_size);
}

bool WriteStream::const_memcpy(const uint8_t *data, int data_size)
{
    Stream::padZeroes();

    this->flush();

    if(m_writer)
        if(!m_writer->memcpy(data, data_size))
            return (false);

    m_bits += data_size*8;
    return (true);
}

bool WriteStream::serializeBits(int32_t &value, int bits)
{
    m_bits += bits;

    if(m_writer)
        return m_writer->writeBits(value, bits);

    return (true);
}

bool WriteStream::serializeInt(int32_t &value, int32_t min, int32_t max)
{
    /*assert(min < max);
    assert(value >= min);
    assert(value <= max);*/

    if(value < min || value > max)
        Logger::warning("WriteStream is trying to write value:"+
                        std::to_string(value)+" not in ["+std::to_string(min)+","+std::to_string(max)+"]");

    value = glm::clamp(value, min, max);

    const int bits = bitsRequired(min, max);

    m_bits += bits;

    uint32_t unsigned_value = value - min;

    if(m_writer)
        return m_writer->writeBits(unsigned_value, bits);

    return (true);
}

bool WriteStream::serializeFloat(float &value)
{
    int v = 0;
    std::memcpy(&v, &value, sizeof(float));
    return this->serializeBits(v,32);
}

bool WriteStream::serializeFloat(float &value, float min, float max, uint8_t decimals)
{
    /*if(value < min || value > max)
        std::cout<<min<<" < "<<value<<" < "<<max<<" (dec: "<<decimals<<std::endl;
    assert(min < max);
    assert(value >= min);
    assert(value <= max);*/

    if(value < min || value > max)
        Logger::warning("WriteStream is trying to write value:"
                        +std::to_string(value)+" not in ["+std::to_string(min)+","+std::to_string(max)+"]");

    value = glm::clamp(value, min, max);

    decimals = pow(10,decimals);

    int32_t minInt = min*decimals;
    int32_t maxInt = max*decimals;
    const int bits = bitsRequired(minInt, maxInt);

    //m_bits += bits;

    int unsigned_value = (uint32_t)((value - min)*decimals);

    return this->serializeBits(unsigned_value,bits);
}

bool WriteStream::serializeBool(bool &value)
{
    int32_t temp = (int32_t)value;
    return this->serializeBits(temp, 1);
}

bool WriteStream::serializeChar(char &value)
{
    int32_t temp = (int32_t)value;
    return this->serializeBits(temp, 8);
}

bool WriteStream::serializeString(std::string &str)
{
    int strSize = str.size();

    assert(strSize < 256);

    this->serializeBits(strSize, 8);

    const uint8_t* temp = reinterpret_cast<const uint8_t *>(str.c_str());
    return this->const_memcpy(temp, strSize);
}


}
