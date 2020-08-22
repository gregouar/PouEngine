#include "PouEngine/renderers/AbstractRenderer.h"

#include <array>

#include "PouEngine/tools/Profiler.h"
#include "PouEngine/tools/Logger.h"

namespace pou
{

AbstractRenderer::AbstractRenderer(RenderWindow *targetWindow, RendererName name, RenderereOrder order) :
    //m_useDynamicView(false),
    m_targetWindow(targetWindow),
    m_renderGraph(targetWindow->getSwapchainSize(),
                  targetWindow->getFramesCount()),
    //m_descriptorPool(VK_NULL_HANDLE),
    m_curFrameIndex(0),
    m_order(order),
    m_name(name)
{
    targetWindow->attachRenderer(this);
}

AbstractRenderer::~AbstractRenderer()
{
}


void AbstractRenderer::update(size_t frameIndex)
{
    //if(!m_useDynamicView)
    m_renderView.update(frameIndex);
}

void AbstractRenderer::render(uint32_t imageIndex)
{
    Profiler::pushClock("Record primary buffer");
    this->recordPrimaryCmb(imageIndex);
    m_finalPasses = m_renderGraph.submitToGraphicsQueue(imageIndex, m_curFrameIndex);
    Profiler::popClock();

    m_curFrameIndex = (m_curFrameIndex + 1) % m_targetWindow->getFramesCount();
}

void AbstractRenderer::setView(ViewInfo viewInfo)
{
    m_renderView.setProjectionFactor(viewInfo.projFactor);
   /** m_renderView.setView(viewInfo.view, viewInfo.viewInv); **/
}

std::vector<FullRenderPass*> AbstractRenderer::getFinalPasses()
{
    return m_finalPasses;
}

size_t AbstractRenderer::getFramesCount()
{
    return m_targetWindow->getFramesCount();
}

RendererName AbstractRenderer::getName()
{
    return m_name;
}

void AbstractRenderer::prepareRenderPass()
{
    m_defaultPass = m_renderGraph.addRenderPass(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    ///Could add something to change loadOp/storeOp depending on RendererOrder
    VkAttachmentLoadOp loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    if(m_order == Renderer_First || m_order == Renderer_Unique)
         loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;

    m_renderGraph.addNewAttachments(m_defaultPass, m_targetWindow->getSwapchainAttachments(), VK_ATTACHMENT_STORE_OP_STORE,loadOp);
    m_renderGraph.addNewAttachments(m_defaultPass, m_targetWindow->getSwapchainDepthAttachments(), VK_ATTACHMENT_STORE_OP_STORE,loadOp);
}

bool AbstractRenderer::initRenderGraph()
{
    m_renderGraph.setDefaultExtent(m_targetWindow->getSwapchainExtent());
    return m_renderGraph.init();
}

bool AbstractRenderer::createRenderView()
{
    m_renderView.setExtent({m_targetWindow->getSwapchainExtent().width,
                            m_targetWindow->getSwapchainExtent().height});

    return m_renderView.create(m_targetWindow->getFramesCount()/*, m_useDynamicView*/);
}

bool AbstractRenderer::init()
{
    this->prepareRenderPass();

    /*if(!this->createDescriptorSetLayouts())
    {
        Logger::error("Cannot create default descriptor set layout");
        return (false);
    }*/

    if(!this->createRenderView())
    {
        Logger::error("Cannot create render view");
        return (false);
    }

    if(!this->initRenderGraph())
    {
        Logger::error("Cannot initialize render graph");
        return (false);
    }

    if(!this->createGraphicsPipeline())
    {
        Logger::error("Cannot create graphics pipeline");
        return (false);
    }

    /*if(!this->createDescriptorPool())
    {
        Logger::error("Cannot create descriptor pool");
        return (false);
    }

    if(!this->createDescriptorSets())
    {
        Logger::error("Cannot create descriptor sets");
        return (false);
    }*/

    return (true);
}

bool AbstractRenderer::reinit()
{
    this->cleanup();
    return this->init();
}

void AbstractRenderer::cleanup()
{
    //VkDevice device = VInstance::device();

    /*for(auto att : m_pingPongAttachments)
        VulkanHelpers::destroyAttachment(att.second);
    m_pingPongAttachments.clear();*/

    for(auto target : m_pingPongRenderTargets)
        delete target.second;
    m_pingPongRenderTargets.clear();

    m_renderView.destroy();

   /* if(m_descriptorPool != VK_NULL_HANDLE)
        vkDestroyDescriptorPool(device,m_descriptorPool,nullptr);
    m_descriptorPool = VK_NULL_HANDLE;*/

    m_renderGraph.destroy();
}


/*VFramebufferAttachment *AbstractRenderer::getPingPongAttachment(const VTextureFormat &format)
{
    auto it = m_pingPongAttachments.find(format);
    if(it == m_pingPongAttachments.end())
    {
        it = m_pingPongAttachments.insert({format, VFramebufferAttachment ()}).first;
        VulkanHelpers::createAttachment(format.width, format.height, format.vkFormat,
                                        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, it->second);
    }

    return &(it->second);
}*/


VRenderTarget *AbstractRenderer::getPingPongRenderTarget(std::vector<VTextureFormat> &formats, VRenderPass *renderPass)
{
    if(formats.empty())
        return (nullptr);

    PingPongFormatsList ppfl;
    ppfl.formats = std::move(formats);
    ppfl.renderPass = renderPass;

    auto it = m_pingPongRenderTargets.find(ppfl);
    if(it == m_pingPongRenderTargets.end())
    {
        it = m_pingPongRenderTargets.insert({ppfl, new VRenderTarget()}).first;
        it->second->setExtent({formats.front().width, formats.front().height});
        for(auto format : formats)
            it->second->createAttachment(format.vkFormat);
        it->second->init(1, renderPass);
    }

    return (it->second);
}


/*bool AbstractRenderer::createDescriptorPool()
{
    if(m_descriptorPoolSizes.empty())
        return (true);

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(m_descriptorPoolSizes.size());
    poolInfo.pPoolSizes = m_descriptorPoolSizes.data();

    //poolInfo.maxSets = m_targetWindow->get * m_descriptorPoolSizes.size();
    for(auto poolSize : m_descriptorPoolSizes)
        poolInfo.maxSets += poolSize.descriptorCount;

    return (vkCreateDescriptorPool(VInstance::device(), &poolInfo, nullptr, &m_descriptorPool) == VK_SUCCESS);
}

bool AbstractRenderer::createDescriptorSets()
{
    return (true);
}*/



}
