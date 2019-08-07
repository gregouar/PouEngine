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


        virtual void notify(NotificationSender* , NotificationType,
                            size_t dataSize = 0, char* data = nullptr) override;


    protected:
        bool loadFromXML(TiXmlHandle *);

    private:
        TextureAsset *m_texture;
        std::map<std::string, std::unique_ptr<SpriteModel> > m_sprites;
};

}

#endif // SPRITESHEETASSET_H
