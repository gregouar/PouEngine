#include "PouEngine/scene/SceneGrid.h"

namespace pou
{

SceneGrid::SceneGrid() :
    SceneNode(),
    m_quadSize(1),
    m_minPos(0),
    m_gridSize(0)
{
    //ctor
}

SceneGrid::~SceneGrid()
{
}


void SceneGrid::addChildNode(std::shared_ptr<SceneNode> childNode)
{
    ///This allows to force compute current global position
    childNode->update();

    this->addChildNodeToGrid(childNode);

    this->setAsParentTo(this, childNode.get());
    this->startListeningTo(childNode.get());
}

void SceneGrid::addChildNodeToGrid(std::shared_ptr<SceneNode> childNode)
{
    auto childPos = childNode->transform()->getGlobalXYPosition();
    this->enlargeForPosition(childPos);

    auto gridPos = this->getGridPos(childPos);

    if(this->containsChildNode(childNode.get(),gridPos))
        return;

    m_grid[gridPos.y][gridPos.x].push_back(childNode);
}

bool SceneGrid::removeChildNode(SceneNode *childNode)
{
    auto childPos = childNode->transform()->getGlobalXYPosition();
    auto gridPos = this->getGridPos(childPos);
    return this->removeChildNode(childNode, gridPos);
}

bool SceneGrid::removeChildNode(SceneNode *childNode, glm::ivec2 gridPos)
{
    if(gridPos.x < 0 || gridPos.y < 0 || gridPos.x >= m_gridSize.x || gridPos.y >= m_gridSize.y)
        return (false);

    for(auto childIt = m_grid[gridPos.y][gridPos.x].begin() ;
        childIt != m_grid[gridPos.y][gridPos.x].end() ; ++childIt)
    {
        if(childIt->get() == childNode)
        {
            this->setAsParentTo(nullptr, childNode);
            this->stopListeningTo(childNode);
            m_grid[gridPos.y][gridPos.x].erase(childIt);
            return (true);
        }
    }
    return (false);
}

void SceneGrid::removeAllChilds()
{
    m_grid.clear();
    m_gridSize = glm::vec2(0);
}

bool SceneGrid::containsChildNode(SceneNode *childNode, glm::ivec2 gridPos)
{
    if(gridPos.x < 0 || gridPos.y < 0 || gridPos.x >= m_gridSize.x || gridPos.y >= m_gridSize.y)
        return (false);

    for(auto childIt = m_grid[gridPos.y][gridPos.x].begin() ;
        childIt != m_grid[gridPos.y][gridPos.x].end() ; ++childIt)
    {
        if(childIt->get() == childNode)
            return (true);
    }
    return (false);
}

std::shared_ptr<SceneNode> SceneGrid::extractChildNode(SceneNode *childNode)
{
    auto gridPos = this->getGridPos(childNode->transform()->getGlobalXYPosition());
    return this->extractChildNode(childNode, gridPos);
}

std::shared_ptr<SceneNode> SceneGrid::extractChildNode(SceneNode *childNode, glm::ivec2 gridPos)
{
    if(gridPos.x < 0 || gridPos.y < 0 || gridPos.x >= m_gridSize.x || gridPos.y >= m_gridSize.y)
        return (nullptr);

    for(auto childIt = m_grid[gridPos.y][gridPos.x].begin() ;
        childIt != m_grid[gridPos.y][gridPos.x].end() ; ++childIt)
    {
        if(childIt->get() == childNode)
        {
            std::shared_ptr<SceneNode> rNode;
            rNode = *childIt;
            this->setAsParentTo(nullptr, rNode.get());
            //rNode->setParent(nullptr);
            this->stopListeningTo(rNode.get());
            m_grid[gridPos.y][gridPos.x].erase(childIt);
            return (rNode);
        }
    }
    return (nullptr);
}

void SceneGrid::moveChildNode(SceneNode *childNode, glm::ivec2 oldGridPos)
{
    if(oldGridPos.x < 0 || oldGridPos.y < 0 || oldGridPos.x >= m_gridSize.x || oldGridPos.y >= m_gridSize.y)
        return;

    std::shared_ptr<SceneNode> rNode;

    for(auto childIt = m_grid[oldGridPos.y][oldGridPos.x].begin() ;
        childIt != m_grid[oldGridPos.y][oldGridPos.x].end() ; ++childIt)
    {
        if(childIt->get() == childNode)
        {
            rNode = *childIt;
            m_grid[oldGridPos.y][oldGridPos.x].erase(childIt);
            break;
        }
    }

    if(rNode)
        this->addChildNodeToGrid(rNode);
}

void SceneGrid::setQuadSize(float s)
{
    if(s <= 0)
        return;

    m_quadSize = s;
}

void SceneGrid::resizeQuad(glm::ivec2 minPos, glm::ivec2 gridSize)
{
    std::vector< std::vector< std::vector< std::shared_ptr<SceneNode> > > >
            newGrid;

    newGrid.resize(gridSize.y);
    int y = minPos.y;
    for(auto it = newGrid.begin() ; it != newGrid.end() ; ++it, ++y)
    {
        int relY = y - minPos.y;
        if(relY >= 0 && relY < m_gridSize.y)
        {
            int relX = std::min(0, minPos.x - m_minPos.x);
            int newX = std::max(0, m_minPos.x - minPos.x);
            it->insert(it->begin()+newX,m_grid[relY].begin()+relX, m_grid[relY].end());
        }

        it->resize(gridSize.x);
    }

    m_grid.swap(newGrid);

    m_minPos = minPos;
    m_gridSize = gridSize;
}

void SceneGrid::enlargeForPosition(glm::vec2 pos)
{
    auto minPos = m_minPos;
    auto gridSize = m_gridSize;
    bool enlarge = false;

    auto requiredGridPos = this->getGridPos(pos);

    if(requiredGridPos.x < 0)
    {
        minPos.x += requiredGridPos.x;
        enlarge = true;
    }

    if(requiredGridPos.y < 0)
    {
        minPos.y += requiredGridPos.x;
        enlarge = true;
    }

    if(requiredGridPos.x >= m_gridSize.x)
    {
        m_gridSize.x = requiredGridPos.x + 1;
        enlarge = true;
    }

    if(requiredGridPos.y >= m_gridSize.y)
    {
        m_gridSize.y = requiredGridPos.y + 1;
        enlarge = true;
    }

    if(enlarge)
        this->resizeQuad(minPos, gridSize);
}

void SceneGrid::addUpdateProbe(SceneNode *node, float radius)
{
    if(!node)
        return;

    GridProbe probe;
    probe.node = node;
    probe.radius = radius;
    m_updateProbes.push_back(probe);

    this->startListeningTo(node);
}

void SceneGrid::removeUpdateProbe(SceneNode *node)
{
    for(auto it = m_updateProbes.begin() ; it != m_updateProbes.end() ; ++it)
        if(it->node == node)
        {
            m_updateProbes.erase(it);
            return;
        }
}

void SceneGrid::setRenderProbe(SceneNode *node, float radius)
{
    m_renderProbe.node      = node;
    m_renderProbe.radius    = radius;
    if(node)
        this->startListeningTo(node);
}

void SceneGrid::probesZones(std::unordered_set< std::vector<std::shared_ptr<SceneNode> > *> &zonesToUpdate, GridProbe &probe)
{
    auto probeGridPos_Min =  this->getGridPos(probe.node->transform()->getGlobalXYPosition()-probe.radius*glm::vec2(1));
    auto probeGridPos_Max =  this->getGridPos(probe.node->transform()->getGlobalXYPosition()+probe.radius*glm::vec2(1));

    for(auto y = probeGridPos_Min.y ; y <= probeGridPos_Max.y ; ++y)
    for(auto x = probeGridPos_Min.x ; x <= probeGridPos_Max.x; ++x)
    if(y >= 0 && x >= 0
    && y < m_gridSize.y && x < m_gridSize.x)
        zonesToUpdate.insert(&m_grid[y][x]);

    /*auto probeGridPos = this->getGridPos(probe.node->transform()->getGlobalXYPosition()-probe.radius*glm::vec2(1));
    auto probeGridSize = glm::ceil(probe.radius*2/m_quadSize);

    for(auto y = 0 ; y < (int)probeGridSize ; ++y)
    for(auto x = 0 ; x < (int)probeGridSize ; ++x)
    if(probeGridPos.y + y >= 0 && probeGridPos.x + x >= 0
    && probeGridPos.y + y < m_gridSize.y && probeGridPos.x + x < m_gridSize.x)
        zonesToUpdate.insert(&m_grid[probeGridPos.y + y][probeGridPos.x + x]);*/
}

void SceneGrid::update(const pou::Time &elapsedTime, uint32_t localTime)
{
    SceneNode::update(elapsedTime, localTime);

    std::unordered_set< std::vector<std::shared_ptr<SceneNode> > *> zonesToUpdate;

    for(auto &probe : m_updateProbes)
        this->probesZones(zonesToUpdate, probe);

    int nbrUpdatedNodes = 0;

       size_t i = 0;
    for(auto* zone : zonesToUpdate)
    for(auto node : *zone)
    {
        node->update(elapsedTime, localTime), ++i;
       nbrUpdatedNodes++;
    }

    for(auto &nodeToMove : m_nodesToMove)
        this->moveChildNode(nodeToMove.second, nodeToMove.first);

    m_nodesToMove.clear();
}

void SceneGrid::generateRenderingData(pou::SceneRenderingInstance *renderingInstance)
{
    SceneNode::generateRenderingDataWithoutPropagating(renderingInstance);

    if(!m_renderProbe.node)
        return;

    std::unordered_set< std::vector<std::shared_ptr<SceneNode> > *> zonesToUpdate;
    this->probesZones(zonesToUpdate, m_renderProbe);

    for(auto* zone : zonesToUpdate)
    for(auto node : *zone)
        node->generateRenderingData(renderingInstance);
}

glm::ivec2 SceneGrid::getGridPos(glm::vec2 worldPos)
{
    return glm::floor((worldPos - glm::vec2(m_minPos) * m_quadSize)/m_quadSize);
}

void SceneGrid::notify(pou::NotificationSender* sender, int notificationType, void* data)
{
    SceneNode::notify(sender, notificationType, data);

    if(notificationType == pou::NotificationType_NodeMoved && data != nullptr)
    {
        auto node = static_cast<SceneNode*>(sender);
        auto oldPos = *((glm::vec3*)data);

        auto oldGridPos = this->getGridPos(glm::vec2(oldPos));
        auto newGridPos = this->getGridPos(node->transform()->getGlobalXYPosition());

        if(oldGridPos != newGridPos)
        if(this->containsChildNode(node, oldGridPos))
            m_nodesToMove.push_back({oldGridPos, node});
    }

    if(notificationType == pou::NotificationType_SenderDestroyed)
    {
        this->removeUpdateProbe((SceneNode*)sender);
        if(sender == m_renderProbe.node)
            m_renderProbe.node = nullptr;
    }
}

}


