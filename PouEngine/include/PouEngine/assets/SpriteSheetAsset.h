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


        SpriteModel* getSpriteModel(const std::string &spriteName);
        SpriteModel* getSpriteModel(int spriteId);

        int getSpriteId(const std::string &spriteName) const;

        virtual void notify(NotificationSender* , NotificationType,
                            size_t dataSize = 0, char* data = nullptr) override;


    protected:
        bool loadFromXML(TiXmlHandle *);

        int generateSpriteId(const std::string &spriteName);

    private:
        TextureAsset    *m_texture;
        MaterialAsset   *m_material;
        bool m_useMaterial;

        glm::vec2     m_textureScale;

        bool m_waitingForTextureLoading;

        std::map<std::string, int>                      m_spritesIdByName;
        std::map<int, std::unique_ptr<SpriteModel> >    m_spritesById;

};

}

#endif // SPRITESHEETASSET_H
