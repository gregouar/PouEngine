#ifndef VEC3SYNCELEMENT_H
#define VEC3SYNCELEMENT_H

#include "PouEngine/sync/AbstractSyncElement.h"

namespace pou
{

class Vec3SyncElement : public AbstractSyncElement
{
    public:
        Vec3SyncElement();
        Vec3SyncElement(glm::vec3 v);
        virtual ~Vec3SyncElement();

        void useMinMax(bool use = true);
        void setMinMaxAndPrecision(glm::vec3 min, glm::vec3 max, glm::uvec3 precision);

        void setValue(const glm::vec3 &v);
        const glm::vec3 &getValue() const;

    protected:
        virtual void serializeImpl(Stream *stream, uint32_t clientTime);

    private:
        SyncAttribute<glm::vec3> m_attribute;

        bool m_useMinMax;
        glm::vec3   m_min, m_max;
        glm::uvec3  m_precision;
};

}

#endif // VEC3SYNCELEMENT_H
