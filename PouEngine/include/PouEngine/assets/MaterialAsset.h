#ifndef MATERIALASSET_H
#define MATERIALASSET_H

#include "PouEngine/assets/Asset.h"
#include "PouEngine/core/NotificationListener.h"
#include "PouEngine/vulkanImpl/VTexture.h"

#include "tinyxml/tinyxml.h"
#include "glm/glm.hpp"

namespace pou
{

class MaterialAsset : public Asset, public NotificationListener
{
    public:
        MaterialAsset();
        MaterialAsset(const AssetTypeId);
        virtual ~MaterialAsset();

        bool loadFromFile(const std::string &filePath);

        virtual void notify(NotificationSender* , NotificationType,
                            size_t dataSize = 0, char* data = nullptr) override;

        VTexture getAlbedoMap();
        VTexture getNormalMap();
        VTexture getHeightMap();
        VTexture getRmeMap();

        float getHeightFactor();
        glm::vec3 getRmeFactor();
        glm::vec2 getExtent();

    protected:
        bool loadFromXML(TiXmlHandle *);

    private:
        TextureAsset* m_albedoMap; //Color
        TextureAsset* m_normalMap;
        TextureAsset* m_heightMap;
        TextureAsset* m_rmeMap; // Roughness-Metalness-Emissivity

        float       m_heightFactor;
        glm::vec3   m_rmeFactor;
};

}

#endif // MATERIALASSET_H
