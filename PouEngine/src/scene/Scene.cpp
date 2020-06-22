#include "PouEngine/scene/Scene.h"

#include "PouEngine/utils/Logger.h"
#include "PouEngine/renderers/SceneRenderer.h"
//#include "PouEngine/renderers/PBRToolbox.h"
#include "PouEngine/scene/SceneObject.h"
#include "PouEngine/assets/TextureAsset.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

namespace pou
{


Scene::Scene() :
    m_rootNode(/**0,**/ this)
{
    m_rootNode.setPosition(0,0,0);
    ///m_curNewId = 0;

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

    ///this->destroyAllCreatedObjects();
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
        glm::vec3 camPos          = camera->getParentNode()->getGlobalPosition();
        glm::mat4 camTranslate    = glm::translate(glm::mat4(1.0), -camPos);
        glm::mat4 camTranslateInv = glm::translate(glm::mat4(1.0), camPos);

        //std::cout<<camPos.z<<std::endl;

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

        //for(auto i = 0 ; i < 4 ; ++i)
        //std::cout<<projMat[i][0]<<" "<<projMat[i][1]<<" "<<projMat[i][2]<<" "<<projMat[i][3]<<std::endl;

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

/**CameraObject* Scene::createCamera()
{
    CameraObject* camera = new CameraObject();
    this->addCreatedObject(this->generateObjectId(), camera);
    return camera;
}

SpriteEntity *Scene::createSpriteEntity(SpriteModel *model)
{
    SpriteEntity *entity = new SpriteEntity();
    entity->setSpriteModel(model);
    this->addCreatedObject(this->generateObjectId(), entity);
    return entity;
}

MeshEntity *Scene::createMeshEntity(MeshAsset *model)
{
    MeshEntity *entity = new MeshEntity();
    entity->setMesh(model);
    this->addCreatedObject(this->generateObjectId(), entity);
    return entity;
}

LightEntity *Scene::createLightEntity(LightType type, Color color, float intensity)
{
    LightEntity *entity = new LightEntity();
    entity->setType(type);
    entity->setDiffuseColor(color);
    entity->setIntensity(intensity);
    this->addCreatedObject(this->generateObjectId(), entity);
    return entity;
}*


void Scene::addCreatedObject(const ObjectTypeId id, SceneObject* obj)
{
    auto entityIt = m_createdObjects.find(id);

    if(entityIt != m_createdObjects.end())
    {
        std::ostringstream warn_report;
        warn_report << "Adding scene object of same id as another one (Id="<<id<<")";
        Logger::warning(warn_report);
    }

    m_createdObjects[id] = obj;
}


void Scene::destroyCreatedObject(const ObjectTypeId id)
{
    auto objIt = m_createdObjects.find(id);

    if(objIt == m_createdObjects.end())
    {
        std::ostringstream error_report;
        error_report << "Cannot destroy scene object (Id="<<id<<")";
        Logger::error(error_report);
    } else {
        if(objIt->second != nullptr)
            delete objIt->second;
        m_createdObjects.erase(objIt);
    }
}

void Scene::destroyAllCreatedObjects()
{
    for(auto it : m_createdObjects)
        if(it.second != nullptr)
            delete it.second;
    m_createdObjects.clear();
}**/


glm::vec2 Scene::convertScreenToWorldCoord(glm::vec2 p, CameraObject *cam)
{
    glm::vec3 camPos = glm::vec3(0.0,0.0,0.0);
    if(cam != nullptr)
        camPos = cam->getParentNode()->getGlobalPosition();

    //glm::vec4 worldPos = m_viewAngleInv*glm::vec4(pos,1.0);
    glm::vec4 worldPos = glm::vec4(p,0.0,1.0);
    worldPos += glm::vec4(camPos,0.0);

    return {worldPos.x, worldPos.y};
}


void Scene::setAmbientLight(Color color)
{
    m_renderingData.setAmbientLight(color);
}


void Scene::notify(NotificationSender *sender, int notificationType,
                   void* data)
{

}

/**ObjectTypeId Scene::generateObjectId()
{
    return m_curNewId++;
}**/


}
