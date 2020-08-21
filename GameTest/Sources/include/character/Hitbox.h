#ifndef HITBOX_H
#define HITBOX_H

#include "PouEngine/tools/MathTools.h"

#include "tinyxml/tinyxml.h"

#include "Types.h"

#include <vector>

class Hitbox
{
    public:
        Hitbox();
        virtual ~Hitbox();

        bool loadFromXML(TiXmlElement *element);

        float getFactor(DamageType type) const;
        float getFactor(int type) const;
        pou::HashedString getSkeleton() const;
        pou::HashedString getNode() const;
        const pou::MathTools::Box &getBox() const;
        const glm::vec4 &getColor() const;
        bool getBounce() const;

    protected:

    private:
        pou::HashedString m_skeleton;
        pou::HashedString m_node;

        std::vector<float>  m_factors;
        pou::MathTools::Box m_box;
        glm::vec4           m_color; //Use for hit highlight
        bool                m_bounce;
};

#endif // HITBOX_H
