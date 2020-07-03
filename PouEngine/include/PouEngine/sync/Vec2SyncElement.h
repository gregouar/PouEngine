#ifndef VEC2SYNCELEMENT_H
#define VEC2SYNCELEMENT_H

#include "PouEngine/sync/AbstractSyncElement.h"

namespace pou
{

class Vec2SyncElement : public AbstractSyncElement
{
    public:
        Vec2SyncElement();
        Vec2SyncElement(const glm::vec2 &v);
        virtual ~Vec2SyncElement();

        void useMinMax(bool use = true);
        void setMinMaxAndPrecision(glm::vec2 min, glm::vec2 max, glm::uvec2 precision);

        void operator=(const glm::vec2& v);

        void setValue(const glm::vec2 &v);
        const glm::vec2 &getValue() const;

    protected:
        virtual void serializeImpl(Stream *stream, uint32_t clientTime) override;

    private:
        SyncAttribute<glm::vec2> m_attribute;

        bool m_useMinMax;
        glm::vec2 m_min, m_max;
        glm::uvec2 m_precision;
};

}

#endif // VEC2SYNCELEMENT_H
