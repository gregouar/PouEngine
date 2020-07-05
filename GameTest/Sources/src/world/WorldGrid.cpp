#include "world/WorldGrid.h"


WorldGrid::WorldGrid() :
    WorldNode(),
    m_quadSize(1),
    m_minPos(0),
    m_gridSize(0)
{
    //ctor
}

WorldGrid::~WorldGrid()
{
    this->removeAllChilds();
}


void WorldGrid::addChildNode(std::shared_ptr<pou::SimpleNode> childNode)
{
    ///This allows to compute actual global position
    childNode->update(pou::Time(0));

    auto childPos = childNode->getGlobalXYPosition();
    this->enlargeForPosition(childPos);

    auto gridPos = this->getGridPos(childPos);

    if(this->containsChildNode(childNode.get(),gridPos))
        return;

    //std::cout<<"AddChiuld:"<<gridPos.x<<" "<<gridPos.y<<std::endl;

    m_grid[gridPos.y][gridPos.x].push_back(childNode);
    //childNode->setParent(this);
    this->setAsParentTo(this, childNode.get());
    this->startListeningTo(childNode.get());

    //m_needToUpdateNodes.insert(childNode.get());
}

bool WorldGrid::removeChildNode(pou::SimpleNode *childNode)
{
    auto childPos = childNode->getGlobalXYPosition();
    auto gridPos = this->getGridPos(childPos);
    return this->removeChildNode(childNode, gridPos);
}

