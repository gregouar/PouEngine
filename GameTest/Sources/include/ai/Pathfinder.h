#ifndef PATHFINDER_H
#define PATHFINDER_H

//#include "PouEngine/core/Singleton.h"
#include "PouEngine/physics/PhysicsEngine.h"


struct PathNode
{
    glm::vec2 position;
    PathNode *parentNode;
    int depth;
    //float distanceFromStart;

    std::vector<glm::vec2> nodesToUnlock;
};

class Pathfinder //: public Singleton<Pathfinder>
{
    //friend class Singleton<Pathfinder>;

    public:
        Pathfinder();
        virtual ~Pathfinder();
        /*static*/ bool findPath(glm::vec2 start, glm::vec2 destination,
                                 float pathRadius = 0, float destinationRadius = 0,
                                 float minMassAvoidance = -1, int maxDepth = 32);

        bool pathFounded();
        std::list<glm::vec2> &getPath();

    protected:

        void reset();
        bool findPathImpl(glm::vec2 start, glm::vec2 destination, float radius, float destinationRadius, float minMass, int maxDepth);
        //void exploresNodes(PathNode *startNode);

        void addNodeToExplore(float d, PathNode &node);
        void unlockNodesToExplore(float d, PathNode &node);
        void exploresNodes();

        void simplifyPath();

        float estimateNodeWeight(const PathNode &node);

    private:

        pou::unordered_set_intpair m_alreadyVisitedCells;
        std::multimap<float, PathNode> m_nodesToExplore;
        std::list<PathNode> m_exploredNodes;

        float m_rayThickness;
        float m_destinationRadius;
        float m_minMass;
        int m_maxDepth;
        glm::vec2 m_destination;

        bool m_pathFounded;
        std::list<glm::vec2> m_path;

    public:
        static const int MAX_PATHFINDING_TESTS;
};

#endif // PATHFINDER_H
