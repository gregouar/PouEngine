#ifndef TEXTUREASSET_H
#define TEXTUREASSET_H

//#define GLFW_INCLUDE_VULKAN
//#include <GLFW/glfw3.h>

//#include <glm/glm.hpp>
//#include <vulkan/vulkan.h>

#include "PouEngine/vulkanImpl/vulkanImpl.h"
#include "PouEngine/vulkanImpl/VTexture.h"

#include "PouEngine/assets/Asset.h"

namespace pou
{

class VInstance;

class TextureAsset : public Asset
{
    friend class VApp;

    public:
        TextureAsset();
        TextureAsset(const AssetTypeId);
        virtual ~TextureAsset();

        bool loadFromFile(const std::string &filePath);
        bool loadFromMemory(void* data, std::size_t dataSize);

        VTexture getVTexture();
        glm::vec2 getExtent();

        void writeTexture(const std::vector<uint8_t> &buffer);

        static TextureAsset *generateTexture(size_t width, size_t height, const std::vector<uint8_t> &buffer);

    protected:
        VTexture m_vtexture;
};

}

#endif // TEXTUREASSET_H
