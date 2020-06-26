#ifndef INTSYNCELEMENT_H
#define INTSYNCELEMENT_H

#include "PouEngine/sync/AbstractSyncElement.h"

namespace pou
{

class IntSyncElement : public AbstractSyncElement
{
    public:
        IntSyncElement();
        IntSyncElement(int v);
        virtual ~IntSyncElement();

        void useBits(size_t bits);
        void useMinMax(int min, int max);

        void    setValue(int v);
        int     getValue() const;

    protected:
        virtual void serializeImpl(Stream *stream, uint32_t clientTime);

    private:
        SyncAttribute<int> m_attribute;

        bool m_useBits;
        size_t m_bits;
        int m_min, m_max;
};

}

#endif // INTSYNCELEMENT_H
