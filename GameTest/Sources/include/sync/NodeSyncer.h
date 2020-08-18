#ifndef NODESYNCER_H
#define NODESYNCER_H

#include "PouEngine/scene/SceneNode.h"
#include "PouEngine/sync/SyncElements.h"

class NodeSyncer : public pou::NotificationListener
{
    friend class GameWorld_Sync;

    public:
        //NodeSyncer(std::shared_ptr<pou::SceneNode> node = nullptr);
        NodeSyncer(pou::SceneNode* node = nullptr);
        virtual ~NodeSyncer();

        virtual void syncFrom(NodeSyncer* srcNodeSyncer);

        virtual void setReconciliationDelay(uint32_t serverDelay, uint32_t clientDelay = -1);
        virtual void setMaxRewind(int maxRewind);
        void disableRotationSync(bool disable = true);
        void disableSync(bool disable = true);

        uint32_t getLastUpdateTime();
        uint32_t getLastParentUpdateTime();
        uint32_t getNodeSyncId();
        uint32_t getParentNodeSyncId();

        virtual void update(const pou::Time &elapsedTime = pou::Time(0), uint32_t localTime = -1);

        virtual void serialize(pou::Stream *stream, uint32_t localTime = -1);

        //std::shared_ptr<pou::SceneNode> node();
        pou::SceneNode* node();
        //void setGameWorldSync(GameWorld_Sync *gameWorldSync);

    protected:
        virtual void notify(pou::NotificationSender *sender, int notificationType, void* data = nullptr) override;

        void setNodeSyncId(uint32_t id);
        void setParentNodeSyncId(uint32_t id);

    private:
        //std::shared_ptr<pou::SceneNode> m_node;
        pou::SceneNode *m_node;

        pou::Vec3LinSyncElement m_syncPosition;
        pou::Vec3LinSyncElement m_syncRotations;
        pou::Vec3LinSyncElement m_syncScale;
        pou::Vec4LinSyncElement m_syncColor;

        uint32_t m_nodeSyncId;

        pou::SyncComponent m_nodeSyncComponent;

        uint32_t m_parentNodeSyncId;
        uint32_t m_lastParentUpdateTime;

        //GameWorld_Sync *m_gameWorldSync;

    public:
        static const glm::vec3  NODE_MAX_POS;
        static const float      NODE_MAX_SCALE;
        static const uint8_t    NODE_SCALE_DECIMALS;
};

#endif // NODESYNCER_H
