#include "PouEngine/renderers/UiRenderer.h"

#include "PouEngine/core/VApp.h"
#include "PouEngine/utils/Logger.h"


namespace pou
{

const char *UiRenderer::UIRENDERING_VERTSHADERFILE = "uirendering.vert.spv";
const char *UiRenderer::UIRENDERING_FRAGSHADERFILE = "uirendering.frag.spv";

UiRenderer::UiRenderer(RenderWindow *targetWindow, RendererName name, RenderereOrder order) :
    AbstractRenderer(targetWindow, name, order)
{
    this->init();
}

UiRenderer::~UiRenderer()
{
    this->cleanup();
}

bool UiRenderer::init()
{
    size_t framesCount = m_targetWindow->getFramesCount();

    m_uiElementsVbos.resize(framesCount);
    for(auto &vbo : m_uiElementsVbos)
        vbo = new DynamicVBO<RenderableUiDatum>(8);

    if(!AbstractRenderer::init())
        return (false);

    return (true);
}


void UiRenderer::cleanup()
{
    for(auto vbo : m_uiElementsVbos)
        delete vbo;
    m_uiElementsVbos.clear();

    m_graphicPipeline.destroy();

    AbstractRenderer::cleanup();
}



bool UiRenderer::createGraphicsPipeline()
{
    std::ostringstream vertShaderPath,fragShaderPath;
    vertShaderPath << VApp::DEFAULT_SHADERPATH << UIRENDERING_VERTSHADERFILE;
    fragShaderPath << VApp::DEFAULT_SHADERPATH << UIRENDERING_FRAGSHADERFILE;

    m_graphicPipeline.createShader(vertShaderPath.str(), VK_SHADER_STAGE_VERTEX_BIT);
    m_graphicPipeline.createShader(fragShaderPath.str(), VK_SHADER_STAGE_FRAGMENT_BIT);

    auto bindingDescription = RenderableUiDatum::getBindingDescription();
    auto attributeDescriptions = RenderableUiDatum::getAttributeDescriptions();
    m_graphicPipeline.setVertexInput(1, &bindingDescription,
                                    attributeDescriptions.size(), attributeDescriptions.data());

    m_graphicPipeline.setInputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP, true);

    m_graphicPipeline.attachDescriptorSetLayout(m_renderView.getDescriptorSetLayout());
    m_graphicPipeline.attachDescriptorSetLayout(VTexturesManager::descriptorSetLayout());

    //m_graphicPipeline.attachPushConstant(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(glm::vec4));

    //m_graphicPipeline.setDepthTest(true, true, VK_COMPARE_OP_GREATER_OR_EQUAL);
    m_graphicPipeline.setBlendMode(BlendMode_Alpha,0);

    return m_graphicPipeline.init(m_renderGraph.getRenderPass(m_defaultPass));
}


bool UiRenderer::recordPrimaryCmb(uint32_t imageIndex)
{
    //this->uploadVbos();
    m_uiElementsVbos[m_curFrameIndex]->uploadVBO();

    size_t  uiVboSize      = m_uiElementsVbos[m_curFrameIndex]->getUploadedSize();
    VBuffer uiInstancesVB  = m_uiElementsVbos[m_curFrameIndex]->getBuffer();

    VkDescriptorSet descriptorSets[] = {m_renderView.getDescriptorSet(m_curFrameIndex),
                                        VTexturesManager::descriptorSet(m_curFrameIndex) };

    VkCommandBuffer cmb = m_renderGraph.startRecording(m_defaultPass, imageIndex, m_curFrameIndex);

        if(uiVboSize != 0)
        {
            vkCmdBindDescriptorSets(cmb,VK_PIPELINE_BIND_POINT_GRAPHICS,
                                m_graphicPipeline.getLayout(),0,2, descriptorSets, 0, nullptr);

            vkCmdBindVertexBuffers(cmb, 0, 1, &uiInstancesVB.buffer, &uiInstancesVB.offset);

            m_graphicPipeline.bind(cmb);

            //m_renderView.setupViewport(renderingInstance->getViewInfo(), cmb);

            /*vkCmdDraw(cmb, 4, renderingInstance->getSpritesVboSize(),
                           0, renderingInstance->getSpritesVboOffset());*/

            vkCmdDraw(cmb, 4, uiVboSize,
                           0, 0);
        }

    if(!m_renderGraph.endRecording(m_defaultPass))
        return (false);

    return (true);
}


}
