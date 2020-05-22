#ifndef VTEXTURE_H
#define VTEXTURE_H

#include <vulkan/vulkan.h>
#include "PouEngine/Types.h"

namespace pou
{

struct VTextureFormat
{
    uint32_t width;
    uint32_t height;
    VkFormat vkFormat;

    bool operator==(const VTextureFormat &rhs) const
    {
        if(vkFormat != rhs.vkFormat)
            return (false);

        if(height != rhs.height)
            return (false);

        if(width != rhs.width)
            return (false);

        return (true);
    }

    bool operator<( VTextureFormat const& rhs ) const
    {
        if(vkFormat < rhs.vkFormat)
            return (true);
        if(vkFormat > rhs.vkFormat)
            return (false);

        if(height < rhs.height)
            return (true);
        if(height > rhs.height)
            return (false);

        if(width < rhs.width)
            return (true);
        if(width > rhs.width)
            return (false);

        return (false);
    }
};

class VTexture
{
    friend class VTexturesManager;

    public:
        VTexture();
        virtual ~VTexture();

        bool generateTexture(int texWidth, int texHeight,
                             unsigned char* pixels, CommandPoolName commandPoolName = COMMANDPOOL_SHORTLIVED);
        bool generateTexture(int texWidth, int texHeight, VkFormat format,
                             unsigned char* pixels, CommandPoolName commandPoolName = COMMANDPOOL_SHORTLIVED);

        uint32_t    getTextureId();
        uint32_t    getTextureLayer();
        glm::vec2   getTexturePair();
        VkExtent2D  getExtent();
        VkFormat    getFormat();

    protected:
        uint32_t m_textureId;
        uint32_t m_textureLayer;

        VkExtent2D  m_extent;
        VkFormat    m_format;

};

}

#endif // VTEXTURE_H
