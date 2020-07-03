#ifndef STRINGSYNCELEMENT_H
#define STRINGSYNCELEMENT_H

#include "PouEngine/sync/AbstractSyncElement.h"

namespace pou
{

class StringSyncElement : public AbstractSyncElement
{
    public:
        StringSyncElement();
        StringSyncElement(const std::string &v);
        virtual ~StringSyncElement();

        void operator=(const std::string& v);

        void setValue(const std::string &v);
        const std::string &getValue() const;

    protected:
        virtual void serializeImpl(Stream *stream, uint32_t clientTime);

    private:
        SyncAttribute<std::string> m_attribute;
};

}

#endif // STRINGSYNCELEMENT_H
