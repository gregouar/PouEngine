#ifndef SPRITEMODEL_H
#define SPRITEMODEL_H

#include "PouEngine/core/NotificationListener.h"
#include "PouEngine/core/NotificationSender.h"
#include "PouEngine/vulkanImpl/VTexturesManager.h"
#include "PouEngine/Types.h"

#include <vector>

namespace pou
{

class SceneRenderer;

/*struct Direction
{
    float x;
    float y;
    float z;

    Direction(glm::vec3 v){x=v.x, y=v.y, z=v.z;}

    bool operator<( Direction const& rhs ) const
    {
        if(x < rhs.x)
            return (true);
        if(x > rhs.x)
            return (false);

        if(y < rhs.y)
            return (true);
        if(y > rhs.y)
            return (false);

        if(z < rhs.z)
            return (true);
        if(z > rhs.z)
            return (false);
        return (false);
    }
};

struct SpriteShadowGenerationDatum
{
    glm::vec3 direction;
    glm::vec3 size;
    //glm::vec2 center;

    glm::vec2 texPos;
    glm::vec2 texExtent;

    glm::uvec2 albedo_texId;
    glm::uvec2 height_texId;
    //glm::uvec2 normal_texId;
    //glm::uvec2 rmt_texId;

    static VkVertexInputBindingDescription getBindingDescription();
    static std::array<VkVertexInputAttributeDescription, 6> getAttributeDescriptions();
};*/

class SpriteSheetAsset;

class SpriteModel : public NotificationListener, public NotificationSender
{
    friend class SpriteEntity;

    public:
        SpriteModel(SpriteSheetAsset *spriteSheet);
        virtual ~SpriteModel();

        SpriteModel( const SpriteModel& ) = delete;
        SpriteModel& operator=( const SpriteModel& ) = delete;


        /*void setMaterial(AssetTypeId materialId);
        void setMaterial(MaterialAsset *material);*/
        void setTexture(AssetTypeId textureId);
        void setTexture(TextureAsset *texture);
        void setSize(glm::vec2 size);
        void setCenter(glm::vec2 pos);
        void setTextureRect(glm::vec2 pos, glm::vec2 extent, bool isRelative = true);
        void setNextSprite(int spriteId, float delay);


        void setColor(Color color);
        //void setRmt(Color rmt);

        //void setShadowMapExtent(glm::vec2 extent);

        //MaterialAsset *getMaterial();
        TextureAsset *getTexture();
        glm::vec2 getSize();
        glm::vec2 getCenter();
        glm::vec2 getTextureResolution();
        glm::vec2 getTextureExtent();
        glm::vec2 getTexturePosition();
        bool      isReady();

        float           getNextSpriteDelay();
        SpriteModel*    getNextSpriteModel();

        /*VTexture getDirectionnalShadow(SceneRenderer *renderer, glm::vec3 direction);
        void updateDirectionnalShadow(glm::vec3 oldDirection, glm::vec3 newDirection);
        void deleteDirectionnalShadow(glm::vec3 direction);*/

        virtual void notify(NotificationSender* , NotificationType,
                            size_t dataSize = 0, char* data = nullptr) override;

    protected:
        void cleanup();

        //Add some kind of cleaning ?
        //VTexture generateDirectionnalShadow(SceneRenderer *renderer, glm::vec3 direction);

    private:
        //MaterialAsset *m_material;
        TextureAsset *m_texture;
        std::vector<bool>   m_needToCheckLoading;

        SpriteSheetAsset *m_spriteSheet;

        glm::vec2 m_size;
        glm::vec2 m_center;
        glm::vec2 m_texturePosition;
        glm::vec2 m_textureExtent;
        bool      m_useRelativeTextureRect;

        float   m_nextSpriteDelay;
        int     m_nextSprite;

        glm::vec4 m_color;
        //glm::vec4 m_rmt;

        bool m_isReady;

        /*glm::vec2 m_shadowMapExtent;
        std::map<Direction, std::pair<VRenderableTexture, bool> > m_directionnalShadows;*/
};

}

#endif // SPRITEMODEL_H
