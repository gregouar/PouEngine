#ifndef SCENENODE_H
#define SCENENODE_H

#include <list>
#include <set>
#include <glm/glm.hpp>

#include "PouEngine/Types.h"
#include "PouEngine/system/SimpleNode.h"
#include "PouEngine/scene/SceneEntity.h"
#include "PouEngine/scene/LightEntity.h"
#include "PouEngine/scene/ShadowCaster.h"
#include "PouEngine/scene/SoundObject.h"

/*#include "PouEngine/core/NotificationListener.h"
#include "PouEngine/core/NotificationSender.h"*/


namespace pou
{

class Scene;
class SceneRenderer;

class SceneNode : public SimpleNode //public NotificationSender, public NotificationListener
{
    public:
        SceneNode(/**const NodeTypeId**/);
        SceneNode(/**const NodeTypeId,**/ Scene* scene);
        virtual ~SceneNode();

        std::shared_ptr<SceneNode> createChildNode();
        std::shared_ptr<SceneNode> createChildNode(float, float );
        std::shared_ptr<SceneNode> createChildNode(float, float, float );
        std::shared_ptr<SceneNode> createChildNode(glm::vec2 );
        std::shared_ptr<SceneNode> createChildNode(glm::vec3 );
        ///SceneNode* createChildNode(const NodeTypeId id);

        void attachObject(std::shared_ptr<SceneObject>);
        void detachObject(SceneObject*);

        void attachSound(std::shared_ptr<SoundObject>, int);
        void detachSound(SoundObject *, int);

        void detachAllObjects();

        using SimpleNode::copyFrom;
        virtual void copyFrom(const SceneNode* srcNode); //Also copy childs and objects

        void disableCollisions(bool disable = true);
        bool areCollisionsDisabled();

        void colorize(const glm::vec4 &c);
        virtual void setColor(const glm::vec4 &c);

        /**sf::FloatRect getGlobalBounds();
        sf::FloatRect getBounds();**/

        Scene*  getScene();
        virtual const glm::vec4 &getColor() const;
        const glm::vec4 &getFinalColor() const;

        virtual void generateRenderingData(SceneRenderingInstance *renderingInstance, bool propagateToChilds = true);
        bool playSound(int id);

        virtual void update(const Time &elapsedTime, uint32_t localTime = -1);

    protected:
        virtual std::shared_ptr<SimpleNode> nodeAllocator(/**NodeTypeId**/);

        virtual bool setParent(SimpleNode *parentNode);
        void setScene(Scene *);
        virtual void updateGlobalPosition();

    protected:
        Scene* m_scene;
        glm::vec4 m_color;
        glm::vec4 m_finalColor;

    private:
        std::vector<std::shared_ptr<SceneObject> >    m_attachedObjects;
        std::vector<std::shared_ptr<SceneEntity> >    m_attachedEntities;
        std::vector<std::shared_ptr<LightEntity> >    m_attachedLights;
        std::vector<std::shared_ptr<ShadowCaster> >   m_attachedShadowCasters;

        std::multimap<int, std::shared_ptr<SoundObject> > m_attachedSounds;

        bool m_disableCollisions;
};

}

#endif // SCENENODE_H
