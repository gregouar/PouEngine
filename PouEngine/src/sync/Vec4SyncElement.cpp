#include "PouEngine/sync/Vec4SyncElement.h"

namespace pou
{


Vec4SyncElement::Vec4SyncElement() : Vec4SyncElement(glm::vec4(0))
{
    //ctor
}

Vec4SyncElement::Vec4SyncElement(glm::vec4 v) : AbstractSyncElement(&m_attribute),
    m_attribute(v,-1),
    m_useMinMax(false)
{
    this->useUpdateBit();
}

Vec4SyncElement::~Vec4SyncElement()
{
    //dtor
}

void Vec4SyncElement::useMinMax(bool use)
{
    m_useMinMax = use;
}

void Vec4SyncElement::setMinMaxAndPrecision(glm::vec4 min, glm::vec4 max, glm::uvec4 precision)
{
    if(max.x < min.x || max.y < min.y || max.z < min.z || max.w < min.w)
        return;

    this->useMinMax(true);
    m_min = min;
    m_max = max;
    m_precision = precision;
}

void Vec4SyncElement::setValue(const glm::vec4 &v)
{
    if(m_attribute.setValue(v))
        this->updateLastUpdateTime();
}

const glm::vec4 &Vec4SyncElement::getValue() const
{
    return m_attribute.getValue();
}

void Vec4SyncElement::serializeImpl(Stream *stream, uint32_t clientTime)
{
    auto v = m_attribute.getValue();

    if(m_useMinMax)
    {
        stream->serializeFloat(v.x, m_min.x, m_max.x, m_precision.x);
        stream->serializeFloat(v.y, m_min.y, m_max.y, m_precision.y);
        stream->serializeFloat(v.z, m_min.z, m_max.z, m_precision.z);
        stream->serializeFloat(v.w, m_min.w, m_max.w, m_precision.w);
    }
    else
    {
        stream->serializeFloat(v.x);
        stream->serializeFloat(v.y);
        stream->serializeFloat(v.z);
        stream->serializeFloat(v.w);
    }

    if(stream->isReading())
        m_attribute.setValue(v, true);
}

///
///Vec4LinSyncElement
///


Vec4LinSyncElement::Vec4LinSyncElement() : Vec4LinSyncElement(glm::vec4(0))
{
    //ctor
}

Vec4LinSyncElement::Vec4LinSyncElement(glm::vec4 v) : AbstractSyncElement(&m_attribute),
    m_attribute(v,-1),
    m_useMinMax(false)
{
    this->useUpdateBit();
}

Vec4LinSyncElement::~Vec4LinSyncElement()
{
    //dtor
}

void Vec4LinSyncElement::useMinMax(bool use)
{
    m_useMinMax = use;
}

void Vec4LinSyncElement::setMinMaxAndPrecision(glm::vec4 min, glm::vec4 max, glm::uvec4 precision)
{
    if(max.x < min.x || max.y < min.y || max.z < min.z || max.w < min.w)
        return;

    this->useMinMax(true);
    m_min = min;
    m_max = max;
    m_precision = precision;
}

void Vec4LinSyncElement::setValue(const glm::vec4 &v)
{
    if(m_attribute.setValue(v))
        this->updateLastUpdateTime();
}

const glm::vec4 &Vec4LinSyncElement::getValue() const
{
    return m_attribute.getValue();
}

void Vec4LinSyncElement::serializeImpl(Stream *stream, uint32_t clientTime)
{
    auto v = m_attribute.getValue();

    if(m_useMinMax)
    {
        stream->serializeFloat(v.x, m_min.x, m_max.x, m_precision.x);
        stream->serializeFloat(v.y, m_min.y, m_max.y, m_precision.y);
        stream->serializeFloat(v.z, m_min.z, m_max.z, m_precision.z);
        stream->serializeFloat(v.w, m_min.w, m_max.w, m_precision.w);
    }
    else
    {
        stream->serializeFloat(v.x);
        stream->serializeFloat(v.y);
        stream->serializeFloat(v.z);
        stream->serializeFloat(v.w);
    }

    if(stream->isReading())
        m_attribute.setValue(v, true);
}

}
