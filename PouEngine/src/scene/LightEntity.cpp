#include "PouEngine/scene/LightEntity.h"

#include "PouEngine/renderers/SceneRenderer.h"

namespace pou
{


VkVertexInputBindingDescription LightDatum::getBindingDescription()
{
    VkVertexInputBindingDescription bindingDescription = {};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(LightDatum);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

    return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 6> LightDatum::getAttributeDescriptions()
{
    std::array<VkVertexInputAttributeDescription, 6> attributeDescriptions = {};

    size_t i = 0;
    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(LightDatum, position);
    ++i;

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(LightDatum, color);
    ++i;

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(LightDatum, radius);
    ++i;

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32_UINT;
    attributeDescriptions[i].offset = offsetof(LightDatum, shadowMap);
    ++i;

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32_UINT;
    attributeDescriptions[i].offset = offsetof(LightDatum, squaredShadowMap);
    ++i;

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(LightDatum, shadowShift);
    ++i;

    return attributeDescriptions;
}

LightModel::LightModel() :
    type(LightType_Omni),
    direction(0.0,0.0,-1.0),
    color(1.0,1.0,1.0,1.0),
    radius(100.0),
    intensity(1.0),
    castShadow(false),
    shadowMapExtent(512.0, 512.0),
    shadowBlurRadius(4.0)
{

}

LightEntity::LightEntity() : SceneEntity()
    /*m_type(LightType_Omni),
    m_direction(0.0,0.0,-1.0),
    m_color(1.0,1.0,1.0,1.0),
    m_radius(100.0),
    m_intensity(1.0),
    m_castShadow(false),
    m_shadowMapExtent(512.0, 512.0)*/
{
    m_isALight = true;

    /*m_lightModel.type = LightType_Omni;
    m_lightModel.direction = {0.0,0.0,-1.0};
    m_lightModel.color = {1.0,1.0,1.0,1.0};
    m_lightModel.radius = 100.0;
    m_lightModel.intensity = 1.0;
    m_lightModel.castShadow = false;
    m_lightModel.shadowMapExtent = {512.0, 512.0};*/

    m_datum.shadowShift = {0,0};
    this->updateDatum();
}

LightEntity::~LightEntity()
{
    VTexturesManager::freeTexture(m_shadowMap);
}

/*void LightEntity::draw(SceneRenderer *renderer)
{
    renderer->addToLightsVbo(this->getLightDatum());
}*/


void LightEntity::generateRenderingData(SceneRenderingInstance *renderingInstance)
{
    renderingInstance->addToLightsVbo(this->getLightDatum());
}

void LightEntity::generateShadowMap(SceneRenderer* renderer, std::list<ShadowCaster*> &shadowCastersList)
{
    if(!(m_shadowMap.attachment.extent.width  == m_lightModel.shadowMapExtent.x
      && m_shadowMap.attachment.extent.height == m_lightModel.shadowMapExtent.y))
        this->recreateShadowMap(renderer);

    renderer->addShadowMapToRender(m_shadowMap.renderTarget, m_datum);
    m_datum.shadowShift = {0,0};

    for(auto shadowCaster : shadowCastersList)
    {
        glm::vec2 shadowShift = shadowCaster->castShadow(renderer, this);
        if(glm::abs(shadowShift.x) > glm::abs(m_datum.shadowShift.x))
            m_datum.shadowShift.x = shadowShift.x;
        if(glm::abs(shadowShift.y) > glm::abs(m_datum.shadowShift.y))
            m_datum.shadowShift.y = shadowShift.y;
    }

    ///I don't really need to return this...
   // return m_shadowMap.texture;
}

LightType LightEntity::getType()
{
    return m_lightModel.type;
}

glm::vec3 LightEntity::getDirection()
{
    return m_lightModel.direction;
}

Color LightEntity::getDiffuseColor()
{
    return m_lightModel.color;
}

float LightEntity::getRadius()
{
    return m_lightModel.radius;
}

float LightEntity::getIntensity()
{
    return m_lightModel.intensity;
}

bool LightEntity::isCastingShadows()
{
    return m_lightModel.castShadow;
}

void LightEntity::setType(LightType type)
{
    if(m_lightModel.type != type)
    {
        m_lightModel.type = type;
        this->updateDatum();
    }
}

void LightEntity::setDirection(glm::vec3 direction)
{
    if(m_lightModel.direction != direction)
    {
        glm::vec3 oldDirection = m_lightModel.direction;
        m_lightModel.direction = direction;
        this->updateDatum();

        this->sendNotification(Notification_UpdateShadow, sizeof(oldDirection), (char*)(&oldDirection));
    }
}

void LightEntity::setDiffuseColor(glm::vec3 color)
{
    this->setDiffuseColor(glm::vec4(color,1.0));
}

void LightEntity::setDiffuseColor(Color color)
{
    if(m_lightModel.color != color)
    {
        m_lightModel.color = color;
        this->updateDatum();
    }
}

void LightEntity::setRadius(float radius)
{
    if(m_lightModel.radius != radius)
    {
        m_lightModel.radius = radius;
        this->updateDatum();
    }
}

void LightEntity::setIntensity(float intensity)
{
    if(m_lightModel.intensity != intensity)
    {
        m_lightModel.intensity = intensity;
        this->updateDatum();
    }
}

void LightEntity::setModel(const LightModel &lightModel)
{
    //if(m_lightModel != lightModel)
    {
        m_lightModel = lightModel;
        this->updateDatum();
    }
}

void LightEntity::setShadowMapExtent(glm::vec2 extent)
{
    m_lightModel.shadowMapExtent = extent;
}

void LightEntity::setShadowBlurRadius(float blurRadius)
{
    m_lightModel.shadowBlurRadius = blurRadius;
    this->updateDatum();
}

void LightEntity::enableShadowCasting()
{
    if(!m_lightModel.castShadow)
    {
        m_lightModel.castShadow = true;
        this->updateDatum();
    }
}

void LightEntity::disableShadowCasting()
{
    if(m_lightModel.castShadow)
    {
        m_lightModel.castShadow = false;
        this->updateDatum();
    }
}


LightDatum LightEntity::getLightDatum()
{
    return m_datum;
}

/// Protected ///
void LightEntity::notify(NotificationSender *sender, NotificationType type,
                         size_t dataSize, char* data)
{
    if(type == Notification_NodeMoved)
        this->updateDatum();
}

void LightEntity::updateDatum()
{
    if(m_lightModel.type == LightType_Directional)
        m_datum.position = glm::vec4(m_lightModel.direction,0.0);
    else if(m_parentNode != nullptr)
        m_datum.position = glm::vec4(m_parentNode->getGlobalPosition(),1.0);

    m_datum.color     = m_lightModel.color;
    m_datum.color.a  *= m_lightModel.intensity;

    m_datum.radius    = m_lightModel.radius;

    if(m_lightModel.castShadow)
    {
        m_datum.shadowMap = {m_shadowMap.depthTexture.getTextureId(),
                             m_shadowMap.depthTexture.getTextureLayer()};
        m_datum.squaredShadowMap = {m_shadowMap.texture.getTextureId(),
                                    m_shadowMap.texture.getTextureLayer()};
    }
    else
    {
        m_datum.shadowMap = {0,0};
        m_datum.squaredShadowMap = {0,0};
    }

    m_datum.shadowBlurRadius = m_lightModel.shadowBlurRadius;
}

void LightEntity::recreateShadowMap(SceneRenderer* renderer)
{
   // VTexturesManager::freeTexture(m_shadowMap);
    //VTexturesManager::allocRenderableTexture(m_lightModel.shadowMapExtent.x, m_lightModel.shadowMapExtent.y, VK_FORMAT_D16_UNORM /*VK_FORMAT_D32_SFLOAT*//*VK_FORMAT_D24_UNORM_S8_UINT*/,
    //                                         renderer->getShadowMapsRenderPass(), &m_shadowMap);

    VTexturesManager::freeTexture(m_shadowMap);
    VTexturesManager::allocRenderableTextureWithDepth(m_lightModel.shadowMapExtent.x, m_lightModel.shadowMapExtent.y,
                                                      VK_FORMAT_R32G32_SFLOAT, VK_FORMAT_D16_UNORM,
                                                      renderer->getShadowMapsRenderPass(), &m_shadowMap);
   // VTexturesManager::allocRenderableTexture(m_lightModel.shadowMapExtent.x, m_lightModel.shadowMapExtent.y, VK_FORMAT_D16_UNORM /*VK_FORMAT_R32_SFLOAT*/,
   //                                          renderer->getShadowMapsRenderPass(), &m_shadowMap);

    this->updateDatum();
}


}
