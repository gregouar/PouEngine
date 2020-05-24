#include "PouEngine/utils/Stream.h"

#include "PouEngine/utils/Logger.h"

namespace pou
{

Stream::Stream() : m_bits(0)
{
    //ctor
}

Stream::~Stream()
{
    //dtor
}

void Stream::setBuffer(std::vector<uint8_t> &buffer)
{
    this->setBuffer(buffer.data(), buffer.size());
}

int Stream::computeBytes()
{
    int bits = m_bits;
    m_bits = 0;
    //std::cout<<"NBR BYTES:"<<(int)((bits/8) + ((bits % 8) ? 1 : 0))<<std::endl;
    return (int)((bits/8) + ((bits % 8) ? 1 : 0));//(int)((bits/32) + ((bits % 32) ? 1 : 0))*4;
}

void Stream::flush()
{
}

int Stream::bitsRequired(int32_t min, int32_t max)
{
    int bits = 1;
    uint32_t cur = max-min;

    while(cur/2 > 0)
    {
        cur = cur/2;
        bits++;
    }

    //std::cout<<bits<<" bits required to represent"<<max-min<<std::endl;

    return bits;
}

void Stream::padZeroes()
{
    int zeroes = 0;
    int rem_bits = m_bits%8;
    if(rem_bits != 0)
    {
        rem_bits = 8 - rem_bits;
        this->serializeBits(zeroes, rem_bits);
    }
    /*int rem_bits = m_bits%32;
    if(rem_bits != 0)
        rem_bits = 32 - rem_bits;*/
}


}
