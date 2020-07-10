#ifndef COLLISIONOBJECT_H
#define COLLISIONOBJECT_H

#include "PouEngine/scene/SceneObject.h"
#include "PouEngine/physics/PhysicsEngine.h"

namespace pou
{

class CollisionObject : public SceneObject
{
    public:
        CollisionObject();
        virtual ~CollisionObject();

        virtual void update(const Time &elapsedTime, uint32_t localTime = -1);

        void setMass(float mass);

        void setBox(const glm::vec2 &size);
        void setBox(const glm::vec2 &size, const glm::vec2 &center);
        void setBox(const MathTools::Box &box);

        void setDisk(float radius);

    protected:
        virtual SceneNode *setParentNode(SceneNode*);

    private:
        bool m_isBoxBody;
        BoxBody m_boxBody;

        bool m_isDiskBody;
        DiskBody m_diskBody;
};

}

#endif // COLLISIONOBJECT_H
