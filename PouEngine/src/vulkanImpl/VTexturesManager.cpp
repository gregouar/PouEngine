#include "PouEngine/vulkanImpl/VTexturesManager.h"

#include "PouEngine/core/VApp.h"
#include "PouEngine/vulkanImpl/VTexture.h"

#include "PouEngine/core/Config.h"
#include "PouEngine/tools/Logger.h"

namespace pou
{

/// I should turn this into app parameters
const size_t VTexturesManager::MAX_LAYER_PER_TEXTURE = 16; //Number of layers in each texture2DArray
const size_t VTexturesManager::MAX_TEXTURES_ARRAY_SIZE = 128; //Number of texture2DArray

const size_t VTexturesManager::MAX_RENDERABLETEXTURES_ARRAY_SIZE = 32; //Number of texture2DArray of renderable targets
const size_t VTexturesManager::NBR_RENDERABLETEXTURES_ARRAY_SETS = 2; //Number of arrays of renderable textures for ping pong etc

VTexturesManager::VTexturesManager() :
    m_lastFrameIndex(0)
{
    m_allocatedTextureArrays_renderable.resize(NBR_RENDERABLETEXTURES_ARRAY_SETS);
    m_descriptorSets_renderable.resize(NBR_RENDERABLETEXTURES_ARRAY_SETS);
    m_descriptorSets_nearest_renderable.resize(NBR_RENDERABLETEXTURES_ARRAY_SETS);
    m_imageInfos_renderable.resize(NBR_RENDERABLETEXTURES_ARRAY_SETS);
}

VTexturesManager::~VTexturesManager()
{
    this->cleanup();
}

bool VTexturesManager::allocTexture(uint32_t width, uint32_t height,
                                    VBuffer source, CommandPoolName cmdPoolName, VTexture *texture)
{
    return VTexturesManager::instance()->allocTextureImpl({width, height, VK_FORMAT_R8G8B8A8_UNORM, false,0},
                                                          source, cmdPoolName, texture);
}

bool VTexturesManager::allocTexture(const VTextureFormat &format,
                                    VBuffer source, CommandPoolName cmdPoolName, VTexture *texture)
{
    return VTexturesManager::instance()->allocTextureImpl({format.width, format.height, format.vkFormat, false,0},
                                                          source, cmdPoolName, texture);
}


bool VTexturesManager::allocRenderableTexture(size_t renderingSet, const VTextureFormat &format,
                                     VRenderPass *renderPass, VRenderableTexture *texture)
{
    texture->renderingSet = renderingSet;
    texture->renderTarget = new VRenderTarget();

    if(!VTexturesManager::instance()->allocRenderableTextureImpl(renderingSet, format.width, format.height, format.vkFormat,  renderPass,
                                                                 &texture->texture, &texture->attachment))
    {
        delete texture->renderTarget;
        return (false);
    }

    texture->renderTarget->addAttachments({texture->attachment});

    texture->renderTarget->init(1,renderPass);

    return (true);
}

bool VTexturesManager::allocRenderableTextureWithDepth(size_t renderingSet, uint32_t width, uint32_t height, VkFormat format, VkFormat depthFormat,
                                     VRenderPass *renderPass, VRenderableTexture *texture)
{
    texture->renderingSet = renderingSet;
    texture->renderTarget = new VRenderTarget();

    if(!VTexturesManager::instance()->allocRenderableTextureImpl(renderingSet, width, height, format,  renderPass,
                                                                 &texture->texture, &texture->attachment))
    {
        delete texture->renderTarget;
        return (false);
    }

    texture->renderTarget->addAttachments({texture->attachment});

    if(!VTexturesManager::instance()->allocRenderableTextureImpl(renderingSet, width, height, depthFormat,  renderPass,
                                                                 &texture->depthTexture, &texture->depthAttachment))
    {
        delete texture->renderTarget;
        return (false);
    }

    texture->renderTarget->addAttachments({texture->depthAttachment});

    texture->renderTarget->init(1,renderPass);

    return (true);
}


bool VTexturesManager::allocRenderableTextureImpl(size_t renderingSet, uint32_t width, uint32_t height, VkFormat format,
                                              VRenderPass *renderPass, VTexture *texture, VFramebufferAttachment *attachment)
{
    if(!this->allocTextureImpl({width, height, format, true, renderingSet},
                                VBuffer{}, COMMANDPOOL_SHORTLIVED, texture))
        return (false);

    VkImageAspectFlags aspect;
    if(format == VK_FORMAT_D24_UNORM_S8_UINT)
        aspect = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
    else if(format == VK_FORMAT_D32_SFLOAT || format == VK_FORMAT_D16_UNORM )
        aspect = VK_IMAGE_ASPECT_DEPTH_BIT;
    else
        aspect = VK_IMAGE_ASPECT_COLOR_BIT;

    attachment->image =
            this->m_allocatedTextureArrays_renderable[renderingSet][texture->getTextureId()]->image;
    attachment->view  =
            VulkanHelpers::createImageView( attachment->image.vkImage, format,
                                            aspect, 1, 1, texture->getTextureLayer(), 0);
    attachment->mipViews.push_back(attachment->view);

    if(attachment->view == VK_NULL_HANDLE)
        return (false);

    attachment->extent.width  = width;
    attachment->extent.height = height;
    attachment->type.format = format;
    attachment->type.layout = (format == VK_FORMAT_D24_UNORM_S8_UINT || format == VK_FORMAT_D32_SFLOAT || format == VK_FORMAT_D16_UNORM ) ?
                                //VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                                VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    return (true);
}


bool VTexturesManager::allocTextureImpl(VTexture2DArrayFormat format,
                                        VBuffer source, CommandPoolName cmdPoolName, VTexture *texture)
{
    m_createImageMutex.lock();
    auto foundedArrays = m_formatToArray.equal_range(format);
    size_t chosenArray = MAX_TEXTURES_ARRAY_SIZE;

    for(auto ar = foundedArrays.first ; ar != foundedArrays.second ; ++ar)
    {
        if(!format.renderable && !m_allocatedTextureArrays[ar->second]->availableLayers.empty())
        {
            chosenArray = ar->second;
            break;
        } else if(format.renderable && !m_allocatedTextureArrays_renderable[format.renderingSet][ar->second]->availableLayers.empty()) {
            chosenArray = ar->second;
            break;
        }
    }

    if(chosenArray == MAX_TEXTURES_ARRAY_SIZE )
    {
        chosenArray = this->createTextureArray(format, cmdPoolName);
        if(chosenArray == MAX_TEXTURES_ARRAY_SIZE)
            return (false);
    }
    m_createImageMutex.unlock();

    VTexture2DArray *texture2DArray = format.renderable ?
        m_allocatedTextureArrays_renderable[format.renderingSet][chosenArray] : m_allocatedTextureArrays[chosenArray];
    size_t chosenLayer = *texture2DArray->availableLayers.begin();
    texture2DArray->availableLayers.pop_front();

    texture->m_textureId = chosenArray;
    texture->m_textureLayer = chosenLayer;

    if(!format.renderable)
    if(source.buffer != VK_NULL_HANDLE)
    {
        texture2DArray->mutex.lock();

        VInstance::lockGraphicsQueueAccess();

        VulkanHelpers::transitionImageLayout(texture2DArray->image,chosenLayer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                             VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,cmdPoolName);
            VBuffersAllocator::copyBufferToImage(source, texture2DArray->image.vkImage,
                                             format.width, format.height,chosenLayer,cmdPoolName);
        VulkanHelpers::transitionImageLayout(texture2DArray->image,chosenLayer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                             VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,cmdPoolName);

        VInstance::unlockGraphicsQueueAccess();

        texture2DArray->mutex.unlock();
    }

    return (true);
}

size_t VTexturesManager::createTextureArray(VTexture2DArrayFormat format, CommandPoolName cmdPoolName)
{
   // std::lock_guard<std::mutex> lock(m_createImageMutex);

    if((!format.renderable && m_allocatedTextureArrays.size() >= MAX_TEXTURES_ARRAY_SIZE)
    || (format.renderable && m_allocatedTextureArrays_renderable[format.renderingSet].size() >= MAX_RENDERABLETEXTURES_ARRAY_SIZE))
    {
        Logger::error("Maximum number of texture arrays allocated");
        return (MAX_TEXTURES_ARRAY_SIZE);
    }

    VTexture2DArray *texture2DArray = new VTexture2DArray();

    //texture2DArray->layerCount      = MAX_LAYER_PER_TEXTURE;
    texture2DArray->extent.width    = format.width;
    texture2DArray->extent.height   = format.height;

    VkImageUsageFlags   usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    VkImageAspectFlags  aspect = VK_IMAGE_ASPECT_COLOR_BIT;
    VkImageLayout       layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    if(format.renderable)
    {
        if(format.vkFormat == VK_FORMAT_D24_UNORM_S8_UINT || format.vkFormat == VK_FORMAT_D32_SFLOAT
        || format.vkFormat == VK_FORMAT_D16_UNORM )
        {
            usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            aspect = VK_IMAGE_ASPECT_DEPTH_BIT;
            layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; //VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        }
        else
        {
            usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
            layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;//VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        }
    }

    if(!VulkanHelpers::createImage(format.width, format.height, MAX_LAYER_PER_TEXTURE, format.vkFormat, VK_IMAGE_TILING_OPTIMAL,
                                   usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                   texture2DArray->image))
        return (MAX_TEXTURES_ARRAY_SIZE);

    VulkanHelpers::transitionImageLayoutLayers(texture2DArray->image, 0, MAX_LAYER_PER_TEXTURE, VK_IMAGE_LAYOUT_UNDEFINED,
                                         VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,cmdPoolName);

    texture2DArray->view = VulkanHelpers::createImageView(texture2DArray->image, aspect);

    for(size_t i = 0 ; i < MAX_LAYER_PER_TEXTURE ; ++i)
        texture2DArray->availableLayers.push_back(i);

    VkDescriptorImageInfo imageInfo = {};
    imageInfo.sampler       = nullptr;
    imageInfo.imageLayout   = layout;
    imageInfo.imageView     = texture2DArray->view;

    for(auto b : m_needToUpdateDescSet)
        b = true;
    //for(auto b : m_needToUpdateImgDescSet)
      //  b = true;


    if(format.renderable)
    {
        m_imageInfos_renderable[format.renderingSet][m_allocatedTextureArrays_renderable[format.renderingSet].size()] = imageInfo;
        m_formatToArray.insert({format, m_allocatedTextureArrays_renderable[format.renderingSet].size()});
        m_allocatedTextureArrays_renderable[format.renderingSet].push_back(texture2DArray);
        return m_allocatedTextureArrays_renderable[format.renderingSet].size()-1;
    } else {
        m_imageInfos[m_allocatedTextureArrays.size()] = imageInfo;
        m_formatToArray.insert({format, m_allocatedTextureArrays.size()});
        m_allocatedTextureArrays.push_back(texture2DArray);
        return m_allocatedTextureArrays.size()-1;
    }

}


void VTexturesManager::freeTexture(VTexture &texture)
{
    VTexturesManager::instance()->freeTextureImpl(texture);
}

void VTexturesManager::freeTexture(VRenderableTexture &renderableTexture)
{
    VTexturesManager::instance()->freeTextureImpl(renderableTexture);

    /*if(renderableTexture.attachment.view != VK_NULL_HANDLE)
        vkDestroyImageView(VInstance::device(), renderableTexture.attachment.view, nullptr);
    renderableTexture.attachment.view = VK_NULL_HANDLE;

    renderableTexture.renderTarget.destroy();*/
}

void VTexturesManager::freeTextureImpl(VTexture &texture, bool isRenderable, size_t renderingSet)
{
    if(!(texture.m_textureId == 0 && texture.m_textureLayer == 0))
    {
        if(isRenderable)
            m_allocatedTextureArrays_renderable[renderingSet][texture.m_textureId]->availableLayers.push_back(texture.m_textureLayer);
        else
            m_allocatedTextureArrays[texture.m_textureId]->availableLayers.push_back(texture.m_textureLayer);
        texture.m_textureId = 0;
        texture.m_textureLayer = 0;
    }
}


void VTexturesManager::freeTextureImpl(VRenderableTexture &renderableTexture)
{
    this->freeTextureImpl(renderableTexture.texture, true, renderableTexture.renderingSet);
    this->freeTextureImpl(renderableTexture.depthTexture, true, renderableTexture.renderingSet);

    if(renderableTexture.renderTarget != nullptr)
        m_cleaningList_renderTargets.push_back({m_framesCount, renderableTexture.renderTarget});

    if(renderableTexture.attachment.view != VK_NULL_HANDLE)
    {
        m_cleaningList_imageViews.push_back({m_framesCount, renderableTexture.attachment.view});
        m_cleaningList_imageViews.push_back({m_framesCount, renderableTexture.depthAttachment.view});
    }

    renderableTexture.attachment.mipViews.clear();
    renderableTexture.attachment.view   = VK_NULL_HANDLE;
    renderableTexture.depthAttachment.mipViews.clear();
    renderableTexture.depthAttachment.view   = VK_NULL_HANDLE;
    renderableTexture.renderTarget      = nullptr;
}

void VTexturesManager::updateCleaningLists(bool cleanAll)
{
    //Maybe I should have a class called cleanable or something.. or just work with pointers

    for(auto renderTarget : m_cleaningList_renderTargets)
    {
        --renderTarget.first;
        if(cleanAll || renderTarget.first == -1)
        {
            delete renderTarget.second;
            m_cleaningList_renderTargets.remove(renderTarget);
        }
    }

    for(auto imageView : m_cleaningList_imageViews)
    {
        --imageView.first;
        if(cleanAll || imageView.first == -1)
        {
            vkDestroyImageView(VInstance::device(), imageView.second, nullptr);
            m_cleaningList_imageViews.remove(imageView);
        }
    }
}

VkSampler VTexturesManager::sampler(bool nearest)
{
    if(nearest)
        return VTexturesManager::instance()->m_sampler_nearest;
    return VTexturesManager::instance()->m_sampler;
}

VkDescriptorSetLayout VTexturesManager::descriptorSetLayout(bool withRenderableTextures)
{
    return VTexturesManager::instance()->getDescriptorSetLayout(withRenderableTextures);
}

VkDescriptorSet VTexturesManager::descriptorSet(bool nearest, bool withRenderableTextures, size_t renderingSet)
{
    return VTexturesManager::instance()->getDescriptorSet(VTexturesManager::instance()->m_lastFrameIndex,nearest,
                                                          withRenderableTextures, renderingSet);
}

VkDescriptorSet VTexturesManager::descriptorSet(size_t frameIndex, bool nearest, bool withRenderableTextures, size_t renderingSet)
{
    return VTexturesManager::instance()->getDescriptorSet(frameIndex, nearest, withRenderableTextures, renderingSet);
}

/*VkDescriptorSet VTexturesManager::imgDescriptorSet(size_t imageIndex)
{
    return VTexturesManager::instance()->getImgDescriptorSet(imageIndex);
}*/

size_t VTexturesManager::descriptorSetVersion(size_t frameIndex)
{
    return VTexturesManager::instance()->getDescriptorSetVersion(frameIndex);
}

/*size_t VTexturesManager::imgDescriptorSetVersion(size_t imageIndex)
{
    return VTexturesManager::instance()->getImgDescriptorSetVersion(imageIndex);
}*/

VkDescriptorSetLayout VTexturesManager::getDescriptorSetLayout(bool withRenderableTextures)
{
    if(withRenderableTextures)
        return m_descriptorSetLayout_renderable;
    return m_descriptorSetLayout;
}

VkDescriptorSet VTexturesManager::getDescriptorSet(size_t frameIndex, bool nearest, bool withRenderableTextures, size_t renderingSet)
{
    if(!nearest)
    {
        if(withRenderableTextures)
            return m_descriptorSets_renderable[renderingSet][frameIndex];
        return m_descriptorSets[frameIndex];
    } else
    {
        if(withRenderableTextures)
            return m_descriptorSets_nearest_renderable[renderingSet][frameIndex];
        return m_descriptorSets_nearest[frameIndex];
    }
    //return nearest ? m_descriptorSets_nearest[frameIndex] : m_descriptorSets[frameIndex];
}


/*VkDescriptorSet VTexturesManager::getImgDescriptorSet(size_t imageIndex)
{
    return m_imgDescriptorSets[imageIndex];
}*/

size_t VTexturesManager::getDescriptorSetVersion(size_t frameIndex)
{
    return m_descSetVersion[frameIndex];
}

/*size_t VTexturesManager::getImgDescriptorSetVersion(size_t imageIndex)
{
    return m_imgDescSetVersion[imageIndex];
}*/

VTexture VTexturesManager::getDummyTexture()
{
    return VTexturesManager::instance()->m_dummyTexture;
}

/// Protected ///

void VTexturesManager::update(size_t frameIndex, size_t imageIndex)
{
    if(m_needToUpdateDescSet[frameIndex] == true)
        this->updateDescriptorSet(frameIndex);
    //if(m_needToUpdateImgDescSet[imageIndex] == true)
      //  this->updateImgDescriptorSet(imageIndex);
    m_lastFrameIndex = frameIndex;

    this->updateCleaningLists();
}

bool VTexturesManager::createDescriptorSetLayouts()
{
    VkDescriptorSetLayoutBinding layoutBindings[3];
    layoutBindings[0].binding = 0;
    layoutBindings[0].descriptorCount = 1;
    layoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
    layoutBindings[0].pImmutableSamplers = nullptr;
    layoutBindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    layoutBindings[1].binding = 1;
    layoutBindings[1].descriptorCount = MAX_TEXTURES_ARRAY_SIZE;
    layoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    layoutBindings[1].pImmutableSamplers = nullptr;
    layoutBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    layoutBindings[2].binding = 2;
    layoutBindings[2].descriptorCount = MAX_RENDERABLETEXTURES_ARRAY_SIZE;
    layoutBindings[2].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    layoutBindings[2].pImmutableSamplers = nullptr;
    layoutBindings[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    {
        VkDescriptorSetLayoutCreateInfo layoutInfo = {};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = 2;
        layoutInfo.pBindings = layoutBindings;

        if(vkCreateDescriptorSetLayout(VInstance::device(), &layoutInfo, nullptr, &m_descriptorSetLayout) != VK_SUCCESS)
            return(false);

        layoutInfo.bindingCount = 3;
        return (vkCreateDescriptorSetLayout(VInstance::device(), &layoutInfo, nullptr, &m_descriptorSetLayout_renderable) == VK_SUCCESS);
    }
}

bool VTexturesManager::createSampler(bool nearest)
{
    VkSamplerCreateInfo samplerInfo = {};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

    if(nearest)
    {
        samplerInfo.magFilter = VK_FILTER_NEAREST;
        samplerInfo.minFilter = VK_FILTER_NEAREST;
    } else {
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
    }

    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = Config::getInt("Graphics","AnisotropicFiltering",VApp::DEFAULT_ANISOTROPIC);
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_TRANSPARENT_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    auto *sampler = nearest ? &m_sampler_nearest : &m_sampler;
    return (vkCreateSampler(VInstance::device(), &samplerInfo, nullptr, sampler) == VK_SUCCESS);
}

bool VTexturesManager::createDescriptorPool(size_t framesCount, size_t imagesCount)
{
    VkDescriptorPoolSize poolSize[2];
    poolSize[0].type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    poolSize[0].descriptorCount = static_cast<uint32_t>(framesCount*(2+2*NBR_RENDERABLETEXTURES_ARRAY_SETS)/*+imagesCount*/);
    poolSize[1].type = VK_DESCRIPTOR_TYPE_SAMPLER;
    poolSize[1].descriptorCount = static_cast<uint32_t>(framesCount*2/*+imagesCount*/);

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 2;
    poolInfo.pPoolSizes = poolSize;

    poolInfo.maxSets = static_cast<uint32_t>(framesCount*(2+2*NBR_RENDERABLETEXTURES_ARRAY_SETS)/*+imagesCount*/);

    return (vkCreateDescriptorPool(VInstance::device(), &poolInfo, nullptr, &m_descriptorPool) == VK_SUCCESS);
}

bool VTexturesManager::createDescriptorSets(size_t framesCount, size_t imagesCount)
{
    VkDevice device = VInstance::device();

    std::vector<VkDescriptorSetLayout> layouts(imagesCount, m_descriptorSetLayout);
    std::vector<VkDescriptorSetLayout> layouts_renderable(imagesCount, m_descriptorSetLayout_renderable);
    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_descriptorPool;
    allocInfo.pSetLayouts = layouts.data();
    allocInfo.descriptorSetCount = static_cast<uint32_t>(framesCount);

    m_descSetVersion = std::vector<size_t> (framesCount, 0);
    m_descriptorSets.resize(framesCount);
    if (vkAllocateDescriptorSets(device, &allocInfo,m_descriptorSets.data()) != VK_SUCCESS)
        return (false);

    m_descriptorSets_nearest.resize(framesCount);
    if (vkAllocateDescriptorSets(device, &allocInfo,m_descriptorSets_nearest.data()) != VK_SUCCESS)
        return (false);

    allocInfo.pSetLayouts = layouts_renderable.data();

    for(size_t i = 0 ; i < NBR_RENDERABLETEXTURES_ARRAY_SETS ; ++i)
    {
        m_descriptorSets_renderable[i].resize(framesCount);
        if (vkAllocateDescriptorSets(device, &allocInfo,m_descriptorSets_renderable[i].data()) != VK_SUCCESS)
            return (false);

        m_descriptorSets_nearest_renderable[i].resize(framesCount);
        if (vkAllocateDescriptorSets(device, &allocInfo,m_descriptorSets_nearest_renderable[i].data()) != VK_SUCCESS)
            return (false);

    }

    /*allocInfo.descriptorSetCount = static_cast<uint32_t>(imagesCount);
    m_imgDescSetVersion = std::vector<size_t> (imagesCount, 0);
    m_imgDescriptorSets.resize(imagesCount);
    if (vkAllocateDescriptorSets(device, &allocInfo,m_imgDescriptorSets .data()) != VK_SUCCESS)
        return (false);*/

    for(size_t i = 0 ; i < framesCount ; ++i)
        this->updateDescriptorSet(i);

    //for(size_t i = 0 ; i < imagesCount ; ++i)
      //  this->updateImgDescriptorSet(i);

    return (true);
}

void VTexturesManager::updateDescriptorSet(size_t frameIndex)
{
    this->writeDescriptorSet(m_descriptorSets[frameIndex]);
    this->writeDescriptorSet(m_descriptorSets_nearest[frameIndex],true);

    for(size_t i = 0 ; i < NBR_RENDERABLETEXTURES_ARRAY_SETS ; ++i)
    {
        this->writeDescriptorSet(m_descriptorSets_renderable[i][frameIndex],false,true,i);
        this->writeDescriptorSet(m_descriptorSets_nearest_renderable[i][frameIndex],true,true,i);
    }

    m_needToUpdateDescSet[frameIndex] = false;
    ++m_descSetVersion[frameIndex];
}

/*void VTexturesManager::updateImgDescriptorSet(size_t imageIndex)
{
    this->writeDescriptorSet(m_imgDescriptorSets[imageIndex]);
    m_needToUpdateImgDescSet[imageIndex] = false;
    ++m_imgDescSetVersion[imageIndex];
}*/

void VTexturesManager::writeDescriptorSet(VkDescriptorSet &descSet, bool nearest, bool withRenderableTextures, size_t renderingSet)
{
    VkWriteDescriptorSet setWrites[3];
    size_t setCounts = withRenderableTextures ? 3 : 2;

	VkDescriptorImageInfo samplerInfo = {};
	samplerInfo.sampler = nearest ? m_sampler_nearest : m_sampler;

	setWrites[0] = {};
	setWrites[0].sType              = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	setWrites[0].dstBinding         = 0;
	setWrites[0].dstArrayElement    = 0;
	setWrites[0].descriptorType     = VK_DESCRIPTOR_TYPE_SAMPLER;
	setWrites[0].descriptorCount    = 1;
	setWrites[0].dstSet             = descSet;
	setWrites[0].pBufferInfo        = 0;
	setWrites[0].pImageInfo         = &samplerInfo;

	setWrites[1] = {};
	setWrites[1].sType              = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	setWrites[1].dstBinding         = 1;
	setWrites[1].dstArrayElement    = 0;
	setWrites[1].descriptorType     = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	setWrites[1].descriptorCount    = MAX_TEXTURES_ARRAY_SIZE;
	setWrites[1].pBufferInfo        = 0;
	setWrites[1].dstSet             = descSet;
	setWrites[1].pImageInfo         = m_imageInfos.data();

	if(withRenderableTextures)
    {
        setWrites[2] = {};
        setWrites[2].sType              = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        setWrites[2].dstBinding         = 2;
        setWrites[2].dstArrayElement    = 0;
        setWrites[2].descriptorType     = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        setWrites[2].descriptorCount    = MAX_RENDERABLETEXTURES_ARRAY_SIZE;
        setWrites[2].pBufferInfo        = 0;
        setWrites[2].dstSet             = descSet;
        setWrites[2].pImageInfo         = m_imageInfos_renderable[renderingSet].data();
    }

    vkUpdateDescriptorSets(VInstance::device(), setCounts, setWrites, 0, nullptr);
}

bool VTexturesManager::createDummyTexture()
{
    unsigned char dummyTexturePtr[4] = {255,255,255,255};
    return m_dummyTexture.generateTexture(1,1,dummyTexturePtr);
}

bool VTexturesManager::init(size_t framesCount, size_t imagesCount)
{
    m_imageInfos.resize(MAX_TEXTURES_ARRAY_SIZE);

    for(size_t i = 0 ; i < NBR_RENDERABLETEXTURES_ARRAY_SETS ; ++i)
        m_imageInfos_renderable[i].resize(MAX_RENDERABLETEXTURES_ARRAY_SIZE);

    m_framesCount = framesCount;

    if(!this->createDummyTexture())
        return (false);

    for(size_t i = 0 ; i < NBR_RENDERABLETEXTURES_ARRAY_SETS ; ++i)
        m_allocatedTextureArrays_renderable[i].push_back(nullptr);

    m_needToUpdateDescSet = std::vector<bool> (framesCount, true);
    //m_needToUpdateImgDescSet = std::vector<bool> (imagesCount, true);

   // size_t i = 0;
    for(auto &imageInfo : m_imageInfos)
    {
        imageInfo.sampler       = nullptr;
        imageInfo.imageLayout   = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView     = m_allocatedTextureArrays[0]->view;
        //i++;
    }

    //i = 0;

    for(size_t i = 0 ; i < NBR_RENDERABLETEXTURES_ARRAY_SETS ; ++i)
    {
        for(auto &imageInfo : m_imageInfos_renderable[i])
        {
            imageInfo.sampler       = nullptr;
            imageInfo.imageLayout   = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView     = m_allocatedTextureArrays[0]->view;
           // i++;
        }
    }

    if(!this->createDescriptorSetLayouts())
        return (false);
    if(!this->createSampler())
        return (false);
    if(!this->createSampler(true))
        return (false);
    if(!this->createDescriptorPool(framesCount,imagesCount))
        return (false);
    if(!this->createDescriptorSets(framesCount,imagesCount))
        return (false);

    return (true);
}

void VTexturesManager::cleanup()
{
    VkDevice device = VInstance::device();

    this->updateCleaningLists(true);

    vkDestroySampler(device, m_sampler, nullptr);
    vkDestroySampler(device, m_sampler_nearest, nullptr);
    vkDestroyDescriptorPool(device,m_descriptorPool,nullptr);
    vkDestroyDescriptorSetLayout(device, m_descriptorSetLayout, nullptr);
    vkDestroyDescriptorSetLayout(device, m_descriptorSetLayout_renderable, nullptr);

    for(auto vtexture : m_allocatedTextureArrays)
    {
        vkDestroyImageView(device, vtexture->view, nullptr);
        VulkanHelpers::destroyImage(vtexture->image);
        delete vtexture;
    }
    m_allocatedTextureArrays.clear();

    for(size_t i = 0 ; i < NBR_RENDERABLETEXTURES_ARRAY_SETS ; ++i)
    {
        for(auto vtexture : m_allocatedTextureArrays_renderable[i])
        {
            if(vtexture)
            {
                vkDestroyImageView(device, vtexture->view, nullptr);
                VulkanHelpers::destroyImage(vtexture->image);
                delete vtexture;
            }
        }
        m_allocatedTextureArrays_renderable[i].clear();
    }

    m_formatToArray.clear();
}

}
