#include "PouEngine/sync/IntSyncElement.h"

namespace pou
{

IntSyncElement::IntSyncElement() : IntSyncElement(0)
{
    //ctor
}

IntSyncElement::IntSyncElement(int v) : AbstractSyncElement(&m_attribute),
    m_attribute(v,-1),
    m_useBits(true),
    m_bits(32)
{
    //ctor
}

IntSyncElement::~IntSyncElement()
{
    //dtor
}

void IntSyncElement::useBits(size_t bits)
{
    m_useBits = true;
    m_bits = bits;
}

void IntSyncElement::useMinMax(int min, int max)
{
    if(max < min)
        return;

    m_useBits = false;
    m_min = min;
    m_max = max;
}

void IntSyncElement::operator=(int v)
{
    this->setValue(v);
}

void IntSyncElement::setValue(int v)
{
    if(m_attribute.setValue(v))
        this->updateLastUpdateTime();
}

int IntSyncElement::getValue() const
{
    return m_attribute.getValue();
}

void IntSyncElement::serializeImpl(Stream *stream, uint32_t clientTime)
{
    auto v = m_attribute.getValue();

    if(m_useBits)
        stream->serializeBits(v, m_bits);
    else
        stream->serializeInt(v, m_min, m_max);

    if(stream->isReading())
        m_attribute.setValue(v, true);
}

}

