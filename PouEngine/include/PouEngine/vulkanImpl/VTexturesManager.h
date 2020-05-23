#ifndef VTEXTURESMANAGER_H
#define VTEXTURESMANAGER_H

#include "PouEngine/vulkanImpl/vulkanImpl.h"
#include "PouEngine/vulkanImpl/VTexture.h"

#include <map>
#include <list>

namespace pou
{

struct VRenderableTexture
{
    VRenderableTexture() : renderTarget(nullptr){}

    VTexture                texture,
                            depthTexture;
    VFramebufferAttachment  attachment,
                            depthAttachment;
    VRenderTarget          *renderTarget;

    size_t renderingSet;
};

struct VTexture2DArrayFormat
{
    uint32_t width;
    uint32_t height;
    VkFormat vkFormat;
    bool     renderable;
    size_t   renderingSet;

    bool operator<( VTexture2DArrayFormat const& rhs ) const
    {
        if(renderable < rhs.renderable)
            return (true);
        if(renderable > rhs.renderable)
            return (false);

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

        if(renderingSet < rhs.renderingSet)
            return (true);
        if(renderingSet > rhs.renderingSet)
            return (false);

        return (false);
    }
};

struct VTexture2DArray
{
    VImage      image;
    VkImageView view;

    VkExtent2D  extent;

    std::mutex  mutex;

    std::list<size_t>   availableLayers;
};

///I need a cleaning list so that I free resource only after a full image_index*loop

class VTexturesManager : public Singleton<VTexturesManager>
{
    public:
        friend class Singleton<VTexturesManager>;
        friend class VApp;

        void update(size_t frameIndex, size_t imageIndex);

        static bool allocTexture(const VTextureFormat &format,
                                 VBuffer source, CommandPoolName cmdPoolName, VTexture *texture);
        static bool allocTexture(uint32_t width, uint32_t height,
                                 VBuffer source, CommandPoolName cmdPoolName, VTexture *texture);

        static bool allocRenderableTexture(size_t renderingSet, const VTextureFormat &format,
                                           VRenderPass *renderPass, VRenderableTexture *texture);
        static bool allocRenderableTextureWithDepth(size_t renderingSet, uint32_t width, uint32_t height, VkFormat format, VkFormat depthFormat,
                                           VRenderPass *renderPass, VRenderableTexture *texture);

        static void freeTexture(VTexture &texture);
        static void freeTexture(VRenderableTexture &texture);


        static VkSampler                sampler(bool nearest = false);
        static VkDescriptorSetLayout    descriptorSetLayout(bool withRenderableTextures = false);
        static VkDescriptorSet          descriptorSet(bool nearest = false, bool withRenderableTextures = false, size_t renderingSet = 0); //Use last frame index
        static VkDescriptorSet          descriptorSet(size_t frameIndex, bool nearest = false,
                                                      bool withRenderableTextures = false, size_t renderingSet = 0);
        //static VkDescriptorSet          imgDescriptorSet(size_t imageIndex);
        static size_t                   descriptorSetVersion(size_t frameIndex);
        //static size_t                   imgDescriptorSetVersion(size_t imageIndex);

        static VTexture getDummyTexture();

    protected:
        VTexturesManager();
        virtual ~VTexturesManager();

        bool allocRenderableTextureImpl(size_t renderingSet, uint32_t width, uint32_t height, VkFormat format,
                                           VRenderPass *renderPass, VTexture *texture, VFramebufferAttachment *attachment);

        bool    allocTextureImpl(VTexture2DArrayFormat format, VBuffer source, CommandPoolName cmdPoolName, VTexture *texture);
        bool    allocRenderableTextureImpl(VTexture2DArrayFormat format, CommandPoolName cmdPoolName, VTexture *texture);
        size_t  createTextureArray(VTexture2DArrayFormat format,  CommandPoolName cmdPoolName);

        void    freeTextureImpl(VTexture &texture, bool isRenderable = false, size_t renderingSet = 0);
        void    freeTextureImpl(VRenderableTexture &texture);
        void    updateCleaningLists(bool cleanAll = false);

        bool    createDescriptorSetLayouts();
        bool    createSampler(bool nearest = false);
        bool    createDescriptorPool(size_t framesCount, size_t imagesCount);
        bool    createDescriptorSets(size_t framesCount, size_t imagesCount);
        bool    createDummyTexture();

        void    updateDescriptorSet(size_t frameIndex);
        //void    updateImgDescriptorSet(size_t imageIndex);
        void    writeDescriptorSet(VkDescriptorSet &descSet, bool nearest = false, bool withRenderableTextures = false, size_t renderingSet = 0);

        bool    init(size_t framesCount, size_t imagesCount);
        void    cleanup();

        VkDescriptorSetLayout   getDescriptorSetLayout(bool withRenderableTextures = false);
        VkDescriptorSet         getDescriptorSet(size_t frameIndex, bool nearest = false, bool withRenderableTextures = false, size_t renderingSet = 0);
        //VkDescriptorSet         getImgDescriptorSet(size_t imageIndex);
        size_t                  getDescriptorSetVersion(size_t frameIndex);
        //size_t                  getImgDescriptorSetVersion(size_t imageIndex);



        void desactivateView(VRenderableTexture &texture);
        void resetViews();

    private:
        std::multimap<VTexture2DArrayFormat, size_t> m_formatToArray;
        std::vector<VTexture2DArray*>       m_allocatedTextureArrays;
        std::vector< std::vector<VTexture2DArray*> >  m_allocatedTextureArrays_renderable;
        std::vector<VkDescriptorImageInfo>  m_imageInfos;
        std::vector< std::vector<VkDescriptorImageInfo> >  m_imageInfos_renderable;
        VTexture m_dummyTexture;

        std::mutex m_createImageMutex;

        VkSampler                       m_sampler, m_sampler_nearest;
        std::vector<bool>               m_needToUpdateDescSet;
        //std::vector<bool>               m_needToUpdateImgDescSet;
        std::vector<size_t>             m_descSetVersion;
        //std::vector<size_t>             m_imgDescSetVersion;
        std::vector<VkDescriptorSet>    m_descriptorSets;
        std::vector<VkDescriptorSet>    m_descriptorSets_nearest;
        std::vector< std::vector<VkDescriptorSet> >   m_descriptorSets_renderable;
        std::vector< std::vector<VkDescriptorSet> >   m_descriptorSets_nearest_renderable;
        //std::vector<VkDescriptorSet>    m_imgDescriptorSets;
        VkDescriptorSetLayout           m_descriptorSetLayout;
        VkDescriptorSetLayout           m_descriptorSetLayout_renderable;
        VkDescriptorPool                m_descriptorPool;

        size_t  m_lastFrameIndex;
        size_t  m_framesCount;

        std::list<std::pair<int, VRenderTarget*> >  m_cleaningList_renderTargets;
        std::list<std::pair<int, VkImageView> >     m_cleaningList_imageViews;

    public:
        static const size_t MAX_TEXTURES_ARRAY_SIZE;
        static const size_t MAX_LAYER_PER_TEXTURE;

        static const size_t MAX_RENDERABLETEXTURES_ARRAY_SIZE; // Need to be smaller or equal than MAX_TEXTURES_ARRAY_SIZE
        static const size_t NBR_RENDERABLETEXTURES_ARRAY_SETS;
};

}

#endif // VTEXTURESMANAGER_H
