#ifndef FLOATSYNCELEMENT_H
#define FLOATSYNCELEMENT_H

#include "PouEngine/sync/AbstractSyncElement.h"

namespace pou
{

class FloatSyncElement : public AbstractSyncElement
{
    public:
        FloatSyncElement();
        FloatSyncElement(float v);
        virtual ~FloatSyncElement();

        void useMinMax(bool use = true);
        void setMinMaxAndPrecision(float min, float max, size_t precision);

        void    setValue(float v);
        float   getValue() const;

    protected:
        virtual void serializeImpl(Stream *stream, uint32_t clientTime);

    private:
        SyncAttribute<float> m_attribute;

        bool m_useMinMax;
        float m_min, m_max;
        size_t m_precision;
};

}

#endif // FLOATSYNCELEMENT_H
