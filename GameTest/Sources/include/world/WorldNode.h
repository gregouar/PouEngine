#ifndef WORLDNODE_H
#define WORLDNODE_H

#include "PouEngine/scene/SceneNode.h"
#include "PouEngine/sync/SyncElements.h"

class WorldNode : public pou::SceneNode
{
    public:
        WorldNode();
        virtual ~WorldNode();

        virtual void syncFrom(WorldNode* srcNode);

        std::shared_ptr<WorldNode> createChildNode();
        std::shared_ptr<WorldNode> createChildNode(float, float );
        std::shared_ptr<WorldNode> createChildNode(float, float, float );
        std::shared_ptr<WorldNode> createChildNode(glm::vec2 );
        std::shared_ptr<WorldNode> createChildNode(glm::vec3 );

        virtual void setPosition(glm::vec3 );
        virtual void setScale(glm::vec3 scale);
        virtual void setRotation(glm::vec3 rotation, bool inRadians = true);
        virtual void setColor(const glm::vec4 &c);

        virtual const glm::vec3 &getPosition() const;
        virtual const glm::vec3 &getScale() const;
        virtual const glm::vec3 &getEulerRotation()const;
        virtual const glm::vec4 &getColor() const;

        virtual void setReconciliationDelay(uint32_t serverDelay, uint32_t clientDelay = -1);
        virtual void setMaxRewind(int maxRewind);
        ///void setSyncReconciliationPrecision(glm::vec3 positionPrecision);
        void disableRotationSync(bool disable = true);
        void disableSync(bool disable = true);

        uint32_t getLastUpdateTime();
        uint32_t getLastParentUpdateTime();

        virtual void update(const pou::Time &elapsedTime = pou::Time(0), uint32_t localTime = -1);

        virtual void serialize(pou::Stream *stream, uint32_t localTime = -1);

    protected:
        virtual std::shared_ptr<pou::SimpleNode> nodeAllocator();

        virtual bool setParent(SimpleNode *parentNode);

    protected:
        pou::Vec3LinSyncElement m_syncPosition;
        pou::Vec3LinSyncElement m_syncRotations;
        pou::Vec3LinSyncElement m_syncScale;
        pou::Vec4LinSyncElement m_syncColor;

    private:
        pou::SyncComponent m_syncComponent;
        uint32_t m_lastParentUpdateTime;

    public:
        static const glm::vec3  NODE_MAX_POS;
        static const float      NODE_MAX_SCALE;
        static const uint8_t    NODE_SCALE_DECIMALS;
};

#endif // WORLDNODE_H
