#include "PouEngine/scene/LightEntity.h"

#include "PouEngine/renderers/SceneRenderer.h"
#include "PouEngine/tools/Parser.h"

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

std::array<VkVertexInputAttributeDescription, 5> LightDatum::getAttributeDescriptions()
{
    std::array<VkVertexInputAttributeDescription, 5> attributeDescriptions = {};

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
    attributeDescriptions[i].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(LightDatum, shadowShift);
    ++i;

    return attributeDescriptions;
}

///
///LightModel
///

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


bool LightModel::loadFromXML(TiXmlElement *element)
{
    auto att = element->Attribute("type");
    if(att != nullptr)
    {
        if(std::string(att) == "omni")
            type = pou::LightType_Omni;
        else if(std::string(att) == "directional")
            type = pou::LightType_Directional;
        else if(std::string(att) == "spot")
            type = pou::LightType_Spot;
    }

    att = element->Attribute("radius");
    if(att != nullptr)
        radius = pou::Parser::parseFloat(att);

    att = element->Attribute("intensity");
    if(att != nullptr)
        intensity = pou::Parser::parseFloat(att);

    att = element->Attribute("castShadow");
    if(att != nullptr)
        castShadow = pou::Parser::parseBool(att);

    auto colorChild = element->FirstChildElement("color");
    if(colorChild != nullptr)
    {
        auto colorElement = colorChild->ToElement();
        att = colorElement->Attribute("r");
        if(att != nullptr)
            color.r = pou::Parser::parseFloat(att);
        att = colorElement->Attribute("g");
        if(att != nullptr)
            color.g = pou::Parser::parseFloat(att);
        att = colorElement->Attribute("b");
        if(att != nullptr)
            color.b = pou::Parser::parseFloat(att);
        att = colorElement->Attribute("a");
        if(att != nullptr)
            color.a = pou::Parser::parseFloat(att);
        att = colorElement->Attribute("red");
        if(att != nullptr)
            color.r = pou::Parser::parseFloat(att);
        att = colorElement->Attribute("green");
        if(att != nullptr)
            color.g = pou::Parser::parseFloat(att);
        att = colorElement->Attribute("blue");
        if(att != nullptr)
            color.b = pou::Parser::parseFloat(att);
        att = colorElement->Attribute("alpha");
        if(att != nullptr)
            color.a = pou::Parser::parseFloat(att);
    }

    auto directionChild = element->FirstChildElement("direction");
    if(directionChild != nullptr)
    {
        auto directionElement = directionChild->ToElement();
        att = directionElement->Attribute("x");
        if(att != nullptr)
            direction.x = pou::Parser::parseFloat(att);
        att = directionElement->Attribute("y");
        if(att != nullptr)
            direction.y = pou::Parser::parseFloat(att);
        att = directionElement->Attribute("z");
        if(att != nullptr)
            direction.z = pou::Parser::parseFloat(att);
    }

    return (true);
}


///
///LightEntity
///

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

std::shared_ptr<SceneObject> LightEntity::createCopy()
{
    auto newObject = std::make_shared<LightEntity>();
    newObject->setModel(m_lightModel);
    return newObject;
}

/*void LightEntity::draw(SceneRenderer *renderer)
{
    renderer->addToLightsVbo(this->getLightDatum());
}*/


void LightEntity::generateRenderingData(SceneRenderingInstance *renderingInstance)
{
    renderingInstance->addToLightsVbo(this->getLightDatum());
}

void LightEntity::generateShadowMap(SceneRenderer* renderer, std::vector<ShadowCaster*> &shadowCastersList)
{
    if(!(m_shadowMap.attachment.extent.width  == m_lightModel.shadowMapExtent.x
      && m_shadowMap.attachment.extent.height == m_lightModel.shadowMapExtent.y))
        this->recreateShadowMap(renderer);

    renderer->addShadowMapToRender(m_shadowMap.renderTarget, m_datum);
    m_datum.shadowShift = {0,0};

    for(auto &shadowCaster : shadowCastersList)
    {
        glm::vec2 shadowShift = shadowCaster->castShadow(renderer, this);
        if(glm::abs(shadowShift.x) > glm::abs(m_datum.shadowShift.x))
            m_datum.shadowShift.x = shadowShift.x;
        if(glm::abs(shadowShift.y) > glm::abs(m_datum.shadowShift.y))
            m_datum.shadowShift.y = shadowShift.y;
    }
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
        ///this->setLastUpdateTime(m_curLocalTime);
        this->updateDatum();
    }
}

void LightEntity::setDirection(glm::vec3 direction)
{
    if(m_lightModel.direction != direction)
    {
        //glm::vec3 oldDirection = m_lightModel.direction;
        m_lightModel.direction = direction;
        ///this->setLastUpdateTime(m_curLocalTime);
        this->updateDatum();

        //this->sendNotification(NotificationType_UpdateShadow, &oldDirection);
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
        ///this->setLastUpdateTime(m_curLocalTime);
        this->updateDatum();
    }
}

void LightEntity::setRadius(float radius)
{
    if(m_lightModel.radius != radius)
    {
        m_lightModel.radius = radius;
        ///this->setLastUpdateTime(m_curLocalTime);
        this->updateDatum();
    }
}

void LightEntity::setIntensity(float intensity)
{
    if(m_lightModel.intensity != intensity)
    {
        m_lightModel.intensity = intensity;
        ///this->setLastUpdateTime(m_curLocalTime);
        this->updateDatum();
    }
}

void LightEntity::setModel(const LightModel &lightModel)
{
    //if(m_lightModel != lightModel)
    {
        m_lightModel = lightModel;
        ///this->setLastUpdateTime(m_curLocalTime);
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
void LightEntity::notify(NotificationSender *sender, int notificationType,
                         void* data)
{
    if(notificationType == NotificationType_NodeMoved)
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
        m_datum.shadowMap = {m_shadowMap.texture.getTextureId(),
                             m_shadowMap.texture.getTextureLayer()};
    }
    else
    {
        m_datum.shadowMap = {0,0};
    }

    m_datum.shadowBlurRadius = m_lightModel.shadowBlurRadius;
}

void LightEntity::recreateShadowMap(SceneRenderer* renderer)
{
   // VTexturesManager::freeTexture(m_shadowMap);
    //VTexturesManager::allocRenderableTexture(m_lightModel.shadowMapExtent.x, m_lightModel.shadowMapExtent.y, VK_FORMAT_D16_UNORM /*VK_FORMAT_D32_SFLOAT*//*VK_FORMAT_D24_UNORM_S8_UINT*/,
    //                                         renderer->getShadowMapsRenderPass(), &m_shadowMap);

    VTexturesManager::freeTexture(m_shadowMap);
    VTexturesManager::allocRenderableTextureWithDepth(0, m_lightModel.shadowMapExtent.x, m_lightModel.shadowMapExtent.y,
                                                      VK_FORMAT_R32G32_SFLOAT, VK_FORMAT_D16_UNORM,
                                                      renderer->getShadowMapsRenderPass(), &m_shadowMap);
   // VTexturesManager::allocRenderableTexture(m_lightModel.shadowMapExtent.x, m_lightModel.shadowMapExtent.y, VK_FORMAT_D16_UNORM /*VK_FORMAT_R32_SFLOAT*/,
   //                                          renderer->getShadowMapsRenderPass(), &m_shadowMap);

    this->updateDatum();
}


}
