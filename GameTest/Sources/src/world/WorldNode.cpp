#include "world/WorldNode.h"

#include "logic/GameMessageTypes.h"

const glm::vec3 WorldNode::NODE_MAX_POS         = glm::vec3(50000.0, 50000.0, 1000.0);
const float     WorldNode::NODE_MAX_SCALE       = 100.0f;
const uint8_t   WorldNode::NODE_SCALE_DECIMALS  = 2;

WorldNode::WorldNode() :
    pou::SceneNode(nullptr),
    m_syncPosition(m_position),
    m_syncRotations(m_eulerRotations),
    m_syncScale(m_scale),
    m_syncColor(m_color),
    m_nodeSyncId(0),
    m_lastParentUpdateTime(-1)
{
    m_syncPosition.setMinMaxAndPrecision(-WorldNode::NODE_MAX_POS, WorldNode::NODE_MAX_POS, glm::uvec3(0,0,2));
    m_nodeSyncComponent.addSyncElement(&m_syncPosition);

    m_syncRotations.setMinMaxAndPrecision(glm::vec3(-3.15), glm::vec3(3.15), glm::uvec3(2));
    m_nodeSyncComponent.addSyncElement(&m_syncRotations);

    m_syncScale.setMinMaxAndPrecision(glm::vec3(-WorldNode::NODE_MAX_SCALE),
                                  glm::vec3(WorldNode::NODE_MAX_SCALE),
                                  glm::uvec3(WorldNode::NODE_SCALE_DECIMALS));
    m_nodeSyncComponent.addSyncElement(&m_syncScale);

    m_syncColor.setMinMaxAndPrecision(glm::vec4(0), glm::vec4(10,10,10,1), glm::uvec4(2));
    m_nodeSyncComponent.addSyncElement(&m_syncColor);
}

WorldNode::~WorldNode()
{
    //dtor
}

std::shared_ptr<WorldNode> WorldNode::createChildNode()
{
    return std::dynamic_pointer_cast<WorldNode>(SimpleNode::createChildNode());
}

std::shared_ptr<WorldNode> WorldNode::createChildNode(float x, float y)
{
    return std::dynamic_pointer_cast<WorldNode>(SimpleNode::createChildNode(x,y));
}

std::shared_ptr<WorldNode> WorldNode::createChildNode(float x, float y, float z)
{
    return std::dynamic_pointer_cast<WorldNode>(SimpleNode::createChildNode(x,y,z));
}

std::shared_ptr<WorldNode> WorldNode::createChildNode(glm::vec2 p)
{
    return std::dynamic_pointer_cast<WorldNode>(SimpleNode::createChildNode(p));
}

std::shared_ptr<WorldNode> WorldNode::createChildNode(glm::vec3 p)
{
    return std::dynamic_pointer_cast<WorldNode>(SimpleNode::createChildNode(p));
}

void WorldNode::syncFrom(WorldNode* srcNode)
{
    m_nodeSyncComponent.syncFrom(srcNode->m_nodeSyncComponent);
}

void WorldNode::setReconciliationDelay(uint32_t serverDelay, uint32_t clientDelay)
{
    m_nodeSyncComponent.setReconciliationDelay(serverDelay, clientDelay);
}

void WorldNode::setMaxRewind(int maxRewind)
    {
    m_nodeSyncComponent.setMaxRewindAmount(maxRewind);
}

/**void WorldNode::setSyncReconciliationPrecision(glm::vec3 positionPrecision)
{
    m_syncPosition.setReconciliationPrecision(positionPrecision);
}**/

void WorldNode::disableRotationSync(bool disable)
{
    m_syncRotations.disableSync(disable);
}

void WorldNode::disableSync(bool disable)
{
    m_nodeSyncComponent.disableSync(disable);
}

void WorldNode::setPosition(glm::vec3 pos)
{
    SceneNode::setPosition(pos);
    m_syncPosition.setValue(m_position);
}

void WorldNode::setScale(glm::vec3 scale)
{
    SceneNode::setScale(scale);
    m_syncScale.setValue(m_scale);
}

void WorldNode::setRotation(glm::vec3 rotation, bool inRadians)
{
    SceneNode::setRotation(rotation, inRadians);
    m_syncRotations.setValue(m_eulerRotations);
}

void WorldNode::setColor(const glm::vec4 &c)
{
    SceneNode::setColor(c);
    m_syncColor.setValue(m_color);
}

const glm::vec3 &WorldNode::getPosition() const
{
    return m_syncPosition.getValue();
}

const glm::vec3 &WorldNode::getScale() const
{
    return m_syncScale.getValue();
}

const glm::vec3 &WorldNode::getEulerRotation() const
{
    return m_syncRotations.getValue();
}

const glm::vec4 &WorldNode::getColor() const
{
    return m_syncColor.getValue();
}

uint32_t WorldNode::getLastUpdateTime()
{
    return m_nodeSyncComponent.getLastUpdateTime();
}

uint32_t WorldNode::getLastParentUpdateTime()
{
    return m_lastParentUpdateTime;
}

uint32_t WorldNode::getNodeSyncId()
{
    return m_nodeSyncId;
}

void WorldNode::update(const pou::Time &elapsedTime, uint32_t localTime)
{
    if(m_nodeSyncComponent.update(elapsedTime, localTime))
        this->askForUpdateModelMatrix();

    SceneNode::update(elapsedTime, localTime);

    GameMessage_World_NodeUpdated msg;
    msg.node   = this;
    pou::MessageBus::postMessage(GameMessageType_World_NodeUpdated, &msg);
}

void WorldNode::serialize(pou::Stream *stream, uint32_t clientTime)
{
    m_nodeSyncComponent.serialize(stream, clientTime);
}

///Protected

bool WorldNode::setParent(SimpleNode *p)
{
    if(SceneNode::setParent(p))
    {
        m_nodeSyncComponent.updateLastUpdateTime();
        m_lastParentUpdateTime = m_nodeSyncComponent.getLocalTime();
        return (true);
    }
    return (false);
}

void WorldNode::setNodeSyncId(uint32_t id)
{
    m_nodeSyncId = id;
}

std::shared_ptr<pou::SimpleNode> WorldNode::nodeAllocator(/**NodeTypeId id**/)
{
    return std::make_shared<WorldNode>(/**id**/);
}

