#include "sync/NodeSyncer.h"

#include "logic/GameMessageTypes.h"
#include "world/GameWorld_Sync.h"

const glm::vec3 NodeSyncer::NODE_MAX_POS         = glm::vec3(50000.0, 50000.0, 1000.0);
const float     NodeSyncer::NODE_MAX_SCALE       = 100.0f;
const uint8_t   NodeSyncer::NODE_SCALE_DECIMALS  = 2;

//NodeSyncer::NodeSyncer(std::shared_ptr<pou::SceneNode> node) :
NodeSyncer::NodeSyncer(pou::SceneNode* node) :
    m_node(node),
    /*m_syncPosition(node->transform()->getPosition()),
    m_syncRotations(node->transform()->getEulerRotation()),
    m_syncScale(node->transform()->getScale()),
    m_syncColor(node->getColor()),*/
    m_nodeSyncId(0),
    m_parentNodeSyncId(0),
    m_lastParentUpdateTime(-1)
    //m_gameWorldSync(nullptr)
{
    if(node)
    {
        this->startListeningTo(node/*.get()*/);
        m_syncPosition.setValue(node->transform()->getPosition());
        m_syncRotations.setValue(node->transform()->getEulerRotation());
        m_syncScale.setValue(node->transform()->getScale());
        m_syncColor.setValue(node->getColor());
        m_nodeSyncId = node->getNodeId();
        if(node->getParentNode())
            m_parentNodeSyncId = node->getParentNode()->getNodeId();
    }

    m_syncPosition.setMinMaxAndPrecision(-NodeSyncer::NODE_MAX_POS, NodeSyncer::NODE_MAX_POS, glm::uvec3(0,0,2));
    m_nodeSyncComponent.addSyncElement(&m_syncPosition);

    m_syncRotations.setMinMaxAndPrecision(glm::vec3(-3.15), glm::vec3(3.15), glm::uvec3(2));
    m_nodeSyncComponent.addSyncElement(&m_syncRotations);

    m_syncScale.setMinMaxAndPrecision(glm::vec3(-NodeSyncer::NODE_MAX_SCALE),
                                  glm::vec3(NodeSyncer::NODE_MAX_SCALE),
                                  glm::uvec3(NodeSyncer::NODE_SCALE_DECIMALS));
    m_nodeSyncComponent.addSyncElement(&m_syncScale);

    m_syncColor.setMinMaxAndPrecision(glm::vec4(0), glm::vec4(10,10,10,1), glm::uvec4(2));
    m_nodeSyncComponent.addSyncElement(&m_syncColor);
}

NodeSyncer::~NodeSyncer()
{
    //dtor
}

void NodeSyncer::syncFrom(NodeSyncer* srcNodeSyncer)
{
    m_nodeSyncComponent.syncFrom(srcNodeSyncer->m_nodeSyncComponent);
}

void NodeSyncer::setReconciliationDelay(uint32_t serverDelay, uint32_t clientDelay)
{
    m_nodeSyncComponent.setReconciliationDelay(serverDelay, clientDelay);
}

void NodeSyncer::setMaxRewind(int maxRewind)
{
    m_nodeSyncComponent.setMaxRewindAmount(maxRewind);
}

/**void NodeSyncer::setSyncReconciliationPrecision(glm::vec3 positionPrecision)
{
    m_syncPosition.setReconciliationPrecision(positionPrecision);
}**/

void NodeSyncer::disableRotationSync(bool disable)
{
    m_syncRotations.disableSync(disable);
}

void NodeSyncer::disableSync(bool disable)
{
    m_nodeSyncComponent.disableSync(disable);
}

uint32_t NodeSyncer::getLastUpdateTime()
{
    return m_nodeSyncComponent.getLastUpdateTime();
}

uint32_t NodeSyncer::getLastParentUpdateTime()
{
    return m_lastParentUpdateTime;
}

uint32_t NodeSyncer::getNodeSyncId()
{
    return m_nodeSyncId;
}

uint32_t NodeSyncer::getParentNodeSyncId()
{
    return m_parentNodeSyncId;
}

void NodeSyncer::update(const pou::Time &elapsedTime, uint32_t localTime)
{
    if(m_nodeSyncComponent.update(elapsedTime, localTime) && m_node)
    {
        m_node->transform()->setPosition(m_syncPosition.getValue());
        m_node->transform()->setRotationInRadians(m_syncRotations.getValue());
        m_node->transform()->setScale(m_syncScale.getValue());
        m_node->setColor(m_syncColor.getValue());
    }

    GameMessage_World_NodeSyncerUpdated msg;
    msg.nodeSyncer = this;
    pou::MessageBus::postMessage(GameMessageType_World_NodeSyncerUpdated, &msg);
}

void NodeSyncer::serialize(pou::Stream *stream, uint32_t clientTime)
{
    int nodeSyncId = m_nodeSyncId;
    stream->serializeBits(nodeSyncId, GameWorld_Sync::NODEID_BITS);
    m_nodeSyncId = (uint32_t)nodeSyncId;

    bool newParent = false;
    if(!stream->isReading() && uint32less(clientTime,this->getLastParentUpdateTime()))
        newParent = true;
    stream->serializeBool(newParent);
    if(newParent)
    {
        int parentNodeId = m_parentNodeSyncId;
        stream->serializeBits(parentNodeId, GameWorld_Sync::NODEID_BITS);
        m_parentNodeSyncId = (uint32_t)parentNodeId;
    }

    m_nodeSyncComponent.serialize(stream, clientTime);
}

//std::shared_ptr<pou::SceneNode> NodeSyncer::node()
pou::SceneNode* NodeSyncer::node()
{
    return m_node;
}


///
///Protected
///

void NodeSyncer::notify(pou::NotificationSender *sender, int notificationType, void* data)
{
    if(sender != m_node/*.get()*/)
        return;

    if(notificationType == pou::NotificationType_NodeMoved)
    {
        m_syncPosition.setValue(m_node->transform()->getPosition());
        m_syncRotations.setValue(m_node->transform()->getEulerRotation());
        m_syncScale.setValue(m_node->transform()->getScale());
        m_syncColor.setValue(m_node->getColor());
    }

    if(notificationType == pou::NotificationType_NodeParentChanged)
    {
        m_nodeSyncComponent.updateLastUpdateTime();
        m_lastParentUpdateTime = m_nodeSyncComponent.getLocalTime();


        auto parentNode = m_node->getParentNode();
        if(parentNode)
            m_parentNodeSyncId = parentNode->getNodeId();
        else
            m_parentNodeSyncId = 0;
    }
}

void NodeSyncer::setNodeSyncId(uint32_t id)
{
    m_nodeSyncId = id;
}



