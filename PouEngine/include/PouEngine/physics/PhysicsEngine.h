#ifndef PHYSICSENGINE_H
#define PHYSICSENGINE_H

#include "PouEngine/core/Singleton.h"
#include "PouEngine/tools/MathTools.h"

namespace pou
{

struct BoxBody
{
    SimpleNode *node;
    float mass;
    MathTools::Box box;
    //add bounding box or something, so that we do not need to cache *node when reading
};

struct DiskBody
{
    SimpleNode *node;
    float mass;
    float radius;
    //add global pos ? => so that we do not need to cache *node when reading
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

    protected:
        PhysicsEngine();
        virtual ~PhysicsEngine();

        void resolveCollisionsImpl(/*const Time &elapsedTime*/);

        float computeMassRatio(float mass1, float mass2);

        void addBoxBodyImpl(const BoxBody &box);
        //void addBoxBodyImpl(SimpleNode *node, const BoxBody &box);
        //void addBoxBodiesImpl(SimpleNode *node, const std::vector<BoxBody> &boxes);

        void addDiskBodyImpl(const DiskBody &disk);
        //void addDiskBodyImpl(SimpleNode *node, const DiskBody &disk);
        //void addDiskBodiesImpl(SimpleNode *node, const std::vector<DiskBody> &diks);

        void resolveBoxMinkowskiDiff(const glm::vec2 &closestPoint,
                                        SimpleNode *node1, SimpleNode *node2,
                                        float mass1, float mass2);
        void resolveBoxToBoxCollision(BoxBody *body1, BoxBody *body2);
        void computeMinkowskiDiff(const glm::vec2 &originBoxSize,
                                  glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, /*glm::vec2 p4,*/
                                  std::vector<glm::vec2> &minkowskiDiff);

        void resolveDiskMinkowskiDiff(const glm::vec2 &diskCenter, float diskRadius,
                                        SimpleNode *node1, SimpleNode *node2,
                                        float mass1, float mass2);
        void resolveBoxToDiskCollision(BoxBody *boxBody, DiskBody *diskBody);
        /*void computeMinkowskiDiff(const glm::vec2 &originBoxSize,
                                  glm::vec2 diskOrigin, float radius,
                                  std::vector<glm::vec2> &minkowskiDiff);*/

        void resolveDiskToDiskCollision(DiskBody *body1, DiskBody *body2);




    private:
        std::vector<BoxBody> m_boxBodies;
        std::vector<DiskBody> m_diskBodies;
};

}

#endif // PHYSICSENGINE_H
