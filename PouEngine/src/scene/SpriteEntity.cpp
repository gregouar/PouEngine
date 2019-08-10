#include "PouEngine/scene/SpriteEntity.h"

#include "PouEngine/assets/AssetHandler.h"
#include "PouEngine/assets/TextureAsset.h"
#include "PouEngine/renderers/SceneRenderer.h"
#include "PouEngine/scene/SceneNode.h"
#include "PouEngine/scene/LightEntity.h"

namespace pou
{


VkVertexInputBindingDescription SpriteDatum::getBindingDescription()
{
    VkVertexInputBindingDescription bindingDescription = {};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(SpriteDatum);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

    return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 8> SpriteDatum::getAttributeDescriptions()
{
    std::array<VkVertexInputAttributeDescription, 8> attributeDescriptions = {};

    size_t i = 0;
    /*attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(SpriteDatum, position);
    ++i;

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(SpriteDatum, rotation);
    ++i;

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(SpriteDatum, size);
    ++i;

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(SpriteDatum, center);
    ++i;*/

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(SpriteDatum, modelMat0);
    ++i;

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(SpriteDatum, modelMat1);
    ++i;

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(SpriteDatum, modelMat2);
    ++i;

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(SpriteDatum, modelMat3);
    ++i;

    /////////////////////////////////////////////////////////////

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(SpriteDatum, albedo_color);
    ++i;

    /*attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(SpriteDatum, rmt_color);
    ++i;*/

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(SpriteDatum, texPos);
    ++i;

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(SpriteDatum, texExtent);
    ++i;

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32_UINT;
    attributeDescriptions[i].offset = offsetof(SpriteDatum, albedo_texId);
    ++i;

    /*attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32_UINT;
    attributeDescriptions[i].offset = offsetof(SpriteDatum, height_texId);
    ++i;

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32_UINT;
    attributeDescriptions[i].offset = offsetof(SpriteDatum, normal_texId);
    ++i;

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32_UINT;
    attributeDescriptions[i].offset = offsetof(SpriteDatum, rmt_texId);
    ++i;*/

    return attributeDescriptions;
}

/*VkVertexInputBindingDescription SpriteShadowDatum::getBindingDescription()
{
    VkVertexInputBindingDescription bindingDescription = {};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(SpriteShadowDatum);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

    return bindingDescription;
}*/

/*std::array<VkVertexInputAttributeDescription, 6> SpriteShadowDatum::getAttributeDescriptions()
{
    std::array<VkVertexInputAttributeDescription, 6> attributeDescriptions = {};

    size_t i = 0;
    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(SpriteShadowDatum, position);
    ++i;

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(SpriteShadowDatum, size);
    ++i;

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(SpriteShadowDatum, center);
    ++i;

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(SpriteShadowDatum, texPos);
    ++i;

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(SpriteShadowDatum, texExtent);
    ++i;

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32_UINT;
    attributeDescriptions[i].offset = offsetof(SpriteShadowDatum, texId);
    ++i;

    return attributeDescriptions;
}*/


SpriteEntity::SpriteEntity() :
    m_spriteModel(nullptr),
    m_rotation(0.0f),
    m_color(1.0,1.0,1.0,1.0),
    m_ordering(NOT_ORDERED),
    m_inheritRotation(false)
    //m_rmt(1.0,1.0,1.0)
{
    this->updateDatum();
}

SpriteEntity::~SpriteEntity()
{
    //dtor
}

void SpriteEntity::rotate(float rotation)
{
    this->setRotation(this->getRotation() + rotation);
}

void SpriteEntity::setRotation(float rotation)
{
    if(m_rotation != rotation)
    {
        m_rotation = rotation;
        this->updateDatum();
    }
}


void SpriteEntity::setColor(Color color)
{
    if(m_color != color)
    {
        m_color = color;
        this->updateDatum();
    }
}

void SpriteEntity::setOrdering(SpriteOrdering ordering)
{
    if(ordering >= 0 && ordering < TOTAL_SPRITE_ORDERINGS)
        m_ordering = ordering;
}

void SpriteEntity::setInheritRotation(bool inheritRotation)
{
    if(m_inheritRotation != inheritRotation)
    {
        m_inheritRotation = inheritRotation;
        this->updateDatum();
    }
}

/*void SpriteEntity::setRmt(glm::vec3 rmt)
{
    if(m_rmt != rmt)
    {
        m_rmt = rmt;
        this->updateDatum();
    }
}*/

void SpriteEntity::setSpriteModel(SpriteModel* model)
{
    if(m_spriteModel != model)
    {
        this->stopListeningTo(m_spriteModel);
        m_spriteModel = model;
        this->startListeningTo(m_spriteModel);
        this->updateDatum();
    }
}


float SpriteEntity::getRotation()
{
    return m_rotation;
}

Color SpriteEntity::getColor()
{
    return m_color;
}

SpriteOrdering SpriteEntity::getOrdering()
{
    return m_ordering;
}

/*glm::vec3 SpriteEntity::getRmt()
{
    return m_rmt;
}*/

SpriteDatum SpriteEntity::getSpriteDatum()
{
    return m_datum;
}


void SpriteEntity::generateRenderingData(SceneRenderingInstance *renderingInstance)
{
    if(m_spriteModel == nullptr || m_parentNode == nullptr)
        return;

    if(m_spriteModel->isReady())
    {
        if(m_ordering == NOT_ORDERED)
        {
            renderingInstance->addToSpritesVbo(this->getSpriteDatum());
            return;
        }

        if(m_ordering == ORDERED_BY_Z)
            renderingInstance->addToSpritesOrdered(this->getSpriteDatum(),
                                                    this->getSpriteDatum().modelMat3[2]);
                                                    //this->getSpriteDatum().position.z);*
    }
}

glm::vec2 SpriteEntity::castShadow(SceneRenderer *renderer, LightEntity* light)
{
    /*if(m_spriteModel == nullptr || m_parentNode == nullptr)
        return glm::vec2(0.0);

    if(!m_spriteModel->isReady())
        return glm::vec2(0.0);

    this->startListeningTo(light);

    if(light->getType() == LightType_Directional)
    {
        VTexture shadow = m_spriteModel->getDirectionnalShadow(renderer, light->getDirection());

        glm::vec2 shadowShift = this->generateShadowDatum(light->getDirection());

        m_shadowDatum.texId = shadow.getTexturePair();
        renderer->addToSpriteShadowsVbo(m_shadowDatum);

        return shadowShift;
    }*/

    return glm::vec2(0.0);
}

/*glm::vec2 SpriteEntity::generateShadowDatum(glm::vec3 direction)
{
    if(m_parentNode == nullptr || m_parentNode->getScene() == nullptr)
        return {0.0,0.0};

	glm::vec3 lightDirection = normalize(direction);

	glm::vec2 lightDirectionXY = {lightDirection.x, lightDirection.y};

	glm::vec4 v = glm::vec4(lightDirectionXY / -lightDirection.z, 0.0, 0.0);
	glm::vec4 r = m_parentNode->getScene()->getViewMatrix() * v;
	//r.y        -=  m_parentNode->getScene()->getViewMatrix()[2][1];
	glm::vec2 viewLightDirectionXY = {m_datum.size.z * r.x,
                                      m_datum.size.z *(r.y  - m_parentNode->getScene()->getViewMatrix()[2][1])};

    glm::vec2 totalSize = glm::abs(viewLightDirectionXY)+glm::vec2(m_datum.size.x, m_datum.size.y);

    glm::vec2 spritePos;
    spritePos = glm::max(glm::vec2(0.0), -viewLightDirectionXY);

    m_shadowDatum.position  = m_datum.position + glm::vec3(glm::vec2(v.x,v.y)*m_datum.position.z,0.0);
    m_shadowDatum.size      = glm::vec3(totalSize, m_datum.size.z);
    m_shadowDatum.center    = m_datum.center+spritePos;

    return viewLightDirectionXY;
}*/

void SpriteEntity::notify(NotificationSender *sender, NotificationType notification,
                             size_t dataSize, char* data)
{
    if(notification == Notification_AssetLoaded ||
       notification == Notification_TextureChanged ||
       notification == Notification_ModelChanged ||
       notification == Notification_NodeMoved)
        this->updateDatum();

    /*if(notification == Notification_UpdateShadow)
    {
        if(m_spriteModel != nullptr)
        {
            float* f = (float*)data;
            glm::vec3 d(*f, *(f+1), *(f+2));
            m_spriteModel->updateDirectionnalShadow(d,(dynamic_cast<LightEntity*>(sender))->getDirection());
        }
    }*/

    if(notification == Notification_SenderDestroyed)
    {
        if(sender == m_spriteModel)
        {
            m_spriteModel = nullptr;
            this->updateDatum();
        }
    }
}

void SpriteEntity::updateDatum()
{
    m_datum = {};
   // m_shadowDatum = {};

    if(m_spriteModel == nullptr || m_parentNode == nullptr)
        return;

    //float heightFactor = 1.0;

    m_datum.albedo_color = m_color;
    //m_datum.rmt_color = m_rmt;

    /*MaterialAsset* material = m_spriteModel->getMaterial();
    if(material != nullptr && material->isLoaded())
    {
        m_datum.albedo_texId = {material->getAlbedoMap().getTextureId(),
                                material->getAlbedoMap().getTextureLayer()};
        m_datum.height_texId = {material->getHeightMap().getTextureId(),
                                material->getHeightMap().getTextureLayer()};
        m_datum.normal_texId = {material->getNormalMap().getTextureId(),
                                material->getNormalMap().getTextureLayer()};
        m_datum.rmt_texId    = {material->getRmtMap().getTextureId(),
                                material->getRmtMap().getTextureLayer()};

        //heightFactor = material->getHeightFactor();
        //m_datum.rmt_color *= material->getRmtFactor();
    }*/

    if(m_spriteModel->getTexture() != nullptr)
    {
        m_datum.albedo_texId = {m_spriteModel->getTexture()->getVTexture().getTextureId(),
                                m_spriteModel->getTexture()->getVTexture().getTextureLayer()};
    } else {
        m_datum.albedo_texId = {0,0};
    }


    /*m_datum.position = m_parentNode->getGlobalPosition();
    m_datum.size = m_spriteModel->getSize();
    m_datum.rotation = m_rotation;
    m_datum.center = m_spriteModel->getCenter();*/

    glm::mat4 modelMat(1.0);

    if(m_inheritRotation)
        modelMat = m_parentNode->getModelMatrix() * modelMat;
    else
        modelMat = glm::translate(modelMat, m_parentNode->getGlobalPosition());

    modelMat = glm::rotate(modelMat, m_rotation, glm::vec3(0.0,0.0,1.0));
    modelMat = glm::translate(modelMat, glm::vec3(-m_spriteModel->getCenter(),0.0));
    modelMat = glm::scale(modelMat, {m_spriteModel->getSize().x,
                                     m_spriteModel->getSize().y,
                                     1.0});

    m_datum.modelMat0 = modelMat[0];
    m_datum.modelMat1 = modelMat[1];
    m_datum.modelMat2 = modelMat[2];
    m_datum.modelMat3 = modelMat[3];

    m_datum.texPos    = m_spriteModel->getTexturePosition();
    m_datum.texExtent = m_spriteModel->getTextureExtent();

    /*m_shadowDatum.texPos    = m_datum.texPos;
    m_shadowDatum.texExtent = m_datum.texExtent;*/
}


}
