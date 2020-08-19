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

        virtual void update(const Time &elapsedTime, float localTime = -1);
        ///virtual void rewind(float time);

        virtual void render(SceneRenderer *renderer, CameraObject *camera);

        SceneNode *getRootNode();

        virtual glm::vec2 convertScreenToWorldCoord(glm::vec2 p, CameraObject *cam = nullptr);
       // virtual const glm::mat4 &getViewMatrix() const;

        virtual void setAmbientLight(Color color);

        //virtual void generateViewAngle();

        //virtual void setViewAngle(float zAngle, float xyAngle);
        //virtual void setCurrentCamera(CameraObject *);

        void addRevealingProbe(SceneNode* probe);
        const std::unordered_set<SceneNode*> &getRevealingProbes();

        virtual void notify(NotificationSender* , int notificationType,
                            void* data = nullptr) override;

    protected:
        SceneNode m_rootNode;

        //float m_zAngle;
        //float m_xyAngle;
        float m_projectionFactor;
        /** I can probably remove the following : **/
        glm::mat4   m_viewAngle,    //World to screen transformation matrix (inv is transpose)
                    m_viewAngleInv;// 2D Screen to world transformation matrix

    private:
        //TextureAsset           *m_envMapAsset;
        SceneRenderingData      m_renderingData;

        std::unordered_set<SceneNode*> m_revealingProbes;
};

}

#endif // SCENE_H
