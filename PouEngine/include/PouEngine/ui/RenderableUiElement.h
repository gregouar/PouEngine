#ifndef RENDERABLEUIELEMENT_H
#define RENDERABLEUIELEMENT_H

#include "PouEngine/ui/UiElement.h"

#include "PouEngine/vulkanImpl/vulkanImpl.h"

namespace pou
{

class UiElement;

struct RenderableUiDatum
{
    glm::vec4 modelMat0;
    glm::vec4 modelMat1;
    glm::vec4 modelMat2;
    glm::vec4 modelMat3;

    glm::vec4 albedo_color;
    glm::vec2 texPos;
    glm::vec2 texExtent;
    glm::uvec2 albedo_texId;

    static VkVertexInputBindingDescription getBindingDescription();
    static std::array<VkVertexInputAttributeDescription, 8> getAttributeDescriptions();
};

class RenderableUiElement : public UiElement
{
    public:
        RenderableUiElement(const NodeTypeId, UserInterface *interface);
        virtual ~RenderableUiElement();

        void setColor(const glm::vec4 &c);

        virtual void render(UiRenderer *renderer);

        virtual void notify(NotificationSender* , NotificationType,
                            size_t dataSize = 0, char* data = nullptr) override;


    protected:
        virtual void updateDatum();

    protected:
        RenderableUiDatum   m_datum;

    private:
        glm::vec4           m_color;
};

}

#endif // RENDERABLEUIELEMENT_H
