#ifndef SCENENODE_H
#define SCENENODE_H

#include <list>
#include <set>
#include <glm/glm.hpp>

#include "PouEngine/Types.h"
#include "PouEngine/utils/SimpleNode.h"
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
        SceneNode(const NodeTypeId);
        SceneNode(const NodeTypeId, Scene* scene);
        virtual ~SceneNode();

        SceneNode* createChildNode();
        SceneNode* createChildNode(float, float );
        SceneNode* createChildNode(float, float, float );
        SceneNode* createChildNode(glm::vec2 );
        SceneNode* createChildNode(glm::vec3 );
        SceneNode* createChildNode(const NodeTypeId id);

        void attachObject(SceneObject *);
        void detachObject(SceneObject *);

        void attachSound(SoundObject *, int);
        void detachSound(SoundObject *, int);

        void detachAllObjects();

        virtual void syncFromNode(SceneNode* srcNode);

        void colorize(const glm::vec4 &c);
        void setColor(const glm::vec4 &c);

        /**sf::FloatRect getGlobalBounds();
        sf::FloatRect getBounds();**/

        Scene*  getScene();
        const glm::vec4 &getColor() const;
        const glm::vec4 &getFinalColor() const;

        void generateRenderingData(SceneRenderingInstance *renderingInstance);
        bool playSound(int id);

        virtual void update(const Time &elapsedTime, float localTime = -1);


        virtual void serializeNode(pou::Stream *stream, float localTime = -1);

        /*virtual void notify(NotificationSender* , NotificationType,
                            size_t dataSize = 0, char* data = nullptr) override;*/

    protected:
        virtual SimpleNode* nodeAllocator(NodeTypeId);

        virtual void setParent(SimpleNode *);
        void setScene(Scene *);
        virtual void updateGlobalPosition();

    protected:
        Scene* m_scene;
        //glm::vec4 m_color;
        LinSyncedAttribute<glm::vec4> m_color;
        glm::vec4 m_finalColor;

        //float m_lastColorUpdateTime;

    private:
        std::list<SceneObject *>    m_attachedObjects;
        std::list<SceneEntity *>    m_attachedEntities;
        std::list<LightEntity *>    m_attachedLights;
        std::list<ShadowCaster *>   m_attachedShadowCasters;

        std::multimap<int, SoundObject *> m_attachedSounds;
};

}

#endif // SCENENODE_H
