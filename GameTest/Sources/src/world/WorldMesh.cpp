#include "world/WorldMesh.h"

#include "logic/GameMessageTypes.h"

WorldMesh::WorldMesh() : pou::MeshEntity(),
    m_syncId(0),
    m_lastModelUpdateTime(-1),
    m_lastNodeUpdateTime(-1)
{
    //ctor
}

WorldMesh::~WorldMesh()
{
    //dtor
}


void WorldMesh::update(const pou::Time &elapsedTime, uint32_t localTime)
{
    m_syncComponent.update(elapsedTime, localTime);

    pou::MeshEntity::update(elapsedTime, localTime);

    GameMessage_World_MeshUpdated msg;
    msg.mesh = this;
    pou::MessageBus::postMessage(GameMessageType_World_MeshUpdated, &msg);
}

bool WorldMesh::syncFrom(WorldMesh* srcSprite)
{
    return (true);
}

bool WorldMesh::setMeshModel(pou::MeshAsset* model)
{
    if(pou::MeshEntity::setMeshModel(model))
    {
        m_syncComponent.updateLastUpdateTime();
        m_lastModelUpdateTime = m_syncComponent.getLastUpdateTime();
        return (true);
    }
    return (false);
}

uint32_t WorldMesh::getSyncId()
{
    return m_syncId;
}

uint32_t WorldMesh::getLastUpdateTime()
{
    return m_syncComponent.getLastUpdateTime();
}

uint32_t WorldMesh::getLastModelUpdateTime()
{
    return m_lastModelUpdateTime;
}

uint32_t WorldMesh::getLastNodeUpdateTime()
{
    return m_lastNodeUpdateTime;
}

///
///Protected
///

pou::SceneNode *WorldMesh::setParentNode(pou::SceneNode* parentNode)
{
    auto oldParent = pou::MeshEntity::setParentNode(parentNode);

    if(oldParent != parentNode)
    {
        m_syncComponent.updateLastUpdateTime();
        m_lastNodeUpdateTime = m_syncComponent.getLastUpdateTime();
    }

    return oldParent;
}

void WorldMesh::setSyncId(uint32_t id)
{
    m_syncId = id;
}

