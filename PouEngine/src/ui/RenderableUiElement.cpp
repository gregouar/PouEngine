#include "PouEngine/ui/RenderableUiElement.h"

#include "PouEngine/renderers/UiRenderer.h"

namespace pou
{


VkVertexInputBindingDescription RenderableUiDatum::getBindingDescription()
{
    VkVertexInputBindingDescription bindingDescription = {};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(RenderableUiDatum);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

    return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 8> RenderableUiDatum::getAttributeDescriptions()
{
    std::array<VkVertexInputAttributeDescription, 8> attributeDescriptions = {};

    size_t i = 0;
    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(RenderableUiDatum, modelMat0);
    ++i;

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(RenderableUiDatum, modelMat1);
    ++i;

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(RenderableUiDatum, modelMat2);
    ++i;

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(RenderableUiDatum, modelMat3);
    ++i;

    /////////////////////////////////////////////////////////////

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(RenderableUiDatum, albedo_color);
    ++i;

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(RenderableUiDatum, texPos);
    ++i;

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(RenderableUiDatum, texExtent);
    ++i;

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32_UINT;
    attributeDescriptions[i].offset = offsetof(RenderableUiDatum, albedo_texId);
    ++i;

    return attributeDescriptions;
}


RenderableUiElement::RenderableUiElement(const NodeTypeId id):
    UiElement(id),
    m_color(1.0f)
{
    //ctor
}

RenderableUiElement::~RenderableUiElement()
{
    //dtor
}

void RenderableUiElement::render(UiRenderer *renderer)
{
    this->updateDatum();
    renderer->addOrderedUiElements(m_datum,m_datum.modelMat3[2]);

    UiElement::render(renderer);
}


void RenderableUiElement::notify(NotificationSender *sender, NotificationType notification,
                             size_t dataSize, char* data)
{
    if(notification == Notification_AssetLoaded ||
       notification == Notification_TextureChanged ||
       notification == Notification_ModelChanged ||
       notification == Notification_NodeMoved)
        this->updateDatum();
}

void RenderableUiElement::updateDatum()
{
    m_datum = {};

    m_datum.albedo_color = m_color;

    /*if(m_spriteModel->getTexture() != nullptr)
    {
        m_datum.albedo_texId = {m_spriteModel->getTexture()->getVTexture().getTextureId(),
                                m_spriteModel->getTexture()->getVTexture().getTextureLayer()};
    } else */{
        m_datum.albedo_texId = {0,0};
    }

    glm::mat4 modelMat = m_modelMatrix;
    /*glm::mat4 modelMat(1.0);

    if(m_parentNode != nullptr)
        modelMat = m_parentNode->getModelMatrix() * modelMat;*/

    //modelMat = glm::rotate(modelMat, m_rotation, glm::vec3(0.0,0.0,1.0));
   // modelMat = glm::translate(modelMat, glm::vec3(-m_spriteModel->getCenter(),0.0));
    modelMat = glm::scale(modelMat, {UiElement::getSize().x,
                                     UiElement::getSize().y,
                                     1.0});

    m_datum.modelMat0 = modelMat[0];
    m_datum.modelMat1 = modelMat[1];
    m_datum.modelMat2 = modelMat[2];
    m_datum.modelMat3 = modelMat[3];

    m_datum.texPos    = {0,0};//m_spriteModel->getTexturePosition();
    m_datum.texExtent = {1,1};//m_spriteModel->getTextureExtent();
}

}