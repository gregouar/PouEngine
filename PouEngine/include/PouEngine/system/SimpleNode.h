#ifndef SIMPLENODE_H
#define SIMPLENODE_H

#include <glm/glm.hpp>
#include <map>

#include "PouEngine/core/NotificationListener.h"
#include "PouEngine/core/NotificationSender.h"
#include "PouEngine/system/Stream.h"
//#include "PouEngine/sync/SyncElements.h"

namespace pou
{

class SimpleNode : public NotificationSender, public NotificationListener
{
    friend class SceneGrid;

    public:
        SimpleNode(/**const NodeTypeId**/);
        virtual ~SimpleNode();

        //void destroy();

        virtual void addChildNode(std::shared_ptr<SimpleNode> childNode);

        virtual bool removeChildNode(SimpleNode *childNode);
        bool removeChildNode(std::shared_ptr<SimpleNode> childNode);
        virtual void removeAllChilds();

        void removeFromParent();

        std::shared_ptr<SimpleNode> createChildNode();
        std::shared_ptr<SimpleNode> createChildNode(float, float );
        std::shared_ptr<SimpleNode> createChildNode(float, float, float );
        std::shared_ptr<SimpleNode> createChildNode(glm::vec2 );
        std::shared_ptr<SimpleNode> createChildNode(glm::vec3 );

        virtual bool containsChildNode(std::shared_ptr<SimpleNode> childNode);

        virtual void copyFrom(const SimpleNode* srcNode);

        void move(float, float);
        void move(float, float, float);
        void move(glm::vec2 );
        void move(glm::vec3 );
        void setPosition(float, float);
        void setPosition(float, float, float);
        void setPosition(glm::vec2 );
        virtual void setPosition(glm::vec3 );
        void setGlobalPosition(float, float);
        void setGlobalPosition(float, float, float);
        void setGlobalPosition(glm::vec2 );
        void setGlobalPosition(glm::vec3 );
        void setName(const std::string &name);
        void setRigidity(float rigidity);

        void scale(float scale);
        void scale(glm::vec3 scale);
        void linearScale(float, float, float);
        void linearScale(glm::vec3 scale);
        void setScale(float scale);
        virtual void setScale(glm::vec3 scale);
        void rotate(float value, glm::vec3 axis, bool inRadians = true);
        void rotate(glm::vec3 values, bool inRadians = true);
        virtual void setRotation(glm::vec3 rotation, bool inRadians = true);

        glm::vec3 getVelocity() const;

        virtual const glm::vec3 &getPosition() const;
        glm::vec2 getXYPosition() const;
        const glm::vec3 &getGlobalPosition() const;
        glm::vec2 getGlobalXYPosition() const;
        virtual const glm::vec3 &getScale() const;
        virtual const glm::vec3 &getEulerRotation()const;
        const glm::mat4 &getModelMatrix() const;
        const glm::mat4 &getInvModelMatrix() const;

        const std::string &getName()  const;
        float getRigidity() const;
        SimpleNode* getParent();
        SimpleNode* getChildByName(const std::string &name, bool recursiveSearch = true);
        void getNodesByName(std::map<std::string, SimpleNode*> &namesAndResMap);
        size_t getTreeDepth();

        virtual void update(const Time &elapsedTime = Time(0), uint32_t localTime = -1);
        ///virtual void rewind(uint32_t time);

        virtual void notify(NotificationSender* , int notificationType,
                            void* data = nullptr) override;


    protected:
        virtual std::shared_ptr<SimpleNode> nodeAllocator(/**NodeTypeId**/);

        virtual bool setParent(SimpleNode *parentNode); //Do not remove from old parent if new parent is null to prevent from self destruction of shared_ptr
        virtual bool setAsParentTo(SimpleNode *parentNode, SimpleNode *childNode);

        void askForUpdateModelMatrix();
        virtual void updateGlobalPosition();
        void updateModelMatrix();

        void computeFlexibleMove(glm::vec3 );

    protected:
        glm::vec3 m_lastGlobalPosition;
        glm::vec3 m_globalPosition;

        glm::vec3 m_position;
        glm::vec3 m_eulerRotations;
        glm::vec3 m_scale;

        float     m_rigidity;

        float m_curFlexibleLength;
        float m_curFlexibleRotation;

        glm::mat4 m_modelMatrix;
        glm::mat4 m_invModelMatrix;

        SimpleNode* m_parent;
        std::vector< std::shared_ptr<SimpleNode> > m_childs;


    private:
        std::string m_name;

        bool m_needToUpdateModelMat;
        size_t m_treeDepth;
};

}

#endif // SIMPLENODE_H
