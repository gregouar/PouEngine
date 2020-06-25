#ifndef BOOLSYNCELEMENT_H
#define BOOLSYNCELEMENT_H

#include "PouEngine/sync/AbstractSyncElement.h"

namespace pou
{

class BoolSyncElement : public AbstractSyncElement
{
    public:
        BoolSyncElement();
        BoolSyncElement(bool v);
        virtual ~BoolSyncElement();

        void    setValue(bool v);
        bool    getValue();

    protected:
        virtual void serializeImpl(Stream *stream, uint32_t clientTime);

    private:
        SyncAttribute<bool> m_attribute;
};

}

#endif // BOOLSYNCELEMENT_H
