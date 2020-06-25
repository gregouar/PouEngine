#include "PouEngine/sync/Vec3SyncElement.h"

namespace pou
{

Vec3SyncElement::Vec3SyncElement() : Vec3SyncElement(glm::vec3(0))
{
    //ctor
}

Vec3SyncElement::Vec3SyncElement(glm::vec3 v) : AbstractSyncElement(&m_attribute),
    m_attribute(v,0),
    m_useMinMax(false)
{
    this->useUpdateBit();
}

Vec3SyncElement::~Vec3SyncElement()
{
    //dtor
}

void Vec3SyncElement::useMinMax(bool use)
{
    m_useMinMax = use;
}

void Vec3SyncElement::setMinMaxAndPrecision(glm::vec3 min, glm::vec3 max, glm::uvec3 precision)
{
    if(max.x < min.x || max.y < min.y || max.z < min.z)
        return;

    this->useMinMax(true);
    m_min = min;
    m_max = max;
    m_precision = precision;
}

void Vec3SyncElement::setValue(const glm::vec3 &v)
{
    if(m_attribute.setValue(v))
        this->updateLastUpdateTime();
}

const glm::vec3 &Vec3SyncElement::getValue()
{
    return m_attribute.getValue();
}

void Vec3SyncElement::serializeImpl(Stream *stream, uint32_t clientTime)
{
    auto v = m_attribute.getValue();

    if(m_useMinMax)
    {
        stream->serializeFloat(v.x, m_min.x, m_max.x, m_precision.x);
        stream->serializeFloat(v.y, m_min.y, m_max.y, m_precision.y);
        stream->serializeFloat(v.z, m_min.z, m_max.z, m_precision.z);
    }
    else
    {
        stream->serializeFloat(v.x);
        stream->serializeFloat(v.y);
        stream->serializeFloat(v.z);
    }

    if(stream->isReading())
        m_attribute.setValue(v, true);
}

}
