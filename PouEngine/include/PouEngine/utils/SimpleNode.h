#ifndef SIMPLENODE_H
#define SIMPLENODE_H

#include <glm/glm.hpp>
#include <map>

#include "PouEngine/core/NotificationListener.h"
#include "PouEngine/core/NotificationSender.h"

namespace pou
{

class SimpleNode : public NotificationSender, public NotificationListener
{
    public:
        SimpleNode(const NodeTypeId);
        virtual ~SimpleNode();

        void addChildNode(SimpleNode*);
        void addChildNode(const NodeTypeId id, SimpleNode*);

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

        void move(float, float);
        void move(float, float, float);
        void move(glm::vec2 );
        void move(glm::vec3 );
        void setPosition(float, float);
        void setPosition(float, float, float);
        void setPosition(glm::vec2 );
        void setPosition(glm::vec3 );

        void scale(float scale);
        void scale(glm::vec3 scale);
        void setScale(float scale);
        void setScale(glm::vec3 scale);
        void rotate(float value, glm::vec3 axis);
        void setRotation(glm::vec3 rotation);

        glm::vec3 getPosition();
        glm::vec3 getGlobalPosition();
        glm::vec3 getScale();
        glm::vec3 getEulerRotation();
        const glm::mat4 &getModelMatrix();

        NodeTypeId getId();
        SimpleNode* getParent();
        SimpleNode* getChild(const NodeTypeId id);

        //could be used for animation ?
        void update(const Time &elapsedTime);

        virtual void notify(NotificationSender* , NotificationType,
                            size_t dataSize = 0, char* data = nullptr) override;


    protected:
        virtual void setParent(SimpleNode *);
        void setId(const NodeTypeId );
        NodeTypeId generateId();

        virtual SimpleNode* nodeAllocator(NodeTypeId);

        void updateGlobalPosition();
        void updateModelMatrix();

    protected:
        glm::vec3 m_globalPosition;

        glm::vec3 m_position;
        glm::vec3 m_eulerRotations;
        glm::vec3 m_scale;

        glm::mat4 m_modelMatrix;

        SimpleNode *m_parent;
        std::map<NodeTypeId, SimpleNode*> m_childs;

    private:
        NodeTypeId m_id;
        std::set<NodeTypeId> m_createdChildsList;

        int m_curNewId;
};

}

#endif // SIMPLENODE_H
