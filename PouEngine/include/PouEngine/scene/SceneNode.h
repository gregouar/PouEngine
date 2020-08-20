#ifndef SCENENODE_H
#define SCENENODE_H

#include <list>
#include <set>
#include <glm/glm.hpp>

#include "PouEngine/Types.h"
//#include "PouEngine/system/SimpleNode.h"
#include "PouEngine/scene/SceneEntity.h"
#include "PouEngine/scene/LightEntity.h"
#include "PouEngine/scene/ShadowCaster.h"
#include "PouEngine/scene/SoundObject.h"
#include "PouEngine/math/TransformComponent.h"


namespace pou
{

class Scene;
class SceneRenderer;

class SceneNode :  public NotificationSender, public NotificationListener //: public SimpleNode
{
    public:
        SceneNode();
        SceneNode(Scene* scene);
        virtual ~SceneNode();

        std::shared_ptr<SceneNode> createChildNode();
        virtual void addChildNode(std::shared_ptr<SceneNode> childNode);
        bool containsChildNode(SceneNode *childNode);
        virtual bool removeChildNode(SceneNode *childNode);
        void removeFromParent();

        void attachObject(std::shared_ptr<SceneObject>);
        void detachObject(SceneObject*);

        void attachSound(std::shared_ptr<SoundObject>, int);
        void detachSound(SoundObject *, int);

        void detachAllObjects();

        void copyFrom(const SceneNode* srcNode); //Also copy childs and objects

        void disableCollisions(bool disable = true);
        bool areCollisionsDisabled();

        void colorize(const glm::vec4 &c);
        virtual void setColor(const glm::vec4 &c);

        /**sf::FloatRect getGlobalBounds();
        sf::FloatRect getBounds();**/

        Scene*  getScene();
        SceneNode*  getParentNode();
        const glm::vec4 &getColor() const;
        const glm::vec4 &getFinalColor() const;

        virtual void generateRenderingData(SceneRenderingInstance *renderingInstance);
        void generateRenderingDataWithoutPropagating(SceneRenderingInstance *renderingInstance);
        bool playSound(int id);

        virtual void update(const Time &elapsedTime = Time(0), uint32_t localTime = -1);

        TransformComponent *transform();
        const TransformComponent *const_transform() const;

        void setName(HashedString name);
        HashedString getName() const;
        void getNodesByName(std::unordered_map<HashedString, SceneNode*> &namesAndResMap);

        void setNodeId(uint32_t id);
        uint32_t getNodeId() const;

    protected:
        virtual void notify(NotificationSender *sender, int notificationType, void* data = nullptr) override;
        //virtual std::shared_ptr<SceneNode> nodeAllocator();

        bool setParentNode(SceneNode *parentNode);
        bool setAsParentTo(SceneNode *parentNode, SceneNode *childNode);
        void setScene(Scene *);
        void updateFinalColor();

    protected:
        Scene* m_scene;

    private:
        uint32_t m_nodeId;

        SceneNode *m_parentNode;
        std::vector< std::shared_ptr<SceneNode> >       m_childNodes;

        std::vector< std::shared_ptr<SceneObject> >     m_attachedObjects;
        std::vector< std::shared_ptr<SceneEntity> >     m_attachedEntities;
        std::vector< std::shared_ptr<LightEntity> >     m_attachedLights;
        std::vector< std::shared_ptr<ShadowCaster> >    m_attachedShadowCasters;

        std::unordered_multimap<HashedString, std::shared_ptr<SoundObject> > m_attachedSounds;

        bool m_disableCollisions;

        TransformComponent m_transformComponent;
        glm::vec4 m_color;
        glm::vec4 m_finalColor; ///Reimplement this later

        HashedString m_name;

};

}

#endif // SCENENODE_H
