#ifndef SIMPLENODE_H
#define SIMPLENODE_H

#include <glm/glm.hpp>
#include <map>

#include "PouEngine/core/NotificationListener.h"
#include "PouEngine/core/NotificationSender.h"
#include "PouEngine/utils/Stream.h"
#include "PouEngine/utils/SyncedAttribute.h"

namespace pou
{

class SimpleNode : public NotificationSender, public NotificationListener
{
    public:
        SimpleNode(const NodeTypeId);
        virtual ~SimpleNode();

        void addChildNode(SimpleNode*);
        void addChildNode(const NodeTypeId id, SimpleNode*);

        void moveChildNode(SimpleNode* node, SimpleNode* target);
        void moveChildNode(const NodeTypeId id, SimpleNode* target);

        virtual SimpleNode* removeChildNode(SimpleNode*);
        virtual SimpleNode* removeChildNode(const NodeTypeId id);

        SimpleNode* createChildNode();
        SimpleNode* createChildNode(float, float );
        SimpleNode* createChildNode(float, float, float );
        SimpleNode* createChildNode(glm::vec2 );
        SimpleNode* createChildNode(glm::vec3 );
        SimpleNode* createChildNode(const NodeTypeId id);

        bool destroyChildNode(SimpleNode*);
        bool destroyChildNode(const NodeTypeId id);

        void removeAndDestroyAllChilds(bool destroyNonCreatedChilds = false);

        virtual void copyFrom(const SimpleNode* srcNode);
        virtual void syncFromNode(SimpleNode* srcNode);

        void move(float, float);
        void move(float, float, float);
        void move(glm::vec2 );
        void move(glm::vec3 );
        void setPosition(float, float);
        void setPosition(float, float, float);
        void setPosition(glm::vec2 );
        void setPosition(glm::vec3 );
        void setGlobalPosition(float, float);
        void setGlobalPosition(float, float, float);
        void setGlobalPosition(glm::vec2 );
        void setGlobalPosition(glm::vec3 );
        void setName(const std::string &name);
        void setRigidity(float rigidity);

        //void setLocalTime(uint32_t localTime);
        virtual void setReconciliationDelay(uint32_t serverDelay, uint32_t clientDelay = -1);
        virtual void setMaxRewind(int maxRewind);
        void disableRotationSync(bool disable = true);
        void disableSync(bool disable = true);

        void scale(float scale);
        void scale(glm::vec3 scale);
        void linearScale(float, float, float);
        void linearScale(glm::vec3 scale);
        void setScale(float scale);
        void setScale(glm::vec3 scale);
        void rotate(float value, glm::vec3 axis, bool inRadians = true);
        void rotate(glm::vec3 values, bool inRadians = true);
        void setRotation(glm::vec3 rotation, bool inRadians = true);

        glm::vec3 getPosition() const;
        glm::vec2 getXYPosition() const;
        glm::vec3 getGlobalPosition() const;
        glm::vec2 getGlobalXYPosition() const;
        glm::vec3 getScale() const;
        glm::vec3 getEulerRotation()const;
        const glm::mat4 &getModelMatrix() const;
        const glm::mat4 &getInvModelMatrix() const;

        NodeTypeId getId()  const;
        const std::string &getName()  const;
        float getRigidity() const;
        SimpleNode* getParent();
        SimpleNode* getChild(const NodeTypeId id);
        SimpleNode* getChildByName(const std::string &name, bool recursiveSearch = true);
        void getNodesByName(std::map<std::string, SimpleNode*> &namesAndResMap);
        //std::list<SimpleNode*> getAllChilds();

        uint32_t getLastUpdateTime();
        uint32_t getLastParentUpdateTime();
        uint32_t getLocalTime();

        virtual void update(const Time &elapsedTime = Time(0), uint32_t localTime = -1);
        ///virtual void rewind(uint32_t time);

        virtual void notify(NotificationSender* , int notificationType,
                            void* data = nullptr) override;


        virtual void serializeNode(Stream *stream, uint32_t localTime = -1);


    protected:
        virtual SimpleNode* nodeAllocator(NodeTypeId);

        virtual void setParent(SimpleNode *);
        void setId(const NodeTypeId );
        NodeTypeId generateId();

        void addCreatedChildNode(SimpleNode*);

        void askForUpdateModelMatrix();
        virtual void updateGlobalPosition();
        void updateModelMatrix();

        void computeFlexibleMove(glm::vec3 );
        //void computeFlexibleRotate(float );

        void setLastUpdateTime(uint32_t time, bool force = false);

    protected:
        glm::vec3 m_globalPosition;

        //glm::vec3 m_position;
        //glm::vec3 m_eulerRotations;
        //glm::vec3 m_scale;
        LinSyncedAttribute<glm::vec3> m_position;
        LinSyncedAttribute<glm::vec3> m_eulerRotations;
        LinSyncedAttribute<glm::vec3> m_scale;

        float     m_rigidity;

        float m_curFlexibleLength;
        float m_curFlexibleRotation;

        glm::mat4 m_modelMatrix;
        glm::mat4 m_invModelMatrix;

        SimpleNode *m_parent;
        std::map<NodeTypeId, SimpleNode*> m_childs;

        uint32_t m_curLocalTime;
        uint32_t m_lastSyncTime;
        uint32_t m_lastUpdateTime;
        uint32_t m_lastParentUpdateTime;
        //float m_lastPositionUpdateTime;
        //float m_lastRotationUpdateTime;
        //float m_lastScaleUpdateTime;

        bool m_disableRotationSync;
        bool m_disableSync;

    private:
        NodeTypeId m_id;
        std::string m_name;
        std::set<NodeTypeId> m_createdChildsList;

        int m_curNewId;

        bool m_needToUpdateModelMat;

    public:
        static const glm::vec3  NODE_MAX_POS;
        static const float      NODE_MAX_SCALE;
        static const uint8_t    NODE_SCALE_DECIMALS;
};

}

#endif // SIMPLENODE_H
