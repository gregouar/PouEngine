#include "PouEngine/sync/Vec2SyncElement.h"

namespace pou
{

Vec2SyncElement::Vec2SyncElement() : Vec2SyncElement(glm::vec2(0))
{
    //ctor
}

Vec2SyncElement::Vec2SyncElement(const glm::vec2 &v) : AbstractSyncElement(&m_attribute),
    m_attribute(v,-1),
    m_useMinMax(false)
{
    this->useUpdateBit();
}

Vec2SyncElement::~Vec2SyncElement()
{
    //dtor
}

void Vec2SyncElement::useMinMax(bool use)
{
    m_useMinMax = use;
}

void Vec2SyncElement::setMinMaxAndPrecision(glm::vec2 min, glm::vec2 max, glm::uvec2 precision)
{
    if(max.x < min.x || max.y < min.y)
        return;

    this->useMinMax(true);
    m_min = min;
    m_max = max;
    m_precision = precision;
}

void Vec2SyncElement::setValue(const glm::vec2 &v)
{
    if(m_attribute.setValue(v))
        this->updateLastUpdateTime();
}

const glm::vec2 &Vec2SyncElement::getValue() const
{
    return m_attribute.getValue();
}

void Vec2SyncElement::serializeImpl(Stream *stream, uint32_t clientTime)
{
    auto v = m_attribute.getValue();

    if(m_useMinMax)
    {
        stream->serializeFloat(v.x, m_min.x, m_max.x, m_precision.x);
        stream->serializeFloat(v.y, m_min.y, m_max.y, m_precision.y);
    }
    else
    {
        stream->serializeFloat(v.x);
        stream->serializeFloat(v.y);
    }

    if(stream->isReading())
        m_attribute.setValue(v, true);
}

}
