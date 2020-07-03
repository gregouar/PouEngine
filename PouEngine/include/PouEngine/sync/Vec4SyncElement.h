#ifndef VEC4SYNCELEMENT_H
#define VEC4SYNCELEMENT_H

#include "PouEngine/sync/AbstractSyncElement.h"

namespace pou
{

class Vec4SyncElement : public AbstractSyncElement
{
    public:
        Vec4SyncElement();
        Vec4SyncElement(glm::vec4 v);
        virtual ~Vec4SyncElement();

        void useMinMax(bool use = true);
        void setMinMaxAndPrecision(glm::vec4 min, glm::vec4 max, glm::uvec4 precision);

        void operator=(const glm::vec4& v);

        void setValue(const glm::vec4 &v);
        const glm::vec4 &getValue() const;

    protected:
        virtual void serializeImpl(Stream *stream, uint32_t clientTime);

    private:
        SyncAttribute<glm::vec4> m_attribute;

        bool m_useMinMax;
        glm::vec4   m_min, m_max;
        glm::uvec4  m_precision;
};


class Vec4LinSyncElement : public AbstractSyncElement
{
    public:
        Vec4LinSyncElement();
        Vec4LinSyncElement(glm::vec4 v);
        virtual ~Vec4LinSyncElement();

        void useMinMax(bool use = true);
        void setMinMaxAndPrecision(glm::vec4 min, glm::vec4 max, glm::uvec4 precision);

        void operator=(const glm::vec4& v);

        void setValue(const glm::vec4 &v);
        const glm::vec4 &getValue() const;

    protected:
        virtual void serializeImpl(Stream *stream, uint32_t clientTime);

    private:
        LinSyncAttribute<glm::vec4> m_attribute;

        bool m_useMinMax;
        glm::vec4   m_min, m_max;
        glm::uvec4  m_precision;
};



}

#endif // VEC4SYNCELEMENT_H
