#include "PouEngine/scene/Scene.h"

#include "PouEngine/tools/Logger.h"
#include "PouEngine/renderers/SceneRenderer.h"
//#include "PouEngine/renderers/PBRToolbox.h"
#include "PouEngine/scene/SceneObject.h"
#include "PouEngine/assets/TextureAsset.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

namespace pou
{


Scene::Scene() :
    m_rootNode(this)
{
    m_rootNode.transform()->setPosition(glm::vec3(0));

    m_projectionFactor  = 1000.0f;
    m_viewAngle         = glm::mat4(1.0);
    m_viewAngleInv      = glm::mat4(1.0);
}

Scene::~Scene()
{
    this->cleanAll();
}

void Scene::cleanAll()
{
    ///m_rootNode.removeAllChilds();

    m_renderingData.cleanup();
}


void Scene::update(const Time &elapsedTime, float localTime)
{
    m_rootNode.update(elapsedTime,localTime);

   // m_renderingData.update();
}

/**void Scene::rewind(float time)
{
    m_rootNode.rewind(time);
}**/

void Scene::render(SceneRenderer *renderer, CameraObject *camera)
{
    if(!m_renderingData.isInitialized())
        m_renderingData.init(renderer);

    if(camera != nullptr)
    {
        glm::vec3 camPos          = glm::round(camera->getParentNode()->transform()->getGlobalPosition());
        glm::mat4 camTranslate    = glm::translate(glm::mat4(1.0), -camPos);
        glm::mat4 camTranslateInv = glm::translate(glm::mat4(1.0), camPos);

        ViewInfo viewInfo;
        viewInfo.view     = m_viewAngle;
        viewInfo.viewInv  = m_viewAngleInv;
        viewInfo.projFactor = m_projectionFactor;

        /*glm::mat4 projMat = //glm::perspective(45.0f, 1024.0f/768.0f, 1.0f, 150.0f);
         glm::perspective(45.0f, 1024.0f/768.0f, .01f, 500.0f);*/
        //projMat[1][1] *= -1;

        //glm::mat4 projMat = glm::mat4(1);

       /* float K = 10.0f;
        glm::mat4 projMat = glm::mat4(
            1,0,0,0,
            0,1,0,0,
            );*/

        //viewInfo.proj  = projMat;

        renderer->setView(viewInfo);

        SceneRenderingInstance *renderingInstance = new SceneRenderingInstance(&m_renderingData, renderer);
        viewInfo.view     = m_viewAngle*camTranslate;
        viewInfo.viewInv  = camTranslateInv*m_viewAngleInv;
        viewInfo.viewportOffset = camera->getViewportOffset();
        viewInfo.viewportExtent = camera->getViewportExtent();


        renderingInstance->setViewInfo(viewInfo, camPos, camera->getZoom());

        m_rootNode.generateRenderingData(renderingInstance);
        renderingInstance->computeOrdering();

        renderer->addRenderingInstance(renderingInstance);
    }
}

SceneNode *Scene::getRootNode()
{
    return &m_rootNode;
}

glm::vec2 Scene::convertScreenToWorldCoord(glm::vec2 p, CameraObject *cam)
{
    glm::vec3 camPos = glm::vec3(0.0,0.0,0.0);
    if(cam != nullptr)
        camPos = cam->getParentNode()->transform()->getGlobalPosition();

    //glm::vec4 worldPos = m_viewAngleInv*glm::vec4(pos,1.0);
    glm::vec4 worldPos = glm::vec4(p,0.0,1.0);
    worldPos += glm::vec4(camPos,0.0);

    return {worldPos.x, worldPos.y};
}


void Scene::setAmbientLight(Color color)
{
    m_renderingData.setAmbientLight(color);
}

void Scene::addRevealingProbe(SceneNode* probe)
{
    if(m_revealingProbes.insert(probe).second)
        this->startListeningTo(probe, NotificationType_SenderDestroyed);
}

const std::unordered_set<SceneNode*> &Scene::getRevealingProbes()
{
    return m_revealingProbes;
}

void Scene::notify(NotificationSender *sender, int notificationType,
                   void* data)
{
    if(notificationType == NotificationType_SenderDestroyed)
    {
        m_revealingProbes.erase((SceneNode*)sender);
    }
}



}
