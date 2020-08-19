#ifndef SPRITESHEETASSET_H
#define SPRITESHEETASSET_H

#include <memory>

#include "PouEngine/assets/Asset.h"
#include "PouEngine/core/NotificationListener.h"
#include "PouEngine/scene/SpriteModel.h"
#include "PouEngine/vulkanImpl/VTexture.h"

#include "tinyxml/tinyxml.h"

namespace pou
{

class SpriteSheetAsset : public Asset, public NotificationListener
{
    public:
        SpriteSheetAsset();
        SpriteSheetAsset(const AssetTypeId);
        virtual ~SpriteSheetAsset();

        bool loadFromFile(const std::string &filePath);


        SpriteModel* getSpriteModel(HashedString spriteName);
        ///SpriteModel* getSpriteModel(int spriteId);

        ///int getSpriteId(const std::string &spriteName) const;

        virtual void notify(NotificationSender* , int notificationType,
                            void* data = nullptr) override;


    protected:
        bool loadFromXML(TiXmlHandle *);

        ///int generateSpriteId(const std::string &spriteName);

    private:
        TextureAsset    *m_texture;
        MaterialAsset   *m_material;
        bool m_useMaterial;

        glm::vec2     m_textureScale;

        bool m_waitingForTextureLoading;

        /**std::unordered_map<HashedString, int>                   m_spritesIdByName;
        std::unordered_map<int, std::unique_ptr<SpriteModel> >  m_spritesById;**/
        std::unordered_map<HashedString, std::unique_ptr<SpriteModel> >  m_spritesByName;
};

}

#endif // SPRITESHEETASSET_H
