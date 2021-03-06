#include "PouEngine/scene/IsoSpriteEntity.h"

#include "PouEngine/assets/AssetHandler.h"
#include "PouEngine/assets/MaterialAsset.h"
#include "PouEngine/renderers/SceneRenderer.h"
#include "PouEngine/scene/SceneNode.h"
#include "PouEngine/scene/LightEntity.h"

namespace pou
{


VkVertexInputBindingDescription IsoSpriteDatum::getBindingDescription()
{
    VkVertexInputBindingDescription bindingDescription = {};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(IsoSpriteDatum);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

    return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 12> IsoSpriteDatum::getAttributeDescriptions()
{
    std::array<VkVertexInputAttributeDescription, 12> attributeDescriptions = {};

    size_t i = 0;
    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(IsoSpriteDatum, position);
    ++i;

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(IsoSpriteDatum, rotation);
    ++i;

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(IsoSpriteDatum, size);
    ++i;

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(IsoSpriteDatum, center);
    ++i;

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(IsoSpriteDatum, albedo_color);
    ++i;

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(IsoSpriteDatum, rmt_color);
    ++i;

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(IsoSpriteDatum, texPos);
    ++i;

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(IsoSpriteDatum, texExtent);
    ++i;

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32_UINT;
    attributeDescriptions[i].offset = offsetof(IsoSpriteDatum, albedo_texId);
    ++i;

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32_UINT;
    attributeDescriptions[i].offset = offsetof(IsoSpriteDatum, height_texId);
    ++i;

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32_UINT;
    attributeDescriptions[i].offset = offsetof(IsoSpriteDatum, normal_texId);
    ++i;

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32_UINT;
    attributeDescriptions[i].offset = offsetof(IsoSpriteDatum, rmt_texId);
    ++i;

    return attributeDescriptions;
}

VkVertexInputBindingDescription IsoSpriteShadowDatum::getBindingDescription()
{
    VkVertexInputBindingDescription bindingDescription = {};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(IsoSpriteShadowDatum);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

    return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 6> IsoSpriteShadowDatum::getAttributeDescriptions()
{
    std::array<VkVertexInputAttributeDescription, 6> attributeDescriptions = {};

    size_t i = 0;
    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(IsoSpriteShadowDatum, position);
    ++i;

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(IsoSpriteShadowDatum, size);
    ++i;

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(IsoSpriteShadowDatum, center);
    ++i;

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(IsoSpriteShadowDatum, texPos);
    ++i;

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(IsoSpriteShadowDatum, texExtent);
    ++i;

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32_UINT;
    attributeDescriptions[i].offset = offsetof(IsoSpriteShadowDatum, texId);
    ++i;

    return attributeDescriptions;
}


IsoSpriteEntity::IsoSpriteEntity() :
    m_spriteModel(nullptr),
    m_rotation(0.0f),
    m_color(1.0,1.0,1.0,1.0),
    m_rmt(1.0,1.0,1.0)
{
    this->updateDatum();
}

IsoSpriteEntity::~IsoSpriteEntity()
{
    //dtor
}

void IsoSpriteEntity::setRotation(float rotation)
{
    if(m_rotation != rotation)
    {
        m_rotation = rotation;
        this->updateDatum();
    }
}


void IsoSpriteEntity::setColor(Color color)
{
    if(m_color != color)
    {
        m_color = color;
        this->updateDatum();
    }
}

void IsoSpriteEntity::setRmt(glm::vec3 rmt)
{
    if(m_rmt != rmt)
    {
        m_rmt = rmt;
        this->updateDatum();
    }
}

void IsoSpriteEntity::setSpriteModel(IsoSpriteModel* model)
{
    if(m_spriteModel != model)
    {
        this->stopListeningTo(m_spriteModel);
        m_spriteModel = model;
        this->startListeningTo(m_spriteModel);
        this->updateDatum();
    }
}


float IsoSpriteEntity::getRotation()
{
    return m_rotation;
}

Color IsoSpriteEntity::getColor()
{
    return m_color;
}

glm::vec3 IsoSpriteEntity::getRmt()
{
    return m_rmt;
}

IsoSpriteDatum IsoSpriteEntity::getIsoSpriteDatum()
{
    return m_datum;
}


/*void IsoSpriteEntity::draw(SceneRenderer *renderer)
{
    if(m_spriteModel == nullptr || m_parentNode == nullptr)
        return;

    if(m_spriteModel->isReady())
        renderer->addToSpritesVbo(this->getIsoSpriteDatum());
}*/

void IsoSpriteEntity::generateRenderingData(SceneRenderingInstance *renderingInstance)
{
    if(m_spriteModel == nullptr || m_parentNode == nullptr)
        return;

    if(m_spriteModel->isReady())
        renderingInstance->addToSpritesVbo(this->getIsoSpriteDatum());
}

glm::vec2 IsoSpriteEntity::castShadow(SceneRenderer *renderer, LightEntity* light)
{
    if(m_spriteModel == nullptr || m_parentNode == nullptr)
        return glm::vec2(0.0);

    if(!m_spriteModel->isReady())
        return glm::vec2(0.0);

    this->startListeningTo(light);

    if(light->getType() == LightType_Directional)
    {
        VTexture shadow = m_spriteModel->getDirectionnalShadow(renderer, light->getDirection());

        //Since I need to generate it here, I could send it to VS
        //Also I should maybe store it somewhere instead of recomputing it each time
        glm::vec2 shadowShift = this->generateShadowDatum(light->getDirection());

        m_shadowDatum.texId = shadow.getTexturePair();
        renderer->addToSpriteShadowsVbo(m_shadowDatum/*, shadowShift*/);

        return shadowShift;
    }

    return glm::vec2(0.0);
}

glm::vec2 IsoSpriteEntity::generateShadowDatum(glm::vec3 direction)
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
}

void IsoSpriteEntity::notify(NotificationSender *sender, NotificationType notification,
                             size_t dataSize, char* data)
{
    if(notification == Notification_AssetLoaded ||
       notification == Notification_TextureChanged ||
       notification == Notification_ModelChanged ||
       notification == Notification_SceneNodeMoved)
        this->updateDatum();

    if(notification == Notification_UpdateShadow)
    {
        if(m_spriteModel != nullptr)
        {
            float* f = (float*)data;
            glm::vec3 d(*f, *(f+1), *(f+2));
            m_spriteModel->updateDirectionnalShadow(d,(dynamic_cast<LightEntity*>(sender))->getDirection());
        }
    }

    if(notification == Notification_SenderDestroyed)
    {
        if(sender == m_spriteModel)
        {
            m_spriteModel = nullptr;
            this->updateDatum();
        }
    }
}

void IsoSpriteEntity::updateDatum()
{
    m_datum = {};
    m_shadowDatum = {};

    if(m_spriteModel == nullptr || m_parentNode == nullptr)
        return;

    float heightFactor = 1.0;

    m_datum.albedo_color = m_color;
    m_datum.rmt_color = m_rmt;

    MaterialAsset* material = m_spriteModel->getMaterial();
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

        heightFactor = material->getHeightFactor();
        m_datum.rmt_color *= material->getRmtFactor();
    }

    m_datum.position = m_parentNode->getGlobalPosition();
    m_datum.size = glm::vec3(m_spriteModel->getSize(), heightFactor);
    m_datum.rotation = m_rotation;
    m_datum.center = m_spriteModel->getTextureCenter();

    m_datum.texPos    = m_spriteModel->getTexturePosition();
    m_datum.texExtent = m_spriteModel->getTextureExtent();

    m_shadowDatum.texPos    = m_datum.texPos;
    m_shadowDatum.texExtent = m_datum.texExtent;
}


}