bool WorldGrid::removeChildNode(pou::SimpleNode *childNode, glm::ivec2 gridPos)
{
    /*for(auto probeIt = m_updateProbes.begin() ; probeIt != m_updateProbes.end() ; ++probeIt)
        if(probeIt->node.get() == childNode)
            probeIt = m_updateProbes.erase(probeIt);

    if(m_renderProbe.node.get() == childNode)
        m_renderProbe.node.reset();*/

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

void WorldGrid::removeAllChilds()
{
    m_grid.clear();
    m_gridSize = glm::vec2(0);
    /* do something */
}

bool WorldGrid::containsChildNode(pou::SimpleNode *childNode, glm::ivec2 gridPos)
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

std::shared_ptr<pou::SimpleNode> WorldGrid::extractChildNode(pou::SimpleNode *childNode)
{
    auto gridPos = this->getGridPos(childNode->getGlobalXYPosition());
    return this->extractChildNode(childNode, gridPos);
}

std::shared_ptr<pou::SimpleNode> WorldGrid::extractChildNode(pou::SimpleNode *childNode, glm::ivec2 gridPos)
{
    if(gridPos.x < 0 || gridPos.y < 0 || gridPos.x >= m_gridSize.x || gridPos.y >= m_gridSize.y)
        return (nullptr);

    for(auto childIt = m_grid[gridPos.y][gridPos.x].begin() ;
        childIt != m_grid[gridPos.y][gridPos.x].end() ; ++childIt)
    {
        if(childIt->get() == childNode)
        {
            std::shared_ptr<pou::SimpleNode> rNode;
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


void WorldGrid::setQuadSize(float s)
{
    if(s <= 0)
        return;

    m_quadSize = s;
}

void WorldGrid::resizeQuad(glm::ivec2 minPos, glm::ivec2 gridSize)
{
    std::vector< std::vector< std::vector< std::shared_ptr<SimpleNode> > > >
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

void WorldGrid::enlargeForPosition(glm::vec2 pos)
{
    auto minPos = m_minPos;
    auto gridSize = m_gridSize;
    bool enlarge = false;

    if(pos.x < m_minPos.x * m_quadSize)
    {
        minPos.x = glm::floor(pos.x/m_quadSize);
        enlarge = true;
    }

    if(pos.y < m_minPos.y * m_quadSize )
    {
        minPos.y = glm::floor(pos.y/m_quadSize);
        enlarge = true;
    }

    if(pos.x >= m_quadSize * (m_minPos.x + m_gridSize.x))
    {
        gridSize.x = glm::ceil((m_minPos.x * m_quadSize + pos.x)/m_quadSize);
        enlarge = true;
    }

    if(pos.y >= m_quadSize * (m_minPos.y + m_gridSize.y))
    {
        gridSize.y = glm::ceil((m_minPos.y * m_quadSize + pos.y)/m_quadSize);
        enlarge = true;
    }

    if(enlarge)
        this->resizeQuad(minPos, gridSize);
}

//void WorldGrid::addUpdateProbe(std::shared_ptr<pou::SimpleNode> node, float radius)
void WorldGrid::addUpdateProbe(pou::SimpleNode *node, float radius)
{
    if(!node)
        return;

    GridProbe probe;
    probe.node = node;
    probe.radius = radius;
    m_updateProbes.push_back(probe);

    this->startListeningTo(node);
}

void WorldGrid::removeUpdateProbe(pou::SimpleNode *node)
{
    for(auto it = m_updateProbes.begin() ; it != m_updateProbes.end() ; ++it)
        if(it->node == node)
        {
            m_updateProbes.erase(it);
            return;
        }
}

//void WorldGrid::setRenderProbe(std::shared_ptr<pou::SimpleNode> node, float radius)
void WorldGrid::setRenderProbe(pou::SimpleNode *node, float radius)
{
    m_renderProbe.node      = node;
    m_renderProbe.radius    = radius;
    if(node)
        this->startListeningTo(node);
}

void WorldGrid::probesZones(std::set< std::vector<std::shared_ptr<pou::SimpleNode> > *> &zonesToUpdate, GridProbe &probe)
{
    auto probeGridPos = this->getGridPos(probe.node->getGlobalXYPosition()-probe.radius*glm::vec2(1));
    auto probeGridSize = glm::ceil(probe.radius*2/m_quadSize);

    for(auto y = 0 ; y < (int)probeGridSize ; ++y)
    for(auto x = 0 ; x < (int)probeGridSize ; ++x)
    if(probeGridPos.y + y >= 0 && probeGridPos.x + x >= 0
    && probeGridPos.y + y < m_gridSize.y && probeGridPos.x + x < m_gridSize.x)
        zonesToUpdate.insert(&m_grid[probeGridPos.y + y][probeGridPos.x + x]);
}


/*void WorldGrid::probesZones(std::set< std::shared_ptr<SimpleNode> > &nodesToUpdate, GridProbe &probe)
{
    auto probeGridPos = this->getGridPos(probe.node->getGlobalXYPosition()-probe.radius*glm::vec2(1));
    auto probeGridSize = glm::ceil(probe.radius*2/m_quadSize);

    //std::cout<<probeGridPos.x<<" "<<probeGridPos.y<<std::endl;
    //std::cout<<probeGridSize<<std::endl;

    for(auto y = 0 ; y < (int)probeGridSize ; ++y)
    for(auto x = 0 ; x < (int)probeGridSize ; ++x)
    if(probeGridPos.y + y >= 0 && probeGridPos.x + x >= 0
    && probeGridPos.y + y < m_gridSize.y && probeGridPos.x + x < m_gridSize.x)
        nodesToUpdate.insert(m_grid[probeGridPos.y + y][probeGridPos.x + x].begin(),
                             m_grid[probeGridPos.y + y][probeGridPos.x + x].end());
}*/

void WorldGrid::update(const pou::Time &elapsedTime, uint32_t localTime)
{
    WorldNode::update(elapsedTime, localTime);

    std::set< std::vector<std::shared_ptr<pou::SimpleNode> > *> zonesToUpdate;
    //std::set< std::shared_ptr<SimpleNode> > nodesToUpdate;

    for(auto &probe : m_updateProbes)
        this->probesZones(zonesToUpdate, probe);
       // this->probesZones(nodesToUpdate, probe);

    //int nbrUpdatedNodes = 0;

       size_t i = 0;
    for(auto* zone : zonesToUpdate)
    for(auto node : *zone)
    //for(auto node : nodesToUpdate)
    {
        node->update(elapsedTime, localTime), ++i;
       // m_needToUpdateNodes.erase(node.get());
       //nbrUpdatedNodes++;
    }

    //std::cout<<"Nbr Updated nodes:"<<nbrUpdatedNodes<<std::endl;

   // for(auto node : m_needToUpdateNodes)
     //   node->update(elapsedTime, localTime);

    for(auto &nodeToMove : m_nodesToMove)
    {
        auto ptrNode = this->extractChildNode(nodeToMove.second, nodeToMove.first);
        this->addChildNode(ptrNode);
    }

    m_nodesToMove.clear();
}

void WorldGrid::generateRenderingData(pou::SceneRenderingInstance *renderingInstance, bool propagateToChilds)
{
    WorldNode::generateRenderingData(renderingInstance, false);

    if(!propagateToChilds)
        return;

    if(!m_renderProbe.node)
        return;

    std::set< std::vector<std::shared_ptr<pou::SimpleNode> > *> zonesToUpdate;
    //std::set< std::shared_ptr<SimpleNode> > nodesToUpdate;
    this->probesZones(zonesToUpdate, m_renderProbe);
    //this->probesZones(nodesToUpdate, m_renderProbe);

    for(auto* zone : zonesToUpdate)
    for(auto node : *zone)
    //for(auto node : nodesToUpdate)
        std::dynamic_pointer_cast<SceneNode>(node)->generateRenderingData(renderingInstance, propagateToChilds);
}

glm::ivec2 WorldGrid::getGridPos(glm::vec2 worldPos)
{
    return glm::floor((worldPos - glm::vec2(m_minPos) * m_quadSize)/m_quadSize);
}

void WorldGrid::notify(pou::NotificationSender* sender, int notificationType, void* data)
{
    SceneNode::notify(sender, notificationType, data);

    if(notificationType == pou::NotificationType_NodeMoved)
    {
        auto node = static_cast<WorldNode*>(sender);
        auto oldPos = *((glm::vec3*)data);

        auto oldGridPos = this->getGridPos(glm::vec2(oldPos));
        auto newGridPos = this->getGridPos(node->getGlobalXYPosition());

        if(oldGridPos != newGridPos)
        if(this->containsChildNode(node, oldGridPos))
        {
            m_nodesToMove.push_back({oldGridPos, node});
            //auto ptrNode = this->extractChildNode(node, oldGridPos);
            //this->addChildNode(ptrNode);
        }
    }

    if(notificationType == pou::NotificationType_SenderDestroyed)
    {
        this->removeUpdateProbe((pou::SimpleNode*)sender);
        if(sender == m_renderProbe.node)
            m_renderProbe.node = nullptr;
    }
}
