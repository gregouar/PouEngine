#include "PouEngine/renderers/SceneRenderingData.h"

#include "PouEngine/renderers/SceneRenderer.h"

namespace pou
{

SceneRenderingData::SceneRenderingData() :
    m_isInitialized(false)
{
    m_ambientLightingData.ambientLight   = glm::vec4(1.0,1.0,1.0,0.1);
}

SceneRenderingData::~SceneRenderingData()
{
    this->cleanup();
}

bool SceneRenderingData::init(SceneRenderer *renderer)
{
    size_t framesCount = renderer->getFramesCount();
    if(!this->createBuffers(framesCount))
        return (false);
    /*if(!this->createDescriptorSetLayout())
        return (false);*/
    if(!this->createDescriptorPool(framesCount))
        return (false);
    if(!this->createDescriptorSets(framesCount))
        return (false);

    m_isInitialized = true;
    return (true);
}

void SceneRenderingData::cleanup()
{
    if(m_isInitialized)
    {
        VInstance::waitDeviceIdle();
        VkDevice device = VInstance::device();

        for(auto buffer : m_ambientLightingUbos)
            VBuffersAllocator::freeBuffer(buffer);

        if(m_descriptorPool != VK_NULL_HANDLE)
            vkDestroyDescriptorPool(device, m_descriptorPool, nullptr);
        m_descriptorPool = VK_NULL_HANDLE;
    }
    m_isInitialized = false;
}

void SceneRenderingData::update()
{

}

bool SceneRenderingData::isInitialized()
{
    return m_isInitialized;
}

void SceneRenderingData::setAmbientLight(Color color)
{
    if(m_ambientLightingData.ambientLight != color)
        for(auto b : m_needToUpdateAmbientLightingUbos) b = true;
    m_ambientLightingData.ambientLight = color;
}

VkDescriptorSet SceneRenderingData::getAmbientLightingDescSet(size_t frameIndex)
{
    if(m_needToUpdateAmbientLightingDescSets[frameIndex] == true)
        this->updateAmbientLightingDescSet(frameIndex);
    if(m_needToUpdateAmbientLightingUbos[frameIndex] == true)
        this->updateAmbientLightingUbo(frameIndex);
    return m_ambientLightingDescSets[frameIndex];
}

VkDescriptorSetLayout SceneRenderingData::ambientLightingDescSetLayout()
{
    if(s_ambientLightingDescSetLayout == VK_NULL_HANDLE)
        SceneRenderingData::createDescriptorSetLayout();
    return s_ambientLightingDescSetLayout;
}

void SceneRenderingData::cleanStatic()
{
    if(s_ambientLightingDescSetLayout != VK_NULL_HANDLE)
        vkDestroyDescriptorSetLayout(VInstance::device(), s_ambientLightingDescSetLayout, nullptr);
    s_ambientLightingDescSetLayout = VK_NULL_HANDLE;
}


/// Protected ///
VkDescriptorSetLayout SceneRenderingData::s_ambientLightingDescSetLayout = VK_NULL_HANDLE;

bool SceneRenderingData::createBuffers(size_t framesCount)
{
    m_needToUpdateAmbientLightingUbos.resize(framesCount, true);
    m_ambientLightingUbos.resize(framesCount);

    VkDeviceSize bufferSize = sizeof(AmbientLightingData);

    for (size_t i = 0 ; i < framesCount ; ++i)
        if(!VBuffersAllocator::allocBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                           m_ambientLightingUbos[i]))
            return (false);

    return (true);
}

bool SceneRenderingData::createDescriptorSetLayout()
{
    VkDevice device = VInstance::device();

    //Change to 3 for sampler
    std::vector<VkDescriptorSetLayoutBinding> layoutBindings(2, VkDescriptorSetLayoutBinding{});

    for(size_t i = 0 ; i < layoutBindings.size() ; ++i)
    {
        layoutBindings[i].binding = static_cast<uint32_t>(i);
        layoutBindings[i].descriptorCount = 1;
        layoutBindings[i].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    }
    layoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
    /*layoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;*/
    layoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

    VkDescriptorSetLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
    layoutInfo.pBindings    = layoutBindings.data();

    return (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &s_ambientLightingDescSetLayout) == VK_SUCCESS);
}


bool SceneRenderingData::createDescriptorPool(size_t framesCount)
{
    //Change to 3 for sampler
    std::vector<VkDescriptorPoolSize> poolSizes(2,VkDescriptorPoolSize{});
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_SAMPLER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(framesCount);

    /*poolSizes[1].type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(framesCount);*/

    poolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(framesCount);

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType          = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount  = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes     = poolSizes.data();

    poolInfo.maxSets = static_cast<uint32_t>(framesCount);

    return (vkCreateDescriptorPool(VInstance::device(), &poolInfo, nullptr, &m_descriptorPool) == VK_SUCCESS);
}

bool SceneRenderingData::createDescriptorSets(size_t framesCount)
{
    VkDevice device = VInstance::device();

    std::vector<VkDescriptorSetLayout> layouts(framesCount, s_ambientLightingDescSetLayout);
    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(framesCount);
    allocInfo.pSetLayouts = layouts.data();

    m_ambientLightingDescSets.resize(framesCount);
    if (vkAllocateDescriptorSets(device, &allocInfo,m_ambientLightingDescSets.data()) != VK_SUCCESS)
        return (false);

    m_needToUpdateAmbientLightingDescSets.resize(framesCount, true);

    return (true);
}

void SceneRenderingData::updateAmbientLightingDescSet(size_t frameIndex)
{
    VkDescriptorBufferInfo bufferInfo = {};
    bufferInfo.buffer = m_ambientLightingUbos[frameIndex].buffer;
    bufferInfo.offset = m_ambientLightingUbos[frameIndex].offset;
    bufferInfo.range = sizeof(AmbientLightingData);

    VkDescriptorImageInfo imageInfo = {};
    imageInfo.imageLayout   = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  //  imageInfo.imageView     = m_filteredEnvMap.view;
    imageInfo.sampler       = VTexturesManager::sampler();

    ///Change to 3 if reactivate sampler
    std::vector<VkWriteDescriptorSet> descriptorWrites(2,VkWriteDescriptorSet{});
    for(size_t i = 0 ; i < descriptorWrites.size() ; ++i)
    {
        descriptorWrites[i].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[i].dstSet          = m_ambientLightingDescSets[frameIndex];
        descriptorWrites[i].dstArrayElement = 0;
        descriptorWrites[i].dstBinding      = static_cast<uint32_t>(i);
        descriptorWrites[i].descriptorCount = 1;
    }

    descriptorWrites[0].descriptorType  = VK_DESCRIPTOR_TYPE_SAMPLER;
    descriptorWrites[0].pImageInfo      = &imageInfo;

    /*descriptorWrites[1].descriptorType  = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    descriptorWrites[1].pImageInfo      = &imageInfo;*/

    descriptorWrites[1].descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrites[1].pBufferInfo     = &bufferInfo;

    vkUpdateDescriptorSets(VInstance::device(), descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);

    m_needToUpdateAmbientLightingDescSets[frameIndex] = false;
}

void SceneRenderingData::updateAmbientLightingUbo(size_t frameIndex)
{
    VBuffersAllocator::writeBuffer(m_ambientLightingUbos[frameIndex],
                                  &m_ambientLightingData,
                                   sizeof(AmbientLightingData));

    m_needToUpdateAmbientLightingUbos[frameIndex] = false;
}


}
