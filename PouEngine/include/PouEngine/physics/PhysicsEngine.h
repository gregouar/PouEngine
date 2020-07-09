#ifndef PHYSICSENGINE_H
#define PHYSICSENGINE_H

#include "PouEngine/core/Singleton.h"
#include "PouEngine/tools/MathTools.h"

namespace pou
{

struct BoxBody
{
    SimpleNode *node;
    MathTools::Box box;
};

class PhysicsEngine : public Singleton<PhysicsEngine>
{
    friend class Singleton<PhysicsEngine>;

    public:
        static void resolveCollisions(/*const Time &elapsedTime*/);

        static void addBoxBody(SimpleNode *node, const BoxBody &box);
        static void addBoxBodies(SimpleNode *node, const std::vector<BoxBody> &boxes);

    protected:
        PhysicsEngine();
        virtual ~PhysicsEngine();

        void resolveCollisionsImpl(/*const Time &elapsedTime*/);

        void addBoxBodyImpl(SimpleNode *node, const BoxBody &box);
        void addBoxBodiesImpl(SimpleNode *node, const std::vector<BoxBody> &boxes);

        void resolveBoxToBoxCollision(BoxBody *body1, BoxBody *body2);
        void computeMinkowskiDiff(const glm::vec2 &originBoxSize,
                                  glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, /*glm::vec2 p4,*/
                                  std::vector<glm::vec2> &minkowskiDiff);



    private:
        std::vector<BoxBody> m_boxBodies;
};

}

#endif // PHYSICSENGINE_H
