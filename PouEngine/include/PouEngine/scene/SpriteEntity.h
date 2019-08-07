#ifndef SPRITEENTITY_H
#define SPRITEENTITY_H

#include "PouEngine/scene/SceneEntity.h"
#include "PouEngine/scene/ShadowCaster.h"
#include "PouEngine/scene/SpriteModel.h"

#include "PouEngine/vulkanImpl/vulkanImpl.h"

namespace pou
{

enum SpriteOrdering
{
    NOT_ORDERED,
    ORDERED_BY_Z,
    TOTAL_SPRITE_ORDERINGS,
};

struct SpriteDatum
{
    /*glm::vec3 position;
    float rotation;
    glm::vec2 size;
    glm::vec2 center;*/
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

/*struct SpriteShadowDatum
{
    glm::vec3 position;
    glm::vec3 size;
    glm::vec2 center;

    glm::vec2 texPos;
    glm::vec2 texExtent;

    glm::uvec2 texId;

    static VkVertexInputBindingDescription getBindingDescription();
    static std::array<VkVertexInputAttributeDescription, 6> getAttributeDescriptions();
};*/


class SpriteEntity : public ShadowCaster
{
    //friend class SceneRenderer;
    friend class SceneNode;

    public:
        SpriteEntity();
        virtual ~SpriteEntity();

        void rotate(float rotation);

        void setRotation(float rotation);
        void setColor(Color color);
        //void setRmt(glm::vec3 rmt);
        void setSpriteModel(SpriteModel* model);
        void setOrdering(SpriteOrdering ordering);
        void setInheritRotation(bool inheritRotation);

        float getRotation();
        Color getColor();
        SpriteOrdering getOrdering();
        //glm::vec3 getRmt();

        SpriteDatum getSpriteDatum();
        //virtual void draw(SceneRenderer *renderer);
        virtual void generateRenderingData(SceneRenderingInstance *renderingInstance) override;
        virtual glm::vec2 castShadow(SceneRenderer *renderer, LightEntity* light) override;

        virtual void notify(NotificationSender* , NotificationType,
                            size_t dataSize = 0, char* data = nullptr) override;

    protected:
        void cleanup();
        void updateDatum();
        //glm::vec2 generateShadowDatum(glm::vec3 direction);

    protected:
        SpriteDatum          m_datum;
        //SpriteShadowDatum    m_shadowDatum;

        SpriteModel *m_spriteModel;

    private:
        float       m_rotation;
        glm::vec4   m_color;
        SpriteOrdering  m_ordering;
        bool            m_inheritRotation;
};

}

#endif // SPRITEENTITY_H
