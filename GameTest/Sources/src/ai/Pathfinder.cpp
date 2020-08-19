#include "ai/Pathfinder.h"

const int Pathfinder::MAX_PATHFINDING_TESTS = 256;

Pathfinder::Pathfinder()
{
    this->reset();
}

Pathfinder::~Pathfinder()
{
    //dtor
}


bool Pathfinder::findPath(glm::vec2 start, glm::vec2 destination,
                          float radius, float destinationRadius,
                          float minMass, int maxDepth)
{
   //return Pathfinder::instance()
   return this->findPathImpl(start, destination, radius, destinationRadius, minMass, maxDepth);
}

bool Pathfinder::pathFounded()
{
    return m_pathFounded;
}

std::list<glm::vec2> &Pathfinder::getPath()
{
    return m_path;
}

///
///Protected
///

void Pathfinder::reset()
{
    m_nodesToExplore.clear();
    m_exploredNodes.clear();
    m_path.clear();
    m_pathFounded = false;
}

bool Pathfinder::findPathImpl(glm::vec2 start, glm::vec2 destination, float pathRadius, float destinationRadius, float minMass, int maxDepth)
{
    this->reset();
    m_rayThickness  = pathRadius;
    m_minMass       = minMass;
    m_maxDepth      = maxDepth;
    m_destination   = destination;
    m_destinationRadius = destinationRadius;

    m_exploredNodes.push_back({start, nullptr, 0});

    PathNode destinationNode;
    destinationNode.depth = 1;
    destinationNode.parentNode = &m_exploredNodes.front();
    destinationNode.position = destination - destinationRadius * glm::normalize(destination - start);
    m_nodesToExplore.insert({this->estimateNodeWeight(destinationNode), destinationNode});

    //this->exploresNodes(&m_exploredNodes.front());
    this->exploresNodes();

    auto *lastNode = &(*m_exploredNodes.rbegin());
    /*if(lastNode == nullptr || lastNode->position != destination)
        return (false);*/

    while(lastNode != nullptr)
    {
        m_path.push_front(lastNode->position);
        lastNode = lastNode->parentNode;
    }

    m_pathFounded = true;

    this->simplifyPath();

    return (true);
}

void Pathfinder::addNodeToExplore(float d, PathNode &node)
{
    //glm::ivec2 cellPos = glm::round(node.position /*/2.0f*/);
    //if(m_alreadyVisitedCells.insert({cellPos.x, cellPos.y}).second)
        m_nodesToExplore.insert({d, node});
    //else
      //  this->unlockNodesToExplore(d, node);
}

void Pathfinder::unlockNodesToExplore(float d, PathNode &node)
{
    for(size_t i = 0 ; i < node.nodesToUnlock.size() ; ++i)
    {
        PathNode nodeToUnlock;
        nodeToUnlock.depth = node.depth+1;
        nodeToUnlock.parentNode = &m_exploredNodes.back();
        nodeToUnlock.position = node.nodesToUnlock[i];

        if(glm::round(nodeToUnlock.position) != glm::round(m_destination))
            nodeToUnlock.nodesToUnlock.push_back(m_destination);

        this->addNodeToExplore(d+this->estimateNodeWeight(nodeToUnlock), nodeToUnlock);
      //  m_nodesToExplore.insert({nodeIt->first+this->estimateNodeWeight(nodeToUnlock),
        //                        nodeToUnlock});
    }
}

//void Pathfinder::exploresNodes(PathNode *startNode)
void Pathfinder::exploresNodes()
{
    int nbrTest = 0;
    bool foundDestination = false;

    float closestDistance = -1;
    PathNode *closestNode(nullptr);


    ///std::vector<bool> alreadyVisitedCells;
    ///std::unordered_set<int, std::unordered_set<int> > m_alreadyVisitedCells;
    m_alreadyVisitedCells.clear();

    auto nodeIt = m_nodesToExplore.begin();
    while(!m_nodesToExplore.empty())
    {
        ++nbrTest;
        auto &node = nodeIt->second;

        if(node.depth >= m_maxDepth)
            break;

        if(nbrTest > Pathfinder::MAX_PATHFINDING_TESTS)
            break;

        auto start = node.parentNode->position;
        auto destination = node.position;

        auto collisionImpact = pou::PhysicsEngine::castCollisionDetectionRay(start, destination,
                                                                             m_rayThickness, m_minMass);

        if(collisionImpact.detectImpact)
            destination = collisionImpact.collisionImpact;

        m_exploredNodes.push_back({destination, node.parentNode, node.depth});

        float distance = glm::dot(destination - m_destination, destination - m_destination);
        if(distance < closestDistance || closestDistance == -1)
        {
            closestDistance = distance;
            closestNode = &m_exploredNodes.back();
        }

        if(collisionImpact.detectImpact)
        {
            PathNode corner1;
            corner1.depth = node.depth+1;
            corner1.parentNode = &m_exploredNodes.back();
            corner1.position = collisionImpact.corner1_1;
            corner1.nodesToUnlock.push_back(collisionImpact.corner1_2);
            //corner1.nodesToUnlock.push_back(m_destination);
            this->addNodeToExplore(nodeIt->first+this->estimateNodeWeight(corner1), corner1);
            //m_nodesToExplore.insert({nodeIt->first+this->estimateNodeWeight(corner1),
            //                        corner1});

            PathNode corner2;
            corner2.depth = node.depth+1;
            corner2.parentNode = &m_exploredNodes.back();
            corner2.position = collisionImpact.corner2_1;
            corner2.nodesToUnlock.push_back(collisionImpact.corner2_2);
            //corner2.nodesToUnlock.push_back(m_destination);
            this->addNodeToExplore(nodeIt->first+this->estimateNodeWeight(corner2), corner2);
            //m_nodesToExplore.insert({nodeIt->first+this->estimateNodeWeight(corner2),
            //                        corner2});
        } else {
            this->unlockNodesToExplore(nodeIt->first, node);
        }

        //if(destination == m_destination)
        if(glm::dot(destination - m_destination, destination - m_destination) < m_destinationRadius*m_destinationRadius)
        {
            foundDestination = true;
            break;
        }

        m_nodesToExplore.erase(nodeIt);
        nodeIt = m_nodesToExplore.begin();
    }

    //If we did not find the destination, we take the closest solution
    if(!foundDestination)
        m_exploredNodes.push_back(*closestNode);
    else
        std::cout<<"FOUND PATH"<<std::endl;

    std::cout<<"m_exploredNodes:"<<m_exploredNodes.size()<<std::endl;
}

void Pathfinder::simplifyPath()
{
    std::list<glm::vec2> oldPath;
    m_path.swap(oldPath);

    m_path.push_back(oldPath.front());


    while(m_path.back() != m_destination)
    {
        const auto &curNode = m_path.back();
        auto lastNode = m_destination;

        for(auto pathIt = oldPath.rbegin() ; pathIt != oldPath.rend() ; )
        {
            if(*pathIt == curNode)
            {
                m_path.push_back(lastNode);
                break;
            }

            auto collisionImpact = pou::PhysicsEngine::castCollisionDetectionRay(curNode, *pathIt,
                                                                                 20.0f, m_minMass);
            if(!collisionImpact.detectImpact)
            {
                m_path.push_back(*pathIt);
                break;
            }

            lastNode = *pathIt;
            ++pathIt;
        }
    }
}

float Pathfinder::estimateNodeWeight(const PathNode &p)
{
    if(!p.parentNode)
        return 0;
    return glm::length(p.position - p.parentNode->position);
   // return  glm::length(m_destination - p);
}



