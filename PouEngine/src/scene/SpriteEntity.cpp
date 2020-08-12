#include "PouEngine/scene/SpriteEntity.h"

#include "PouEngine/assets/AssetHandler.h"
#include "PouEngine/assets/MaterialAsset.h"
#include "PouEngine/assets/TextureAsset.h"
#include "PouEngine/renderers/SceneRenderer.h"
#include "PouEngine/scene/SceneNode.h"
#include "PouEngine/scene/LightEntity.h"
#include "PouEngine/tools/MathTools.h"

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

std::array<VkVertexInputAttributeDescription, 12> SpriteDatum::getAttributeDescriptions()
{
    std::array<VkVertexInputAttributeDescription, 12> attributeDescriptions = {};

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

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(SpriteDatum, rme_color);
    ++i;

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
    attributeDescriptions[i].offset = offsetof(SpriteDatum, texRes);
    ++i;


    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32_UINT;
    attributeDescriptions[i].offset = offsetof(SpriteDatum, albedo_texId);
    ++i;

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32_UINT;
    attributeDescriptions[i].offset = offsetof(SpriteDatum, normal_texId);
    ++i;

    /*attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32_UINT;
    attributeDescriptions[i].offset = offsetof(SpriteDatum, height_texId);
    ++i;*/

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32_UINT;
    attributeDescriptions[i].offset = offsetof(SpriteDatum, rme_texId);
    ++i;

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
    m_flipX(false),
    m_flipY(false),
    m_color(1.0,1.0,1.0,1.0),
    m_rme(1.0,1.0,1.0),
    m_ordering(NOT_ORDERED),
    m_inheritRotation(true),
    m_nextSpriteElapsedTime(0),
    m_revealedAmount(0)
{
    this->updateDatum();
}

SpriteEntity::~SpriteEntity()
{
    //dtor
}

std::shared_ptr<SceneObject> SpriteEntity::createCopy()
{
    auto newObject = std::make_shared<SpriteEntity>();
    this->copyTo(newObject.get());
    return newObject;
}

void SpriteEntity::rotate(float rotation)
{
    this->setRotation(this->getRotation() + rotation);
}

void SpriteEntity::setRotation(float rotation, bool inRadians)
{
    if(!inRadians)
        rotation = rotation*glm::pi<float>()/180.0;

    if(m_rotation != rotation)
    {
        m_rotation = rotation;
        ///this->setLastUpdateTime(m_curLocalTime);
        this->updateDatum();
    }
}

void SpriteEntity::setFlip(bool x, bool y)
{
    m_flipX = x;
    m_flipY = y;
}

void SpriteEntity::flip(bool x, bool y)
{
    this->setFlip(m_flipX != x, m_flipY != y);
}

void SpriteEntity::setColor(Color color)
{
    if(m_color != color)
    {
        m_color = color;
        ///this->setLastUpdateTime(m_curLocalTime);
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

void SpriteEntity::setRme(glm::vec3 rme)
{
    if(m_rme != rme)
    {
        m_rme = rme;
        ///this->setLastUpdateTime(m_curLocalTime);
        this->updateDatum();
    }
}

bool SpriteEntity::setSpriteModel(SpriteModel* model)
{
    if(m_spriteModel != model)
    {
        this->stopListeningTo(m_spriteModel);
        m_spriteModel = model;
        /**m_lastModelUpdateTime = m_curLocalTime;
        this->setLastUpdateTime(m_curLocalTime);**/
        if(m_spriteModel != nullptr)
        {
            this->startListeningTo(m_spriteModel);
            this->setRme(m_spriteModel->getRme());
            this->setShadowCastingType(m_spriteModel->getShadowCastingType());
        }
        this->updateDatum();
        return (true);
    }
    return (false);
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

glm::vec3 SpriteEntity::getRme()
{
    return m_rme;
}

SpriteDatum SpriteEntity::getSpriteDatum()
{
    return m_datum;
}


SpriteModel* SpriteEntity::getSpriteModel()
{
    return m_spriteModel;
}

/**uint32_t SpriteEntity::getLastModelUptateTime()
{
    return m_lastModelUpdateTime;
}**/


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
    if(m_spriteModel == nullptr || m_parentNode == nullptr)
        return glm::vec2(0.0);

    if(!m_spriteModel->isReady())
        return glm::vec2(0.0);

    ///this->startListeningTo(light);

    if(light->getType() == LightType_Directional)
    {
        //VTexture shadow = m_spriteModel->getDirectionnalShadow(renderer, light->getDirection());

        glm::vec2 shadowShift = this->generateShadowDatum(light->getDirection());

        //m_shadowDatum.texId = shadow.getTexturePair();

        renderer->addToSpriteShadowsVbo(m_datum);

        return shadowShift;
    }

    return glm::vec2(0.0);
}

glm::vec2 SpriteEntity::generateShadowDatum(glm::vec3 direction)
{
    if(m_parentNode == nullptr || m_parentNode->getScene() == nullptr)
    {
        return (glm::vec2(0.0));
    }

	glm::vec3 lightDirection = normalize(direction);

	glm::vec2 lightDirectionXY = {lightDirection.x, lightDirection.y};

	glm::vec4 v = glm::vec4(lightDirectionXY / -lightDirection.z, 0.0, 0.0);

	glm::vec2 viewLightDirectionXY = v*m_parentNode->getGlobalPosition().z;

	/*
	glm::vec4 r = m_parentNode->getScene()->getViewMatrix() * v;
	//r.y        -=  m_parentNode->getScene()->getViewMatrix()[2][1];
	glm::vec2 viewLightDirectionXY = {m_datum.size.z * r.x,
                                      m_datum.size.z *(r.y  - m_parentNode->getScene()->getViewMatrix()[2][1])};*/

    /*glm::vec2 totalSize = glm::abs(viewLightDirectionXY)+glm::vec2(m_datum.size.x, m_datum.size.y);

    glm::vec2 spritePos;
    spritePos = glm::max(glm::vec2(0.0), -viewLightDirectionXY);

    m_shadowDatum.position  = m_datum.position + glm::vec3(glm::vec2(v.x,v.y)*m_datum.position.z,0.0);
    m_shadowDatum.size      = glm::vec3(totalSize, m_datum.size.z);
    m_shadowDatum.center    = m_datum.center+spritePos;*/

    return viewLightDirectionXY;
}

void SpriteEntity::update(const Time &elapsedTime, uint32_t localTime)
{
    ShadowCaster::update(elapsedTime, localTime);

    this->updateSpriteAnimation(elapsedTime);
    this->updateRevealingAnimation(elapsedTime);
}

void SpriteEntity::notify(NotificationSender *sender, int notificationType,
                          void* data)
{
    if(notificationType == NotificationType_AssetLoaded ||
       notificationType == NotificationType_TextureChanged ||
       notificationType == NotificationType_ModelChanged ||
       notificationType == NotificationType_NodeMoved)
        this->updateDatum();

    /*if(notification == NotificationType_UpdateShadow)
    {
        if(m_spriteModel != nullptr)
        {
            float* f = (float*)data;
            glm::vec3 d(*f, *(f+1), *(f+2));
            m_spriteModel->updateDirectionnalShadow(d,(dynamic_cast<LightEntity*>(sender))->getDirection());
        }
    }*/

    if(notificationType == NotificationType_SenderDestroyed)
    {
        if(sender == m_spriteModel)
        {
            m_spriteModel = nullptr;
            this->updateDatum();
        }
    }
}

/**bool SpriteEntity::syncFrom(SpriteEntity* srcEntity)
{
    //if(m_lastSyncTime > srcEntity->getLastUpdateTime() && m_lastSyncTime != (uint32_t)(-1))
      //  return (false);

    m_lastSyncTime = srcEntity->m_curLocalTime;

    return (true);
}**/


///Protected

void SpriteEntity::updateDatum()
{
    m_datum = {};
   // m_shadowDatum = {};

    if(m_spriteModel == nullptr || m_parentNode == nullptr)
        return;

    //float heightFactor = 1.0;

    m_datum.albedo_color = m_color * m_parentNode->getFinalColor() * glm::vec4(1,1,1,1.0f-m_revealedAmount);
    m_datum.rme_color = m_rme;

    m_datum.albedo_texId = {0,0};
    m_datum.normal_texId = {0,0};
    m_datum.rme_texId = {0,0};

    if(m_spriteModel->isUsingMaterial())
    {
        MaterialAsset* material = m_spriteModel->getMaterial();
        if(material != nullptr && material->isLoaded())
        {
            m_datum.albedo_texId = {material->getAlbedoMap().getTextureId(),
                                    material->getAlbedoMap().getTextureLayer()};
            m_datum.normal_texId = {material->getNormalMap().getTextureId(),
                                    material->getNormalMap().getTextureLayer()};
            m_datum.rme_texId = {material->getRmeMap().getTextureId(),
                                    material->getRmeMap().getTextureLayer()};
        }
    } else {
        if(m_spriteModel->getTexture() != nullptr)
        {
            m_datum.albedo_texId = {m_spriteModel->getTexture()->getVTexture().getTextureId(),
                                    m_spriteModel->getTexture()->getVTexture().getTextureLayer()};
        }
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
    modelMat = glm::scale(modelMat, {(m_flipX ? -1 : 1),
                                     (m_flipY ? -1 : 1),
                                     1.0});
    modelMat = glm::translate(modelMat, glm::vec3(-m_spriteModel->getCenter(),0.0));
    modelMat = glm::scale(modelMat, {m_spriteModel->getSize().x, // * (m_flipX ? -1 : 1),
                                     m_spriteModel->getSize().y, // * (m_flipY ? -1 : 1),
                                     1.0});
    /*modelMat = glm::scale(modelMat, {m_spriteModel->getSize().x * (m_flipX ? -1 : 1),
                                     m_spriteModel->getSize().y * (m_flipY ? -1 : 1),
                                     1.0});*/

    m_datum.modelMat0 = modelMat[0];
    m_datum.modelMat1 = modelMat[1];
    m_datum.modelMat2 = modelMat[2];
    m_datum.modelMat3 = modelMat[3];

    m_datum.texPos    = m_spriteModel->getTexturePosition();
    m_datum.texExtent = m_spriteModel->getTextureExtent();
    m_datum.texRes    = m_spriteModel->getTextureResolution();

    /*m_shadowDatum.texPos    = m_datum.texPos;
    m_shadowDatum.texExtent = m_datum.texExtent;*/
}


void SpriteEntity::copyTo(SpriteEntity *target)
{
    target->setRotation(m_rotation);
    target->setFlip(m_flipX, m_flipY);
    target->setColor(m_color);
    target->setRme(m_rme);
    target->setOrdering(m_ordering);
    target->setInheritRotation(m_inheritRotation);
    target->setSpriteModel(m_spriteModel);
    target->setShadowCastingType(m_shadowCastingType);
}

void SpriteEntity::updateSpriteAnimation(const Time &elapsedTime)
{
    m_nextSpriteElapsedTime += elapsedTime.count();

    if(!m_spriteModel)
        return;

    float nextSpriteDelay = m_spriteModel->getNextSpriteDelay();

    while(nextSpriteDelay != -1 && m_spriteModel->getNextSpriteModel() != nullptr
       && m_nextSpriteElapsedTime >= nextSpriteDelay)
    {
        m_nextSpriteElapsedTime -= nextSpriteDelay;
        this->setSpriteModel(m_spriteModel->getNextSpriteModel());

        if(m_spriteModel == nullptr)
            break;

        nextSpriteDelay = m_spriteModel->getNextSpriteDelay();
    }
}

void SpriteEntity::updateRevealingAnimation(const Time &elapsedTime)
{
    if(!m_spriteModel || !m_spriteModel->isRevealable())
        return;

    if(this->getOrdering() != ORDERED_BY_Z)
        this->setOrdering(ORDERED_BY_Z);

    MathTools::Box box;
    box.size = m_spriteModel->getSize();
    box.center = m_spriteModel->getCenter();

    bool shouldBeRevealed = false;

    if(!m_parentNode)
        return;

    auto scene = m_parentNode->getScene();

    if(!scene)
        return;

    auto revealingProbes = scene->getRevealingProbes();
    for(auto probe : revealingProbes)
    {
        if(MathTools::isInBox(probe->getGlobalXYPosition(), box, m_parentNode))
        {
            shouldBeRevealed = true;
            break;
        }
    }

    if(shouldBeRevealed)
    {
        if(m_revealedAmount != .9)
        {
            m_revealedAmount += elapsedTime.count();
            if(m_revealedAmount > .9)
                m_revealedAmount = .9;
            this->updateDatum();
        }
    }
    else if(m_revealedAmount != 0.0)
    {
        m_revealedAmount -= elapsedTime.count();
        if(m_revealedAmount < 0.0)
            m_revealedAmount = 0.0;
        this->updateDatum();
    }
}




}
