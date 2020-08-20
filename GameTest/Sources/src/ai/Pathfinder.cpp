#include "ai/Pathfinder.h"

const int Pathfinder::MAX_PATHFINDING_TESTS = 512;

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

std::list<PathNode> &Pathfinder::getExploredNodes()
{
    return m_exploredNodes;
}

///
///Protected
///

void Pathfinder::reset()
{
    m_nodesToExplore.clear();
    m_exploredNodes.clear();
    m_alreadyVisitedCells.clear();
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

    ///m_exploredNodes.push_back({start, nullptr, 0});
    this->addExploredNode(start, nullptr, 0);

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
    glm::ivec2 cellPos = glm::round(node.position / 4.0f );

    auto visitedCell = m_alreadyVisitedCells.find({cellPos.x, cellPos.y});
    if(visitedCell != m_alreadyVisitedCells.end())
        this->unlockNodesToExplore(d, node, visitedCell->second);
    else
        m_nodesToExplore.insert({d, node});
}

void Pathfinder::unlockNodesToExplore(float d, PathNode &node, PathNode *parentNode)
{
    for(size_t i = 0 ; i < node.nodesToUnlock.size() ; ++i)
    {
        PathNode nodeToUnlock;
        nodeToUnlock.depth = parentNode->depth+1;
        nodeToUnlock.parentNode = parentNode;//&m_exploredNodes.back();
        nodeToUnlock.position = node.nodesToUnlock[i];

        if(glm::round(nodeToUnlock.position / 4.0f) != glm::round(m_destination / 4.0f))
        {
            nodeToUnlock.nodesToUnlock.push_back(m_destination);
            for(size_t j = 0 ; j < node.nodesToUnlockAfter.size() ; ++j)
                nodeToUnlock.nodesToUnlock.push_back(node.nodesToUnlockAfter[j]);
        }

        this->addNodeToExplore(d+this->estimateNodeWeight(nodeToUnlock), nodeToUnlock);
      //  m_nodesToExplore.insert({nodeIt->first+this->estimateNodeWeight(nodeToUnlock),
        //                        nodeToUnlock});
    }
}

void Pathfinder::addExploredNode(glm::vec2 pos, PathNode *parentNode, int depth)
{
    m_exploredNodes.push_back({pos, parentNode, depth});

    glm::ivec2 cellPos = glm::round(pos / 4.0f);
    if(!m_alreadyVisitedCells.insert({{cellPos.x, cellPos.y}, &m_exploredNodes.back()}).second)
       m_exploredNodes.pop_back();
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

    auto nodeIt = m_nodesToExplore.begin();
    while(!m_nodesToExplore.empty())
    {
        ++nbrTest;
        auto &node = nodeIt->second;

        /**if(node.depth >= m_maxDepth)
            break;**/

        if(nbrTest > Pathfinder::MAX_PATHFINDING_TESTS)
            break;

        auto start = node.parentNode->position;
        auto destination = node.position;

        ///Test
        auto cellPos = glm::round(destination / 4.0f);
        auto visitedCell = m_alreadyVisitedCells.find({cellPos.x, cellPos.y});
        if(visitedCell != m_alreadyVisitedCells.end())
            this->unlockNodesToExplore(nodeIt->first, node, visitedCell->second);
        else
        {
        ///

        auto collisionImpact = pou::PhysicsEngine::castCollisionDetectionRay(start, destination,
                                                                             m_rayThickness, m_minMass);

        if(collisionImpact.detectImpact)
            destination = collisionImpact.collisionImpact;

        ///m_exploredNodes.push_back({destination, node.parentNode, node.depth});
        this->addExploredNode(destination, node.parentNode, node.depth);

        //glm::ivec2 cellPos = glm::round(destination / 4.0f);
        //m_alreadyVisitedCells.insert({{cellPos.x, cellPos.y}, &m_exploredNodes.back()});

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
            corner1.nodesToUnlockAfter.push_back(collisionImpact.corner2_2);
            //corner1.nodesToUnlock.push_back(m_destination);
            this->addNodeToExplore(nodeIt->first+this->estimateNodeWeight(corner1), corner1);
            //m_nodesToExplore.insert({nodeIt->first+this->estimateNodeWeight(corner1),
            //                        corner1});

            PathNode corner2;
            corner2.depth = node.depth+1;
            corner2.parentNode = &m_exploredNodes.back();
            corner2.position = collisionImpact.corner2_1;
            corner2.nodesToUnlock.push_back(collisionImpact.corner2_2);
            corner2.nodesToUnlockAfter.push_back(collisionImpact.corner1_2);
            //corner2.nodesToUnlock.push_back(m_destination);
            this->addNodeToExplore(nodeIt->first+this->estimateNodeWeight(corner2), corner2);
            //m_nodesToExplore.insert({nodeIt->first+this->estimateNodeWeight(corner2),
            //                        corner2});
        } else {
            this->unlockNodesToExplore(nodeIt->first, node, &m_exploredNodes.back());
        }

        //if(destination == m_destination)
        if(glm::dot(destination - m_destination, destination - m_destination) < m_destinationRadius*m_destinationRadius)
        {
            foundDestination = true;
            break;
        }

        ///Test
        }
        ///

        m_nodesToExplore.erase(nodeIt);
        nodeIt = m_nodesToExplore.begin();
    }

    //If we did not find the destination, we take the closest solution
    if(!foundDestination)
        m_exploredNodes.push_back(*closestNode);
    /*else
        std::cout<<"FOUND"<<std::endl;
    std::cout<<m_exploredNodes.size()<<std::endl;*/
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



