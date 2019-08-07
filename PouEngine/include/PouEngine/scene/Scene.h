#ifndef SCENE_H
#define SCENE_H

#include "PouEngine/Types.h"
#include "PouEngine/core/NotificationListener.h"
#include "PouEngine/scene/SceneNode.h"
#include "PouEngine/scene/CameraObject.h"
#include "PouEngine/renderers/SceneRenderingInstance.h"
#include "PouEngine/renderers/SceneRenderingData.h"

namespace pou
{

class Scene : public NotificationListener
{
    public:
        Scene();
        virtual ~Scene();

        virtual void cleanAll();

        virtual void update(const Time &elapsedTime);

        virtual void render(SceneRenderer *renderer, CameraObject *camera);

        SceneNode *getRootNode();

        CameraObject       *createCamera();
        SpriteEntity       *createSpriteEntity(SpriteModel *model = nullptr);
        //MeshEntity         *createMeshEntity(MeshAsset *model = nullptr);
        LightEntity        *createLightEntity(LightType type = pou::LightType_Omni,
                                              Color color = {1.0,1.0,1.0,1.0}, float intensity = 1.0);

        void destroyCreatedObject(const ObjectTypeId);
        void destroyAllCreatedObjects();

        virtual glm::vec2 convertScreenToWorldCoord(glm::vec2 p, CameraObject *cam = nullptr);
       // virtual const glm::mat4 &getViewMatrix() const;

        virtual void setAmbientLight(Color color);

        //virtual void generateViewAngle();

        //virtual void setViewAngle(float zAngle, float xyAngle);
        //virtual void setCurrentCamera(CameraObject *);

        virtual void notify(NotificationSender* , NotificationType,
                            size_t dataSize = 0, char* data = nullptr) override;

    protected:
        ObjectTypeId generateObjectId();
        void addCreatedObject(const ObjectTypeId, SceneObject*);

        SceneNode m_rootNode;

        //float m_zAngle;
        //float m_xyAngle;
        /** I can probably remove the following : **/
        glm::mat4   m_viewAngle,    //World to screen transformation matrix (inv is transpose)
                    m_viewAngleInv;// 2D Screen to world transformation matrix

    private:
        std::map<ObjectTypeId, SceneObject*> m_createdObjects;
        ObjectTypeId m_curNewId;

        //TextureAsset           *m_envMapAsset;
        SceneRenderingData      m_renderingData;

        ///static const sf::Vector2u DEFAULT_SHADOWMAP_SIZE;
};

}

#endif // SCENE_H
