#include "PouEngine/scene/SpriteModel.h"

#include <Vulkan/vulkan.h>

#include "PouEngine/assets/SpriteSheetAsset.h"
#include "PouEngine/assets/TextureAsset.h"
#include "PouEngine/assets/AssetHandler.h"

#include "PouEngine/renderers/SceneRenderer.h"

namespace pou
{


/*VkVertexInputBindingDescription SpriteShadowGenerationDatum::getBindingDescription()
{
    VkVertexInputBindingDescription bindingDescription = {};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(SpriteShadowGenerationDatum);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

    return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 6> SpriteShadowGenerationDatum::getAttributeDescriptions()
{
    std::array<VkVertexInputAttributeDescription, 6> attributeDescriptions = {};

    size_t i = 0;

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(SpriteShadowGenerationDatum, direction);
    ++i;

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(SpriteShadowGenerationDatum, size);
    ++i;


    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(SpriteShadowGenerationDatum, texPos);
    ++i;

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(SpriteShadowGenerationDatum, texExtent);
    ++i;

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32_UINT;
    attributeDescriptions[i].offset = offsetof(SpriteShadowGenerationDatum, albedo_texId);
    ++i;

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32_UINT;
    attributeDescriptions[i].offset = offsetof(SpriteShadowGenerationDatum, height_texId);
    ++i;

    return attributeDescriptions;
}*/

SpriteModel::SpriteModel(SpriteSheetAsset *spriteSheet) :
    m_texture(0),
    m_spriteSheet(spriteSheet),
    m_size({1.0f,1.0f}),
    m_center({0.0f,0.0f}),
    m_texturePosition({0.0f,0.0f}),
    m_textureExtent({1.0f,1.0f}),
    m_useRelativeTextureRect(true),
    m_nextSpriteDelay(-1),
    m_nextSprite(-1),
    m_isReady(true)
   // m_shadowMapExtent(0.0,0.0)
{
    //ctor
}

SpriteModel::~SpriteModel()
{
    this->cleanup();
}

void SpriteModel::setTexture(AssetTypeId textureId)
{
    this->setTexture(TexturesHandler::instance()->getAsset(textureId));
}

void SpriteModel::setTexture(TextureAsset *texture)
{
    if(m_texture != texture)
    {
        this->stopListeningTo(m_texture);
        m_texture = texture;
        this->startListeningTo(m_texture);

        if(m_texture != nullptr && !m_texture->isLoaded())
            m_isReady = false;
        else
            m_isReady = true;

        this->sendNotification(Notification_ModelChanged);
    }
}

void SpriteModel::setSize(glm::vec2 size)
{
    if(m_size != size)
    {
        m_size = size;
        this->sendNotification(Notification_ModelChanged);
    }
}

void SpriteModel::setCenter(glm::vec2 pos)
{
    if(m_center != pos)
    {
        m_center = pos;
        this->sendNotification(Notification_ModelChanged);
    }
}

void SpriteModel::setTextureRect(glm::vec2 pos, glm::vec2 extent, bool isRelative)
{
    if(m_texturePosition != pos || m_textureExtent != extent || m_useRelativeTextureRect != isRelative)
    {
        m_texturePosition       = pos;
        m_textureExtent         = extent;
        m_useRelativeTextureRect = isRelative;
        this->sendNotification(Notification_ModelChanged);
    }
}

void SpriteModel::setNextSprite(int spriteId, float delay)
{
    if(delay <= 0 && delay != -1)
        return;

    m_nextSprite        = spriteId;
    m_nextSpriteDelay   = delay;
}

void SpriteModel::setColor(Color color)
{
    if(m_color != color)
    {
        m_color = color;
        this->sendNotification(Notification_ModelChanged);
    }
}



/*void SpriteModel::setRmt(Color rmt)
{
    if(m_rmt != rmt)
    {
        m_rmt = rmt;
        this->sendNotification(Notification_ModelChanged);
    }
}*/

/*MaterialAsset* SpriteModel::getMaterial()
{
    return m_material;
}*/

TextureAsset* SpriteModel::getTexture()
{
    return m_texture;
}

glm::vec2 SpriteModel::getSize()
{
    return m_size;
}

glm::vec2 SpriteModel::getCenter()
{
    return m_center;
}

glm::vec2 SpriteModel::getTextureResolution()
{
    return m_textureExtent;
}

glm::vec2 SpriteModel::getTextureExtent()
{
    if(!m_useRelativeTextureRect)
    {
        if(m_texture == nullptr)
            return {0,0};
        auto texExtent = m_texture->getExtent();
        if(texExtent.x == 0 || texExtent.y == 0)
            return {0,0};

        return {m_textureExtent.x/texExtent.x,
                m_textureExtent.y/texExtent.y};
    }

    return m_textureExtent;
}

glm::vec2 SpriteModel::getTexturePosition()
{
    if(!m_useRelativeTextureRect)
    {
        if(m_texture == nullptr)
            return {0,0};
        auto texExtent = m_texture->getExtent();
        if(texExtent.x == 0 || texExtent.y == 0)
            return {0,0};

        return {m_texturePosition.x/texExtent.x,
                m_texturePosition.y/texExtent.y};
    }

    return m_texturePosition;
}

bool SpriteModel::isReady()
{
    return m_isReady;
}

/*VTexture SpriteModel::getDirectionnalShadow(SceneRenderer *renderer, glm::vec3 direction)
{
    auto foundedMap = m_directionnalShadows.find(direction);
    if(foundedMap == m_directionnalShadows.end() || foundedMap->second.second == true)
        return this->generateDirectionnalShadow(renderer, direction);
    return foundedMap->second.first.texture;
}

void SpriteModel::updateDirectionnalShadow(glm::vec3 oldDirection, glm::vec3 newDirection)
{
    auto foundedMap = m_directionnalShadows.find(oldDirection);
    if(foundedMap != m_directionnalShadows.end())
    {
        VRenderableTexture buf = foundedMap->second.first;
        m_directionnalShadows.erase(foundedMap);
        m_directionnalShadows.insert({newDirection, {buf, true}});
    }
}*/

/// Protected ///

void SpriteModel::cleanup()
{
    /*for(auto &shadowMap : m_directionnalShadows)
        VTexturesManager::freeTexture(shadowMap.second.first);
    m_directionnalShadows.clear();*/
}

float SpriteModel::getNextSpriteDelay()
{
    return m_nextSpriteDelay;
}

SpriteModel* SpriteModel::getNextSpriteModel()
{
    if(!m_spriteSheet)
        return (nullptr);

    if(m_nextSprite == -1)
        return (nullptr);

    return m_spriteSheet->getSpriteModel(m_nextSprite);
}

void SpriteModel::notify(NotificationSender *sender, NotificationType notification,
                            size_t dataSize, char* data)
{
    if(notification == Notification_AssetLoaded)
    {
        m_isReady = true;
        this->sendNotification(Notification_ModelChanged);
    }
    if(notification == Notification_SenderDestroyed)
    {
        if(sender == m_texture)
        {
            m_texture = nullptr;
            this->sendNotification(Notification_ModelChanged);
        }
    }
}

/*VTexture SpriteModel::generateDirectionnalShadow(SceneRenderer *renderer, glm::vec3 direction)
{
    if(m_shadowMapExtent.x == 0.0)
        m_shadowMapExtent = this->getSize();

    bool needToCreate = false;
    auto founded = m_directionnalShadows.find(direction);
    if(founded ==  m_directionnalShadows.end())
    {
        needToCreate = true;
        founded = m_directionnalShadows.insert({direction, {VRenderableTexture{},false}}).first;
    }

    VRenderableTexture *renderableTexture = &founded->second.first;//&m_directionnalShadows[direction].first;

    if(needToCreate)
        VTexturesManager::allocRenderableTexture(m_shadowMapExtent.x, m_shadowMapExtent.y,
                                                 VK_FORMAT_R8G8B8A8_UNORM,
                                                 renderer->getSpriteShadowsRenderPass(),
                                                 renderableTexture);

    SpriteShadowGenerationDatum datum;

    datum.direction = direction;

    datum.size      = {m_size,m_material->getHeightFactor()};
    //datum.center    = m_textureCenter;

    datum.texPos    = m_texturePosition;
    datum.texExtent = m_textureExtent;

    datum.albedo_texId = m_material->getAlbedoMap().getTexturePair();
    datum.height_texId = m_material->getHeightMap().getTexturePair();


    renderer->addSpriteShadowToRender(renderableTexture->renderTarget, datum);
    founded->second.second = false;

    return renderableTexture->texture;
}*/


}
