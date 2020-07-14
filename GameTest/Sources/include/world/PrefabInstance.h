#ifndef PREFABINSTANCE_H
#define PREFABINSTANCE_H

#include "world/WorldNode.h"

class PrefabAsset;
class GameWorld_Sync;

class PrefabInstance : public WorldNode
{
    friend class PrefabAsset;
    friend class GameWorld_Sync;

    public:
        PrefabInstance();
        virtual ~PrefabInstance();

        //virtual void syncFromPrefab(PrefabInstance* srcPrefab);

        virtual void update(const pou::Time &elapsedTime = pou::Time(0), uint32_t localTime = -1);

        void createFromModel(PrefabAsset *prefabModel);

        PrefabAsset *getPrefabModel();
        uint32_t getLastPrefabModelUpdateTime();

        uint32_t getLastPrefabUpdateTime();

        uint32_t getPrefabSyncId();


    protected:
        void setPrefabModel(PrefabAsset *prefabModel);
        void setSyncData(GameWorld_Sync *worldSync, int id);

    private:
        GameWorld_Sync *m_worldSync;
        uint32_t m_prefabSyncId;

        uint32_t m_lastPrefabModelUpdate;
        PrefabAsset *m_prefabModel;

        pou::SyncComponent m_prefabSyncComponent;

};

#endif // PREFABINSTANCE_H
