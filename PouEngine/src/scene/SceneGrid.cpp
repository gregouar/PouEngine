#include "PouEngine/scene/SceneGrid.h"

namespace pou
{

SceneGrid::SceneGrid() :
    m_quadSize(1),
    m_minPos(0),
    m_gridSize(0)
{
    //ctor
}

SceneGrid::~SceneGrid()
{
    //dtor
}


void SceneGrid::addChildNode(std::shared_ptr<SimpleNode> childNode)
{
    ///SceneNode::addChildNode(childNode); //Don't add it !

    auto childPos = childNode->getGlobalXYPosition();
    this->enlargeForPosition(childPos);

    auto gridPos = this->getGridPos(childPos);
    m_grid[gridPos.y][gridPos.x].push_back(childNode);
    childNode->setParent(this);
    this->startListeningTo(childNode.get());
}

bool SceneGrid::removeChildNode(SimpleNode *childNode)
{
    auto childPos = childNode->getGlobalXYPosition();
    auto gridPos = this->getGridPos(childPos);
    for(auto childIt = m_grid[gridPos.y][gridPos.x].begin() ;
        childIt != m_grid[gridPos.y][gridPos.x].end() ; ++childIt)
    {
        if(childIt->get() == childNode)
        {
            childNode->setParent(nullptr);
            m_grid[gridPos.y][gridPos.x].erase(childIt);
            return (true);
        }
    }
    return (false);
}

void SceneGrid::removeAllChilds()
{
    /* do something */
}

void SceneGrid::setQuadSize(float s)
{
    if(s <= 0)
        return;

    m_quadSize = s;
}

void SceneGrid::resizeQuad(glm::ivec2 minPos, glm::ivec2 gridSize)
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

    /*int wantedMaxPosX = minPos.x + gridSize.x;
    int wantedMaxPosY = minPos.y + gridSize.y;

    if(minPos.x < m_minPos.x)
    {
        int delta = m_minPos.x + m_gridSize.x - wantedMaxPosX;
        if(delta > 0)
            gridSize.x += delta;
        m_minPos.x = minPos.x;
    }

    if(minPos.y < m_minPos.y)
    {
        int delta = m_minPos.y + m_gridSize.y - wantedMaxPosY;
        if(delta > 0)
            gridSize.y += delta;
        m_minPos.y = minPos.y;
    }

    m_quad.resuze(gridSize.y);
    for(auto &it : m_quad)
        m_quad.reserve(gridSize.x);

    if(m_gridSize.x < nbrX)
        m_gridSize.x = nbrX;
    if(m_gridSize.y < nbrY)
        m_gridSize.y = nbrY;*/
}

void SceneGrid::enlargeForPosition(glm::vec2 pos)
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

void SceneGrid::addUpdateProbe(std::shared_ptr<SimpleNode> node, float radius)
{
    GridProbe probe;
    probe.node = node;
    probe.radius = radius;
    m_updateProbes.push_back(probe);
}

void SceneGrid::removeUpdateProbe(std::shared_ptr<SimpleNode> node)
{
    for(auto it = m_updateProbes.begin() ; it != m_updateProbes.end() ; ++it)
        if(it->node == node)
        {
            m_updateProbes.erase(it);
            return;
        }
}

void SceneGrid::setRenderProbe(std::shared_ptr<SimpleNode> node, float radius)
{
    m_renderProbe.node      = node;
    m_renderProbe.radius    = radius;
}

void SceneGrid::update(const Time &elapsedTime, uint32_t localTime)
{
    SceneNode::update(elapsedTime, localTime);

    for(auto &probe : m_updateProbes)
    {
    }
}

void SceneGrid::generateRenderingData(SceneRenderingInstance *renderingInstance, bool propagateToChilds)
{
    SceneNode::generateRenderingData(renderingInstance, false);


}

glm::ivec2 SceneGrid::getGridPos(glm::vec2 worldPos)
{
    return glm::floor((worldPos - glm::vec2(m_minPos) * m_quadSize)/m_quadSize);
}

void SceneGrid::notify(NotificationSender* sender, int notificationType, void* data)
{
    SceneNode::notify(sender, notificationType, data);
}

}
