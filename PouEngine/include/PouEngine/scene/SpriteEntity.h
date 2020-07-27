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
    glm::vec3 rme_color;
    glm::vec2 texPos;
    glm::vec2 texExtent;
    glm::uvec2 texRes;
    glm::uvec2 albedo_texId;
    glm::uvec2 normal_texId;
    glm::uvec2 rme_texId;

    static VkVertexInputBindingDescription getBindingDescription();
    static std::array<VkVertexInputAttributeDescription, 12> getAttributeDescriptions();
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

        ///Should implement also copyFrom (and use it)
        virtual std::shared_ptr<SceneObject> createCopy();

        void rotate(float rotation);

        void setRotation(float rotation, bool inRadians = true);
        void flip(bool x, bool y);
        void setFlip(bool x, bool y);
        void setColor(Color color);
        void setRme(glm::vec3 rme);
        virtual bool setSpriteModel(SpriteModel* model);
        void setOrdering(SpriteOrdering ordering);
        void setInheritRotation(bool inheritRotation);

        float getRotation();
        Color getColor();
        SpriteOrdering getOrdering();
        glm::vec3 getRme();

        SpriteModel* getSpriteModel();

        ///uint32_t getLastModelUptateTime();

        SpriteDatum getSpriteDatum();
        //virtual void draw(SceneRenderer *renderer);
        virtual void generateRenderingData(SceneRenderingInstance *renderingInstance) override;
        virtual glm::vec2 castShadow(SceneRenderer *renderer, LightEntity* light) override;

        virtual void update(const Time &elapsedTime, uint32_t localTime = -1);

        virtual void notify(NotificationSender* , int notificationType,
                            void* data = nullptr) override;

        ///virtual bool syncFrom(SpriteEntity* srcEntity);

    protected:
        void cleanup();
        void updateDatum();
        glm::vec2 generateShadowDatum(glm::vec3 direction);

        void copyTo(SpriteEntity *target);

    protected:
        SpriteDatum          m_datum;
        //SpriteShadowDatum    m_shadowDatum;

        SpriteModel *m_spriteModel;
        ///uint32_t m_lastModelUpdateTime;

    private:
        float       m_rotation;
        bool        m_flipX, m_flipY;
        glm::vec4   m_color;
        glm::vec3   m_rme;
        SpriteOrdering  m_ordering;
        bool            m_inheritRotation; ///Rename this to inheritsTransformations

        float   m_nextSpriteElapsedTime;
};

}

#endif // SPRITEENTITY_H
