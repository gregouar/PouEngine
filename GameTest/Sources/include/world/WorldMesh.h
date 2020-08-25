#ifndef WORLDMESH_H
#define WORLDMESH_H

#include "PouEngine/scene/MeshEntity.h"
#include "PouEngine/sync/SyncElements.h"

///Could change this to MeshSyncer if needed later

class WorldMesh : public pou::MeshEntity
{
    friend class GameWorld_Sync;

    public:
        WorldMesh();
        virtual ~WorldMesh();

        virtual void update(const pou::Time &elapsedTime, uint32_t localTime = -1);
        virtual bool syncFrom(WorldMesh* srcSprite);

        virtual bool setMeshModel(pou::MeshAsset* model);

        uint32_t getSyncId();
        uint32_t getLastUpdateTime();
        uint32_t getLastModelUpdateTime();
        uint32_t getLastNodeUpdateTime();

    protected:
        virtual pou::SceneNode *setParentNode(pou::SceneNode*);

        void setSyncId(uint32_t id);

    private:
        uint32_t m_syncId;
        uint32_t m_lastModelUpdateTime;
        uint32_t m_lastNodeUpdateTime;

        pou::SyncComponent m_syncComponent;
};

#endif // WORLDMESH_H
