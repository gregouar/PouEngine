#ifndef PHYSICSENGINE_H
#define PHYSICSENGINE_H

#include "PouEngine/core/Singleton.h"
#include "PouEngine/tools/MathTools.h"
#include "PouEngine/math/TransformComponent.h"

#include <vector>

namespace pou
{

struct BoxBody
{
    TransformComponent *transform;
    float mass;
    MathTools::Box box;
};

struct DiskBody
{
    TransformComponent *transform;
    float mass;
    float radius;
};

struct RigidBody
{
    TransformComponent *transform;
    float mass;

    bool isBox;
    MathTools::Box box;

    bool isDisk;
    float radius;

    float estimatedRightMost;
};

struct CollisionDetectionImpact
{
    bool detectImpact;
    glm::vec2 collisionImpact;

    glm::vec2 corner1_1;
    glm::vec2 corner1_2;

    glm::vec2 corner2_1;
    glm::vec2 corner2_2;
};

class PhysicsEngine : public Singleton<PhysicsEngine>
{
    friend class Singleton<PhysicsEngine>;

    public:
        static void resolveCollisions(/*const Time &elapsedTime*/);

        static void addBoxBody(const BoxBody &box);
        //static void addBoxBody(SimpleNode *node, const BoxBody &box);
        //static void addBoxBodies(SimpleNode *node, const std::vector<BoxBody> &boxes);


        static void addDiskBody(const DiskBody &disk);
        //static void addDiskBody(SimpleNode *node, const DiskBody &disk);
        //static void addDiskBodies(SimpleNode *node, const std::vector<DiskBody> &disks);

        static CollisionDetectionImpact castCollisionDetectionRay(glm::vec2 startPoint, glm::vec2 endPoint,
                                                                  float rayThickness = 0, float minMass = -1);

    protected:
        PhysicsEngine();
        virtual ~PhysicsEngine();

        void resolveCollisionsImpl(/*const Time &elapsedTime*/);

        float computeMassRatio(float mass1, float mass2);

        void addBoxBodyImpl(const BoxBody &box);
        void addDiskBodyImpl(const DiskBody &disk);

        void resolveBoxMinkowskiDiff(const glm::vec2 &closestPoint,
                                    TransformComponent *transform1, TransformComponent *transform2,
                                    float mass1, float mass2);
       // void resolveBoxToBoxCollision(BoxBody *body1, BoxBody *body2);
        void resolveBoxToBoxCollision(RigidBody *body1, RigidBody *body2);
        void computeMinkowskiDiff(const glm::vec2 &originBoxSize,
                                  glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, /*glm::vec2 p4,*/
                                  std::vector<glm::vec2> &minkowskiDiff);

        void resolveDiskMinkowskiDiff(const glm::vec2 &diskCenter, float diskRadius,
                                    TransformComponent *transform1, TransformComponent *transform2,
                                    float mass1, float mass2);
        //void resolveBoxToDiskCollision(BoxBody *boxBody, DiskBody *diskBody);
        void resolveBoxToDiskCollision(RigidBody *boxBody, RigidBody *diskBody);
        /*void computeMinkowskiDiff(const glm::vec2 &originBoxSize,
                                  glm::vec2 diskOrigin, float radius,
                                  std::vector<glm::vec2> &minkowskiDiff);*/

        //void resolveDiskToDiskCollision(DiskBody *body1, DiskBody *body2);
        void resolveDiskToDiskCollision(RigidBody *body1, RigidBody *body2);

        CollisionDetectionImpact castCollisionDetectionRayImpl(glm::vec2 startPoint, glm::vec2 endPoint,
                                                                float rayThickness, float minMass);

        std::pair<int, glm::vec2> testRayCollision(glm::vec2 startPoint, glm::vec2 normalizedRayVect,
                                                    float rayThickness, RigidBody *body);


    private:
        /*std::vector<BoxBody> m_boxBodies;
        std::vector<DiskBody> m_diskBodies;*/

        /*std::map<float, BoxBody>  m_boxBodies;
        std::map<float, DiskBody>  m_diskBodies;*/

        std::multimap<float, RigidBody> m_oldRigidBodies;
        std::multimap<float, RigidBody> m_rigidBodies;



};

}

#endif // PHYSICSENGINE_H
