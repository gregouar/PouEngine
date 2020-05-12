#include "PouEngine/utils/Stream.h"

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


int Stream::computeBytes()
{
    return (int)((m_bits/32) + ((m_bits % 32) ? 1 : 0))*4;
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

    return bits;
}

void Stream::padZeroes()
{
    int zeroes = 0;
    int rem_bits = 8 - m_bits%8;

    this->serializeBits(zeroes, rem_bits);
}


}
