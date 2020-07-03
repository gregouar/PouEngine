#include "PouEngine/sync/StringSyncElement.h"

namespace pou
{

StringSyncElement::StringSyncElement() : StringSyncElement(std::string())
{
    //ctor
}

StringSyncElement::StringSyncElement(const std::string &v) : AbstractSyncElement(&m_attribute),
    m_attribute(v,-1)
{
    this->useUpdateBit();
}

StringSyncElement::~StringSyncElement()
{
    //dtor
}

void StringSyncElement::operator=(const std::string& v)
{
    this->setValue(v);
}

void StringSyncElement::setValue(const std::string &v)
{
    if(m_attribute.setValue(v))
        this->updateLastUpdateTime();
}

const std::string &StringSyncElement::getValue() const
{
    return m_attribute.getValue();
}

void StringSyncElement::serializeImpl(Stream *stream, uint32_t clientTime)
{
    auto v = m_attribute.getValue();

    stream->serializeString(v);

    if(stream->isReading())
        m_attribute.setValue(v, true);
}


}
