#include "world/PrefabInstance.h"

#include "assets/PrefabAsset.h"
#include "world/GameWorld_Sync.h"

PrefabInstance::PrefabInstance() :
    m_worldSync(nullptr),
    m_prefabSyncId(0),
    m_lastPrefabModelUpdate(-1),
    m_prefabModel(nullptr)
{
    //ctor
}

PrefabInstance::~PrefabInstance()
{
    //dtor
}

/*void PrefabInstance::syncFromPrefab(PrefabInstance* srcPrefab)
{

}*/

void PrefabInstance::createFromModel(PrefabAsset *prefabModel)
{
    prefabModel->generateOnNode(this);
}

void PrefabInstance::update(const pou::Time &elapsedTime, uint32_t localTime)
{
    WorldNode::update(elapsedTime, localTime);

    m_prefabSyncComponent.update(elapsedTime, localTime);
}

PrefabAsset *PrefabInstance::getPrefabModel()
{
    return m_prefabModel;
}

uint32_t PrefabInstance::getLastPrefabModelUpdateTime()
{
    return m_lastPrefabModelUpdate;
}

uint32_t PrefabInstance::getLastPrefabUpdateTime()
{
    return m_prefabSyncComponent.getLastUpdateTime();
}

uint32_t PrefabInstance::getPrefabSyncId()
{
    return m_prefabSyncId;
}

///
/// Protected
///

void PrefabInstance::setPrefabModel(PrefabAsset *prefabModel)
{
    if(m_prefabModel != prefabModel)
    {
        m_prefabModel = prefabModel;
        m_prefabSyncComponent.updateLastUpdateTime();
        m_lastPrefabModelUpdate = m_prefabSyncComponent.getLastUpdateTime();
    }
}

void PrefabInstance::setSyncData(GameWorld_Sync *worldSync, int id)
{
    m_worldSync = worldSync;
    m_prefabSyncId = id;
}

