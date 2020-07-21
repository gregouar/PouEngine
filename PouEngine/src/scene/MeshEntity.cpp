#include "PouEngine/scene/MeshEntity.h"

#include "PouEngine/assets/MeshAsset.h"

#include "PouEngine/renderers/SceneRenderer.h"
#include "PouEngine/scene/SceneNode.h"

#include "PouEngine/assets/MeshesHandler.h"

namespace pou
{


VkVertexInputBindingDescription MeshDatum::getBindingDescription()
{
    VkVertexInputBindingDescription bindingDescription = {};
    bindingDescription.binding = 1;
    bindingDescription.stride = sizeof(MeshDatum);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

    return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 11> MeshDatum::getAttributeDescriptions()
{
    std::array<VkVertexInputAttributeDescription, 11> attributeDescriptions = {};

    uint32_t i = 0;
    uint32_t d = MeshVertex::getAttributeDescriptions().size();
    uint32_t b = 1;
    attributeDescriptions[i].binding = b;
    attributeDescriptions[i].location = d+i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(MeshDatum, model_0);
    ++i;

    attributeDescriptions[i].binding = b;
    attributeDescriptions[i].location = d+i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(MeshDatum, model_1);
    ++i;

    attributeDescriptions[i].binding = b;
    attributeDescriptions[i].location = d+i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(MeshDatum, model_2);
    ++i;

    attributeDescriptions[i].binding = b;
    attributeDescriptions[i].location = d+i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(MeshDatum, model_3);
    ++i;


    attributeDescriptions[i].binding = b;
    attributeDescriptions[i].location = d+i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(MeshDatum, albedo_color);
    ++i;

    attributeDescriptions[i].binding = b;
    attributeDescriptions[i].location = d+i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(MeshDatum, rme_color);
    ++i;


    attributeDescriptions[i].binding = b;
    attributeDescriptions[i].location = d+i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32_UINT;
    attributeDescriptions[i].offset = offsetof(MeshDatum, albedo_texId);
    ++i;

    attributeDescriptions[i].binding = b;
    attributeDescriptions[i].location = d+i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32_UINT;
    attributeDescriptions[i].offset = offsetof(MeshDatum, height_texId);
    ++i;

    attributeDescriptions[i].binding = b;
    attributeDescriptions[i].location = d+i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32_UINT;
    attributeDescriptions[i].offset = offsetof(MeshDatum, normal_texId);
    ++i;

    attributeDescriptions[i].binding = b;
    attributeDescriptions[i].location = d+i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32_UINT;
    attributeDescriptions[i].offset = offsetof(MeshDatum, rme_texId);
    ++i;

    attributeDescriptions[i].binding = b;
    attributeDescriptions[i].location = d+i;
    attributeDescriptions[i].format = VK_FORMAT_R32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(MeshDatum, texThickness);
    ++i;


    return attributeDescriptions;
}


MeshEntity::MeshEntity() :
    m_mesh(nullptr),
    m_color(1.0,1.0,1.0,1.0),
    m_rme(1.0,1.0,1.0),
    m_scale(1.0,1.0,1.0)
{
    this->updateDatum();
}

MeshEntity::~MeshEntity()
{
    //dtor
}

std::shared_ptr<SceneObject> MeshEntity::createCopy()
{
    auto newObject = std::make_shared<MeshEntity>();
    newObject->setColor(m_color);
    newObject->setRme(m_rme);
    newObject->setScale(m_scale);
    newObject->setMeshModel(m_mesh);
    newObject->setShadowCastingType(m_shadowCastingType);
    return newObject;
}

bool MeshEntity::setMeshModel(AssetTypeId meshId)
{
    return this->setMeshModel(MeshesHandler::instance()->getAsset(meshId));
}

bool MeshEntity::setMeshModel(MeshAsset* mesh)
{
    if(m_mesh != mesh)
    {
        this->stopListeningTo(m_mesh);
        m_mesh = mesh;
        ///this->setLastUpdateTime(m_curLocalTime);
        this->startListeningTo(m_mesh);
        this->updateDatum();

        return (true);
    }

    return (false);
}


void MeshEntity::setColor(Color color)
{
    if(m_color != color)
    {
        m_color = color;
        ///this->setLastUpdateTime(m_curLocalTime);
        this->updateDatum();
    }
}

void MeshEntity::setRme(glm::vec3 rme)
{
    if(m_rme != rme)
    {
        m_rme = rme;
        ///this->setLastUpdateTime(m_curLocalTime);
        this->updateDatum();
    }
}

void MeshEntity::setScale(float scale)
{
    this->setScale({scale, scale, scale});
}

void MeshEntity::setScale(glm::vec3 scale)
{
    if(m_scale != scale)
    {
        m_scale = scale;
        ///this->setLastUpdateTime(m_curLocalTime);
        this->updateDatum();
    }
}

MeshDatum MeshEntity::getMeshDatum()
{
    return m_datum;
}

MeshAsset *MeshEntity::getMeshModel()
{
    return m_mesh;
}

glm::vec3 MeshEntity::getScale()
{
    return m_scale;
}

void MeshEntity::generateRenderingData(SceneRenderingInstance *renderingInstance)
{
    if(m_mesh != nullptr && /**m_mesh->isLoaded() &&**/ m_datum.albedo_color.a > 0)
        renderingInstance->addToMeshesVbo(m_mesh->getMesh(), this->getMeshDatum());
}

glm::vec2 MeshEntity::castShadow(SceneRenderer *renderer, LightEntity* light)
{
    if(m_mesh == nullptr || !m_mesh->isLoaded())
        return glm::vec2{0.0};

    if(light->getType() == LightType_Directional)
        renderer->addToMeshShadowsVbo(m_mesh->getMesh(), m_datum);

    ///I should compute bounding box and then maxShadowShift
    return glm::vec2{0.0};
}

/*void MeshEntity::draw(SceneRenderer *renderer)
{
    if(m_mesh != nullptr && m_mesh->isLoaded())
        renderer->addToMeshesVbo(m_mesh->getMesh(), this->getMeshDatum());
}*/

void MeshEntity::notify(NotificationSender *sender, int notificationType,
                        void* data)
{
    if(notificationType == NotificationType_AssetLoaded ||
       notificationType == NotificationType_TextureChanged ||
       notificationType == NotificationType_NodeMoved)
        this->updateDatum();

    if(notificationType == NotificationType_SenderDestroyed)
    {
        if(sender == m_mesh)
            m_mesh = nullptr;
    }
}

void MeshEntity::updateDatum()
{
    m_datum = {};
    if(m_mesh == nullptr || m_parentNode == nullptr)
        return;

    m_datum.albedo_color  = m_color;
    m_datum.rme_color     = m_rme;
    m_datum.texThickness  = 0.0;

    m_datum.albedo_texId = {};
    m_datum.height_texId = {};
    m_datum.normal_texId = {};
    m_datum.rme_texId = {};

    MaterialAsset* material = m_mesh->getMaterial();
    if(material != nullptr && material->isLoaded())
    {
        m_datum.albedo_texId = {material->getAlbedoMap().getTextureId(),
                                material->getAlbedoMap().getTextureLayer()};
        m_datum.height_texId = {material->getHeightMap().getTextureId(),
                                material->getHeightMap().getTextureLayer()};
        m_datum.normal_texId = {material->getNormalMap().getTextureId(),
                                material->getNormalMap().getTextureLayer()};
        m_datum.rme_texId    = {material->getRmeMap().getTextureId(),
                                material->getRmeMap().getTextureLayer()};
        m_datum.rme_color *= material->getRmeFactor();

        if(!(m_datum.normal_texId.x == 0 && m_datum.normal_texId.y == 0))
            m_datum.texThickness = material->getHeightFactor();
    }

    glm::vec3 scale = m_scale * m_mesh->getScale();

    /*glm::mat4 modelMatrix = glm::mat4(1.0);

    modelMatrix = glm::translate(modelMatrix, m_parentNode->getGlobalPosition());

    //I should use quaternion (and also inherits rotations from parents)
    modelMatrix = glm::rotate(modelMatrix, m_parentNode->getEulerRotation().x, glm::vec3(1.0,0.0,0.0));
    modelMatrix = glm::rotate(modelMatrix, m_parentNode->getEulerRotation().y, glm::vec3(0.0,1.0,0.0));
    modelMatrix = glm::rotate(modelMatrix, m_parentNode->getEulerRotation().z, glm::vec3(0.0,0.0,1.0));

    modelMatrix = glm::scale(modelMatrix, scale);
    modelMatrix = glm::scale(modelMatrix, m_parentNode->getScale());*/



    glm::mat4 modelMat(1.0);

   // if(m_inheritRotation)
        modelMat = m_parentNode->getModelMatrix() * modelMat;
   /* else
        modelMat = glm::translate(modelMat, m_parentNode->getGlobalPosition());*/

    //modelMat = glm::rotate(modelMat, m_rotation, glm::vec3(0.0,0.0,1.0));
    modelMat = glm::scale(modelMat, scale);

    m_datum.model_0 = modelMat[0];
    m_datum.model_1 = modelMat[1];
    m_datum.model_2 = modelMat[2];
    m_datum.model_3 = modelMat[3];

}

}
