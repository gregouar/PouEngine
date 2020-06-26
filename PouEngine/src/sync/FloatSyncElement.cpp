#include "PouEngine/sync/FloatSyncElement.h"

namespace pou
{

FloatSyncElement::FloatSyncElement() : FloatSyncElement(0)
{
    //ctor
}

FloatSyncElement::FloatSyncElement(float v) : AbstractSyncElement(&m_attribute),
    m_attribute(v,-1),
    m_useMinMax(false)
{
    //ctor
}

FloatSyncElement::~FloatSyncElement()
{
    //dtor
}

void FloatSyncElement::useMinMax(bool use)
{
    m_useMinMax = use;
}

void FloatSyncElement::setMinMaxAndPrecision(float min, float max, size_t precision)
{
    if(max < min)
        return;

    this->useMinMax(true);
    m_min = min;
    m_max = max;
    m_precision = precision;
}

void FloatSyncElement::setValue(float v)
{
    if(m_attribute.setValue(v))
        this->updateLastUpdateTime();
}

float FloatSyncElement::getValue() const
{
    return m_attribute.getValue();
}

void FloatSyncElement::serializeImpl(Stream *stream, uint32_t clientTime)
{
    auto v = m_attribute.getValue();

    if(m_useMinMax)
        stream->serializeFloat(v, m_min, m_max, m_precision);
    else
        stream->serializeFloat(v);

    if(stream->isReading())
        m_attribute.setValue(v, true);
}

}
