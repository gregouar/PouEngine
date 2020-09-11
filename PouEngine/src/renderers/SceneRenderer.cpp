#include "PouEngine/renderers/SceneRenderer.h"

#include "PouEngine/core/VApp.h"
#include "PouEngine/assets/MeshAsset.h"
#include "PouEngine/scene/SpriteEntity.h"
//#include "PouEngine/scene/MeshEntity.h"
#include "PouEngine/tools/Logger.h"
#include "PouEngine/renderers/PBRToolbox.h"

#include <sstream>

namespace pou
{

/*const float SceneRenderer::SSGI_SIZE_FACTOR = 1.0;

const char *SceneRenderer::ISOSPRITE_SHADOWGEN_VERTSHADERFILE = "deferred/isoSpriteShadowGen.vert.spv";
const char *SceneRenderer::ISOSPRITE_SHADOWGEN_FRAGSHADERFILE = "deferred/isoSpriteShadowGen.frag.spv";
const char *SceneRenderer::ISOSPRITE_SHADOWFILT_VERTSHADERFILE = "deferred/isoSpriteShadowFilt.vert.spv";
const char *SceneRenderer::ISOSPRITE_SHADOWFILT_FRAGSHADERFILE = "deferred/isoSpriteShadowFilt.frag.spv";
const char *SceneRenderer::ISOSPRITE_SHADOW_VERTSHADERFILE = "deferred/isoSpriteShadow.vert.spv";
const char *SceneRenderer::ISOSPRITE_SHADOW_FRAGSHADERFILE = "deferred/isoSpriteShadow.frag.spv";
const char *SceneRenderer::MESH_DIRECTSHADOW_VERTSHADERFILE = "deferred/meshDirectShadow.vert.spv";
const char *SceneRenderer::MESH_DIRECTSHADOW_FRAGSHADERFILE = "deferred/meshDirectShadow.frag.spv";
const char *SceneRenderer::ISOSPRITE_DEFERRED_VERTSHADERFILE = "deferred/isoSpriteShader.vert.spv";
const char *SceneRenderer::ISOSPRITE_DEFERRED_FRAGSHADERFILE = "deferred/isoSpriteShader.frag.spv";
const char *SceneRenderer::MESH_DEFERRED_VERTSHADERFILE = "deferred/meshShader.vert.spv";
const char *SceneRenderer::MESH_DEFERRED_FRAGSHADERFILE = "deferred/meshShader.frag.spv";
const char *SceneRenderer::ISOSPRITE_ALPHADETECT_VERTSHADERFILE = "deferred/isoSpriteAlphaDetection.vert.spv";
const char *SceneRenderer::ISOSPRITE_ALPHADETECT_FRAGSHADERFILE = "deferred/isoSpriteAlphaDetection.frag.spv";
const char *SceneRenderer::ISOSPRITE_ALPHADEFERRED_VERTSHADERFILE = "deferred/isoSpriteShader.vert.spv";
const char *SceneRenderer::ISOSPRITE_ALPHADEFERRED_FRAGSHADERFILE = "deferred/isoSpriteAlphaShader.frag.spv";
const char *SceneRenderer::SSGI_BN_VERTSHADERFILE = "lighting/ssgiBN.vert.spv";
const char *SceneRenderer::SSGI_BN_FRAGSHADERFILE = "lighting/ssgiBN.frag.spv";
const char *SceneRenderer::LIGHTING_VERTSHADERFILE = "lighting/lighting.vert.spv";
const char *SceneRenderer::LIGHTING_FRAGSHADERFILE = "lighting/lighting.frag.spv";
const char *SceneRenderer::SSGI_LIGHTING_VERTSHADERFILE = "lighting/ssgiLighting.vert.spv";
const char *SceneRenderer::SSGI_LIGHTING_FRAGSHADERFILE = "lighting/ssgiLighting.frag.spv";
const char *SceneRenderer::AMBIENTLIGHTING_VERTSHADERFILE = "lighting/ambientLighting.vert.spv";
const char *SceneRenderer::AMBIENTLIGHTING_FRAGSHADERFILE = "lighting/ambientLighting.frag.spv";
const char *SceneRenderer::TONEMAPPING_VERTSHADERFILE = "toneMapping.vert.spv";
const char *SceneRenderer::TONEMAPPING_FRAGSHADERFILE = "toneMapping.frag.spv";
const char *SceneRenderer::BLUR_VERTSHADERFILE = "smartBlur.vert.spv";
const char *SceneRenderer::BLUR_FRAGSHADERFILE = "smartBlur.frag.spv";*/


const char *SceneRenderer::SPRITE_SHADOW_VERTSHADERFILE = "deferred/spriteShadow.vert.spv";
const char *SceneRenderer::SPRITE_SHADOW_FRAGSHADERFILE = "deferred/spriteShadow.frag.spv";
const char *SceneRenderer::MESH_DIRECTSHADOW_VERTSHADERFILE = "deferred/meshDirectShadow.vert.spv";
const char *SceneRenderer::MESH_DIRECTSHADOW_FRAGSHADERFILE = "deferred/meshDirectShadow.frag.spv";

const char *SceneRenderer::BLUR_VERTSHADERFILE = "fullscreen.vert.spv";
const char *SceneRenderer::SHADOWMAPBLUR_FRAGSHADERFILE = "lighting/shadowMapBlur.frag.spv";
const char *SceneRenderer::BLUR_FRAGSHADERFILE = "blur.frag.spv";

const char *SceneRenderer::SPRITE_DEFERRED_VERTSHADERFILE = "deferred/spriteShader.vert.spv";
const char *SceneRenderer::SPRITE_DEFERRED_FRAGSHADERFILE = "deferred/spriteShader.frag.spv";
const char *SceneRenderer::MESH_DEFERRED_VERTSHADERFILE = "deferred/meshShader.vert.spv";
const char *SceneRenderer::MESH_DEFERRED_FRAGSHADERFILE = "deferred/meshShader.frag.spv";

const char *SceneRenderer::BENTNORMALS_VERTSHADERFILE = "fullscreen.vert.spv";
const char *SceneRenderer::BENTNORMALS_FRAGSHADERFILE = "lighting/bentNormals.frag.spv";
const char *SceneRenderer::SMARTBLUR_VERTSHADERFILE = "fullscreen.vert.spv";
const char *SceneRenderer::SMARTBLUR_FRAGSHADERFILE = "smartBlur.frag.spv";

const char *SceneRenderer::LIGHTING_VERTSHADERFILE = "lighting/lighting.vert.spv";
const char *SceneRenderer::LIGHTING_FRAGSHADERFILE = "lighting/lighting.frag.spv";
const char *SceneRenderer::AMBIENTLIGHTING_VERTSHADERFILE = "fullscreen.vert.spv";
const char *SceneRenderer::AMBIENTLIGHTING_FRAGSHADERFILE = "lighting/ambientLighting.frag.spv";

const char *SceneRenderer::BLOOM_VERTSHADERFILE = "fullscreen.vert.spv";
const char *SceneRenderer::BLOOM_FRAGSHADERFILE = "lighting/bloom.frag.spv";

const char *SceneRenderer::TONEMAPPING_VERTSHADERFILE = "fullscreen.vert.spv";
const char *SceneRenderer::TONEMAPPING_FRAGSHADERFILE = "toneMapping.frag.spv";


SceneRenderer::SceneRenderer(RenderWindow *targetWindow, RendererName name, RenderereOrder order) :
    AbstractRenderer(targetWindow, name, order)
    //m_sampler(VK_NULL_HANDLE),
    //m_ambientLightingDescriptorLayout(VK_NULL_HANDLE)
{
    //m_useDynamicView = true;
    this->init();
}

SceneRenderer::~SceneRenderer()
{
    this->cleanup();
}

void SceneRenderer::update(size_t frameIndex)
{
    /*for(auto renderableTexture : m_spriteShadowBufs[frameIndex])
        VTexturesManager::freeTexture(renderableTexture);
    m_spriteShadowBufs[frameIndex].clear();*/

    AbstractRenderer::update(frameIndex);
}

void SceneRenderer::addRenderingInstance(SceneRenderingInstance *renderingInstance)
{
    m_renderingInstances.push_back(renderingInstance);
}

void SceneRenderer::addShadowMapToRender(VRenderTarget* shadowMap, const LightDatum &datum)
{
    m_shadowMapsInstances.push_back(ShadowMapRenderingInstance{});

    /*m_shadowMapsInstances.back().lightPosition = datum.position;
    m_shadowMapsInstances.back().lightPosition = datum.position;
    m_shadowMapsInstances.back().shadowShift = datum.shadowShift;*/
    m_shadowMapsInstances.back().lightDatum = datum;
    m_shadowMapsInstances.back().extent = {shadowMap->getExtent().width,
                                            shadowMap->getExtent().height};

    m_shadowMapsInstances.back().spritesVboSize = 0;
    m_shadowMapsInstances.back().spritesVboOffset = m_spritesVbos[m_curFrameIndex]->getSize();



    m_renderGraph.addDynamicRenderTarget(m_shadowMapsPass,shadowMap);

    /*std::vector<VTextureFormat> formats;
    formats.push_back({shadowMap->getExtent().width, shadowMap->getExtent().height,
                      VK_FORMAT_R32_SFLOAT});
    formats.push_back({shadowMap->getExtent().width, shadowMap->getExtent().height,
                      VK_FORMAT_D16_UNORM});
    auto pingPong = this->getPingPongRenderTarget(formats, m_renderGraph.getRenderPass(m_shadowMapsBlurPasses[0]));*/

    VTextureFormat format;
    format.width = m_shadowMapsInstances.back().extent.x;
    format.height = m_shadowMapsInstances.back().extent.y;
    format.vkFormat = VK_FORMAT_R32G32_SFLOAT;

    auto it = m_shadowMapBlurPingPongs.find(format);
    if(it == m_shadowMapBlurPingPongs.end())
    {
        it = m_shadowMapBlurPingPongs.insert({format, VRenderableTexture()}).first;
        VTexturesManager::allocRenderableTextureWithDepth(1, m_shadowMapsInstances.back().extent.x, m_shadowMapsInstances.back().extent.y,
                                                      format.vkFormat, VK_FORMAT_D16_UNORM,
                                                      m_renderGraph.getRenderPass(m_shadowMapsPass), &(it->second));

       // VTexturesManager::allocRenderableTexture(format,m_renderGraph.getRenderPass(m_shadowMapsPass), &(it->second));
    }

    m_renderGraph.addDynamicRenderTarget(m_shadowMapsPass/*m_shadowMapsBlurPasses[0]*/,it->second.renderTarget);
    m_shadowMapsInstances.back().pingPongTex = &(it->second);

    m_renderGraph.addDynamicRenderTarget(m_shadowMapsPass/*m_shadowMapsBlurPasses[0]*/,shadowMap);

    shadowMap->setClearValue(0, glm::vec4(1.0), glm::vec2(0.0));
}

/*void SceneRenderer::addSpriteShadowToRender(VRenderTarget* spriteShadow, const SpriteShadowGenerationDatum &datum)
{
    m_spriteShadowBufs[m_curFrameIndex].push_back(VRenderableTexture());

    VTexturesManager::allocRenderableTexture(spriteShadow->getExtent().width, spriteShadow->getExtent().height,
                                                 VK_FORMAT_R8G8B8A8_UNORM,
                                                 this->getSpriteShadowsRenderPass(),
                                                &m_spriteShadowBufs[m_curFrameIndex].back());

    //m_renderGraph.addDynamicRenderTarget(m_spriteShadowsPass,spriteShadow); //
    m_renderGraph.addDynamicRenderTarget(m_spriteShadowsPass,m_spriteShadowBufs[m_curFrameIndex].back().renderTarget); //Raytracing
    m_renderGraph.addDynamicRenderTarget(m_spriteShadowsPass,spriteShadow); //Filtering
    m_spriteShadowGenerationVbos[m_curFrameIndex]->push_back(datum);

    SpriteShadowGenerationDatum filteredDatum = datum;
    filteredDatum.albedo_texId = m_spriteShadowBufs[m_curFrameIndex].back().texture.getTexturePair();
    m_spriteShadowGenerationVbos[m_curFrameIndex]->push_back(filteredDatum);

    //m_spriteShadowsToRender.push_back({spriteShadow, datum});
}*/

/*void SceneRenderer::addToSpriteShadowsVbo(const IsoSpriteShadowDatum &datum)
{
    m_spriteShadowsVbos[m_curFrameIndex]->push_back(datum);
    m_shadowMapsInstances.back().spritesVboSize++;
}*/

void SceneRenderer::addToSpriteShadowsVbo(const SpriteDatum &datum)
{
    /*m_spriteShadowsVbos[m_curFrameIndex]->push_back(datum);
    m_shadowMapsInstances.back().spritesVboSize++;*/


    /*if(m_shadowMapsInstances.back().spritesVboSize == 0)
        m_shadowMapsInstances.back().spritesVboOffset = this->getSpritesVboSize();*/

    m_spritesVbos[m_curFrameIndex]->push_back(datum);
    m_shadowMapsInstances.back().spritesVboSize++;
}

void SceneRenderer::addToMeshShadowsVbo(VMesh *mesh, const MeshDatum &datum)
{
    auto foundedSizeAndOffset = m_shadowMapsInstances.back().meshesVboSizeAndOffset.find(mesh);
    if(foundedSizeAndOffset == m_shadowMapsInstances.back().meshesVboSizeAndOffset.end())
    {
        foundedSizeAndOffset = m_shadowMapsInstances.back().meshesVboSizeAndOffset.insert(
            foundedSizeAndOffset, {mesh,{0,this->getMeshesVboSize(mesh)}});

        //m_shadowMapsInstances.back().meshesVboOffset[mesh] = this->getMeshesVboSize(mesh);
    }
    this->addToMeshesVbo(mesh,datum);
    ++foundedSizeAndOffset->second.first;
}

void SceneRenderer::addToSpritesVbo(const SpriteDatum &datum)
{
    m_spritesVbos[m_curFrameIndex]->push_back(datum);
}

void SceneRenderer::addToMeshesVbo(VMesh* mesh, const MeshDatum &datum)
{
    auto foundedVbo = m_meshesVbos[m_curFrameIndex].find(mesh);
    if(foundedVbo == m_meshesVbos[m_curFrameIndex].end())
        foundedVbo = m_meshesVbos[m_curFrameIndex].insert(foundedVbo, {mesh, new DynamicVBO<MeshDatum>(4)});
    foundedVbo->second->push_back(datum);
}

void SceneRenderer::addToLightsVbo(const LightDatum &datum)
{
    m_lightsVbos[m_curFrameIndex]->push_back(datum);
}

size_t SceneRenderer::getSpritesVboSize()
{
    return m_spritesVbos[m_curFrameIndex]->getSize();
}

size_t SceneRenderer::getMeshesVboSize(VMesh *mesh)
{
    auto foundedVbo = m_meshesVbos[m_curFrameIndex].find(mesh);
    if(foundedVbo == m_meshesVbos[m_curFrameIndex].end())
        return (0);
    return foundedVbo->second->getSize();
}

size_t SceneRenderer::getLightsVboSize()
{
    return m_lightsVbos[m_curFrameIndex]->getSize();
}

/*VRenderPass *SceneRenderer::getSpriteShadowsRenderPass()
{
    return m_renderGraph.getRenderPass(m_spriteShadowsPass);
}*/

VRenderPass *SceneRenderer::getShadowMapsRenderPass()
{
    return m_renderGraph.getRenderPass(m_shadowMapsPass);
}

bool SceneRenderer::recordToneMappingCmb(uint32_t imageIndex)
{
    VkCommandBuffer cmb = m_renderGraph.startRecording(m_toneMappingPass, imageIndex, m_curFrameIndex);

        m_toneMappingPipeline.bind(cmb);
        VkDescriptorSet descSets[] = {m_renderGraph.getDescriptorSet(m_toneMappingPass,imageIndex)};
        vkCmdBindDescriptorSets(cmb, VK_PIPELINE_BIND_POINT_GRAPHICS, m_toneMappingPipeline.getLayout(),
                                0, 1, descSets, 0, NULL);
        vkCmdDraw(cmb, 3, 1, 0, 0);

    return m_renderGraph.endRecording(m_toneMappingPass);
}


bool SceneRenderer::recordPrimaryCmb(uint32_t imageIndex)
{
    for(auto renderingInstance : m_renderingInstances)
        renderingInstance->prepareShadowsRendering(this, imageIndex);

    this->uploadVbos();

    this->recordShadowCmb(imageIndex);

    bool r = true;

    if(!this->recordDeferredCmb(imageIndex))
        r = false;
    if(!this->recordLightingCmb(imageIndex))
        r = false;
    if(!this->recordAmbientLightingCmb(imageIndex))
        r = false;

    for(auto renderingInstance : m_renderingInstances)
        delete renderingInstance;
    m_renderingInstances.clear();

    return r;
}

void SceneRenderer::uploadVbos()
{
    /*m_spriteShadowGenerationVbos[m_curFrameIndex]->uploadVBO();
    m_spriteShadowsVbos[m_curFrameIndex]->uploadVBO();*/
    m_spritesVbos[m_curFrameIndex]->uploadVBO();

    for(auto &mesh : m_meshesVbos[m_curFrameIndex])
        mesh.second->uploadVBO();

    m_lightsVbos[m_curFrameIndex]->uploadVBO();
}

bool SceneRenderer::recordShadowCmb(uint32_t imageIndex)
{
    ///Shadow map rendering
    VBuffer spritesInstancesVB  = m_spritesVbos[m_curFrameIndex]->getBuffer();

    //Start recording
    VkCommandBuffer cmb = m_renderGraph.startRecording(m_shadowMapsPass, 0, m_curFrameIndex);

    VkDescriptorSet descriptorSets[] = {m_renderView.getDescriptorSet(m_curFrameIndex),
                                        VTexturesManager::descriptorSet(m_curFrameIndex) };


    VkDescriptorSet shadowBludescriptorSets1[] = {VTexturesManager::descriptorSet(m_curFrameIndex, false,true,0) };
    VkDescriptorSet shadowBludescriptorSets2[] = {VTexturesManager::descriptorSet(m_curFrameIndex, false,true,1) };


        auto shadowMapInstance = m_shadowMapsInstances.begin();
        while(m_renderGraph.nextRenderTarget(m_shadowMapsPass))
        {
            VkExtent2D extent = m_renderGraph.getExtent(m_shadowMapsPass);
            glm::vec2 shadowShift = shadowMapInstance->lightDatum.shadowShift;
            glm::vec2 lightXYonZ = {};

            if(shadowMapInstance->lightDatum.position.w == 0)
            {
                lightXYonZ = glm::vec2(shadowMapInstance->lightDatum.position.x,
                                       shadowMapInstance->lightDatum.position.y) / shadowMapInstance->lightDatum.position.z;
            }


            //Mesh shadows drawing
            m_meshDirectShadowsPipeline.bind(cmb);

            vkCmdBindDescriptorSets(cmb,VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    m_meshDirectShadowsPipeline.getLayout(),0,2, descriptorSets, 0, nullptr);

            for(auto &mesh : m_meshesVbos[m_curFrameIndex])
            {
                if(mesh.second->getUploadedSize() != 0)
                {
                    VBuffer meshesInstanceVB = mesh.second->getBuffer();
                    VBuffer vertexBuffer = mesh.first->getVertexBuffer();

                    VkBuffer buffers[] = {vertexBuffer.buffer,
                                          meshesInstanceVB.buffer};
                    VkDeviceSize offsets[] = {vertexBuffer.offset,
                                              meshesInstanceVB.offset};

                    vkCmdBindVertexBuffers(cmb, 0, 2, buffers, offsets);


                    VBuffer indexBuffer = mesh.first->getIndexBuffer();
                    vkCmdBindIndexBuffer(cmb, indexBuffer.buffer,
                                              indexBuffer.offset, VK_INDEX_TYPE_UINT16);

                    for(auto renderingInstance : m_renderingInstances)
                    {
                        m_meshDirectShadowsPipeline.updateViewport(cmb, {0,0}, extent);
                        //m_renderView.setupViewport(renderingInstance->getViewInfo(), cmb);
                        renderingInstance->pushCamPosAndZoom(cmb, m_meshDirectShadowsPipeline.getLayout(),
                                                             VK_SHADER_STAGE_VERTEX_BIT);
                        m_meshDirectShadowsPipeline.updatePushConstant(cmb, 1, (char*)&shadowShift);
                        m_meshDirectShadowsPipeline.updatePushConstant(cmb, 2, (char*)&lightXYonZ);

                        auto &meshSizeAndOffset = shadowMapInstance->meshesVboSizeAndOffset[mesh.first];
                        vkCmdDrawIndexed(cmb, mesh.first->getIndexCount(),
                                               meshSizeAndOffset.first,
                                         0, 0, meshSizeAndOffset.second);
                    }
                }
            }

            //Sprite shadows drawing
            if(m_spritesVbos[m_curFrameIndex]->getUploadedSize() != 0)
            {
                vkCmdBindVertexBuffers(cmb, 0, 1, &spritesInstancesVB.buffer, &spritesInstancesVB.offset);
                m_spriteShadowsPipeline.bind(cmb);

                vkCmdBindDescriptorSets(cmb,VK_PIPELINE_BIND_POINT_GRAPHICS,
                                        m_spriteShadowsPipeline.getLayout(),0,2, descriptorSets, 0, nullptr);

                for(auto renderingInstance : m_renderingInstances)
                {
                    ///m_renderView.setupViewport(renderingInstance->getViewInfo(), cmb);
                    ///I'll need to find something smart to do (in order to have multiple cameras)
                    m_spriteShadowsPipeline.updateViewport(cmb, {0,0}, extent);

                    renderingInstance->pushCamPosAndZoom(cmb, m_spriteShadowsPipeline.getLayout(),
                                                        VK_SHADER_STAGE_VERTEX_BIT);
                    m_spriteShadowsPipeline.updatePushConstant(cmb, 1, (char*)&shadowShift);
                    m_spriteShadowsPipeline.updatePushConstant(cmb, 2, (char*)&lightXYonZ);

                    vkCmdDraw(cmb, 4, shadowMapInstance->spritesVboSize,
                                   0, shadowMapInstance->spritesVboOffset);
                }
            }

            ///Blur
           // VkDescriptorSet blurDescSets1[] = {m_renderGraph.getDescriptorSet(m_shadowMapsPass/*m_shadowMapsBlurPasses[0]*/,imageIndex)};

            m_renderGraph.nextRenderTarget(m_shadowMapsPass/*m_shadowMapsBlurPasses[0]*/);
            m_shadowMapBlurPipelines[0].bind(cmb);

            vkCmdBindDescriptorSets(cmb,VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    m_shadowMapBlurPipelines[0].getLayout(),0,1, shadowBludescriptorSets1, 0, nullptr);

            glm::uvec2 depthTex = shadowMapInstance->lightDatum.shadowMap;

            float radius = shadowMapInstance->lightDatum.shadowBlurRadius / shadowMapInstance->extent.x;

            m_shadowMapBlurPipelines[0].updatePushConstant(cmb, 0, (char*)&depthTex);
            m_shadowMapBlurPipelines[0].updatePushConstant(cmb, 1, (char*)&radius);

            //vkCmdBindDescriptorSets(cmb, VK_PIPELINE_BIND_POINT_GRAPHICS, m_shadowMapBlurPipelines[0].getLayout(),
             //                       0, 1, blurDescSets1, 0, NULL);

            vkCmdDraw(cmb, 3, 1, 0, 0);


            //VkDescriptorSet blurDescSets2[] = {m_renderGraph.getDescriptorSet(m_shadowMapsPass/*m_shadowMapsBlurPasses[0]*/,imageIndex)};

            m_renderGraph.nextRenderTarget(m_shadowMapsPass/*m_shadowMapsBlurPasses[0]*/);
            m_shadowMapBlurPipelines[1].bind(cmb);

            vkCmdBindDescriptorSets(cmb,VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    m_shadowMapBlurPipelines[1].getLayout(),0,1, shadowBludescriptorSets2, 0, nullptr);

            radius = shadowMapInstance->lightDatum.shadowBlurRadius / shadowMapInstance->extent.y;

            depthTex = {shadowMapInstance->pingPongTex->texture.getTextureId(),
                        shadowMapInstance->pingPongTex->texture.getTextureLayer()};

            m_shadowMapBlurPipelines[1].updatePushConstant(cmb, 0, (char*)&depthTex);
            m_shadowMapBlurPipelines[1].updatePushConstant(cmb, 1, (char*)&radius);

            //vkCmdBindDescriptorSets(cmb, VK_PIPELINE_BIND_POINT_GRAPHICS, m_shadowMapBlurPipelines[1].getLayout(),
                    //                0, 1, blurDescSets2, 0, NULL);

            vkCmdDraw(cmb, 3, 1, 0, 0);

            ++shadowMapInstance;
        }

    m_renderGraph.endRecording(m_shadowMapsPass);
    m_shadowMapsInstances.clear();

    return (true);
}

bool SceneRenderer::recordDeferredCmb(uint32_t imageIndex)
{
    size_t  spritesVboSize      = m_spritesVbos[m_curFrameIndex]->getUploadedSize();
    VBuffer spritesInstancesVB  = m_spritesVbos[m_curFrameIndex]->getBuffer();

    //VkDescriptorSet descriptorSets[] = {m_renderView.getDescriptorSet(m_curFrameIndex),
    //                                    VTexturesManager::descriptorSet(m_curFrameIndex) };
    VkDescriptorSet descriptorSets_nearest[] = {m_renderView.getDescriptorSet(m_curFrameIndex),
                                        VTexturesManager::descriptorSet(m_curFrameIndex,true) };


    VkCommandBuffer cmb = m_renderGraph.startRecording(m_deferredPass, imageIndex, m_curFrameIndex);

        vkCmdBindDescriptorSets(cmb,VK_PIPELINE_BIND_POINT_GRAPHICS,
                                m_deferredMeshesPipeline.getLayout(),0,2, descriptorSets_nearest, 0, nullptr);

        m_deferredMeshesPipeline.bind(cmb);

        for(auto &mesh : m_meshesVbos[m_curFrameIndex])
        {
            if(mesh.second->getUploadedSize() != 0)
            {
                VBuffer meshesInstanceVB = mesh.second->getBuffer();
                VBuffer vertexBuffer = mesh.first->getVertexBuffer();

                VkBuffer buffers[] = {vertexBuffer.buffer,
                                      meshesInstanceVB.buffer};
                VkDeviceSize offsets[] = {vertexBuffer.offset,
                                          meshesInstanceVB.offset};

                vkCmdBindVertexBuffers(cmb, 0, 2, buffers, offsets);


                VBuffer indexBuffer = mesh.first->getIndexBuffer();
                vkCmdBindIndexBuffer(cmb, indexBuffer.buffer,
                                          indexBuffer.offset, VK_INDEX_TYPE_UINT16);

                for(auto renderingInstance : m_renderingInstances)
                {
                    m_renderView.setupViewport(renderingInstance->getViewInfo(), cmb);
                    renderingInstance->pushCamPosAndZoom(cmb, m_deferredMeshesPipeline.getLayout(),
                                                         VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);

                    vkCmdDrawIndexed(cmb, mesh.first->getIndexCount(), renderingInstance->getMeshesVboSize(mesh.first),
                                     0, 0, renderingInstance->getMeshesVboOffset(mesh.first));
                }
            }
        }

        vkCmdBindDescriptorSets(cmb,VK_PIPELINE_BIND_POINT_GRAPHICS,
                                m_deferredSpritesPipeline.getLayout(),0,2, descriptorSets_nearest, 0, nullptr);

        if(spritesVboSize != 0)
        {
            vkCmdBindVertexBuffers(cmb, 0, 1, &spritesInstancesVB.buffer, &spritesInstancesVB.offset);

            m_deferredSpritesPipeline.bind(cmb);

            for(auto renderingInstance : m_renderingInstances)
            if(renderingInstance->getSpritesVboSize() != 0)
            {
                m_renderView.setupViewport(renderingInstance->getViewInfo(), cmb);
                renderingInstance->pushCamPosAndZoom(cmb, m_deferredSpritesPipeline.getLayout(),
                                                    VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);

                vkCmdDraw(cmb, 4, renderingInstance->getSpritesVboSize(),
                               0, renderingInstance->getSpritesVboOffset());
            }
            //vkCmdDraw(cmb, 4, spritesVboSize, 0, 0);
        }

    if(!m_renderGraph.endRecording(m_deferredPass))
        return (false);


    /*/// Detection of alpha fragments
    cmb = m_renderGraph.startRecording(m_alphaDetectPass, imageIndex, m_curFrameIndex);

        vkCmdBindDescriptorSets(cmb,VK_PIPELINE_BIND_POINT_GRAPHICS,
                                m_alphaDetectPipeline.getLayout(),0,2, descriptorSets, 0, nullptr);

        if(spritesVboSize != 0)
        {
            vkCmdBindVertexBuffers(cmb, 0, 1, &spritesInstancesVB.buffer, &spritesInstancesVB.offset);

            m_alphaDetectPipeline.bind(cmb);

            for(auto renderingInstance : m_renderingInstances)
            {
                m_renderView.setupViewport(renderingInstance->getViewInfo(), cmb);
                renderingInstance->pushCamPosAndZoom(cmb, m_alphaDetectPipeline.getLayout());

                vkCmdDraw(cmb, 4, renderingInstance->getSpritesVboSize(),
                               0, renderingInstance->getSpritesVboOffset());
            }

            //vkCmdDraw(cmb, 4, spritesVboSize, 0, 0);
        }

    if(!m_renderGraph.endRecording(m_alphaDetectPass))
        return (false);

    VkDescriptorSet descriptorSetsBis[] = { m_renderView.getDescriptorSet(m_curFrameIndex),
                                            VTexturesManager::descriptorSet(m_curFrameIndex),
                                            m_renderGraph.getDescriptorSet(m_alphaDeferredPass, imageIndex) };

    /// Alpha sprites
    cmb = m_renderGraph.startRecording(m_alphaDeferredPass, imageIndex, m_curFrameIndex);

        vkCmdBindDescriptorSets(cmb,VK_PIPELINE_BIND_POINT_GRAPHICS,
                                m_alphaDeferredPipeline.getLayout(),0,3, descriptorSetsBis, 0, nullptr);

        if(spritesVboSize != 0)
        {
            vkCmdBindVertexBuffers(cmb, 0, 1, &spritesInstancesVB.buffer, &spritesInstancesVB.offset);

            m_alphaDeferredPipeline.bind(cmb);

            for(auto renderingInstance : m_renderingInstances)
            {
                m_renderView.setupViewport(renderingInstance->getViewInfo(), cmb);
                renderingInstance->pushCamPosAndZoom(cmb, m_alphaDeferredPipeline.getLayout(),
                                                    VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);

                vkCmdDraw(cmb, 4, renderingInstance->getSpritesVboSize(),
                               0, renderingInstance->getSpritesVboOffset());
            }
            //vkCmdDraw(cmb, 4, spritesVboSize, 0, 0);
        }

    if(!m_renderGraph.endRecording(m_alphaDeferredPass))
        return (false);*/

    return (true);
}


bool SceneRenderer::recordBentNormalsCmb(uint32_t imageIndex)
{
    bool r = true;

    VkCommandBuffer cmb = m_renderGraph.startRecording(m_bentNormalsPass, imageIndex, m_curFrameIndex);

        m_bentNormalsPipeline.bind(cmb);

        VkDescriptorSet descSets[] = {  m_renderView.getDescriptorSet(m_curFrameIndex),
                                        m_renderGraph.getDescriptorSet(m_bentNormalsPass,imageIndex)};

        vkCmdBindDescriptorSets(cmb, VK_PIPELINE_BIND_POINT_GRAPHICS, m_bentNormalsPipeline.getLayout(),
                                0, 2, descSets, 0, NULL);

        vkCmdDraw(cmb, 3, 1, 0, 0);

    r = r & m_renderGraph.endRecording(m_bentNormalsPass);

    for(int i = 0 ; i < 2 ; ++i)
    {
        VkCommandBuffer cmb = m_renderGraph.startRecording(m_bentNormalsBlurPasses[i], imageIndex, m_curFrameIndex);

            m_bentNormalsBlurPipelines[i%2].bind(cmb);
            VkDescriptorSet descSets[] = {m_renderGraph.getDescriptorSet(m_bentNormalsBlurPasses[i],imageIndex)};
            vkCmdBindDescriptorSets(cmb, VK_PIPELINE_BIND_POINT_GRAPHICS, m_bentNormalsBlurPipelines[i%2].getLayout(),
                                    0, 1, descSets, 0, NULL);

            vkCmdDraw(cmb, 3, 1, 0, 0);

        r = r & m_renderGraph.endRecording(m_bentNormalsBlurPasses[i]);
    }

    return r;
}

bool SceneRenderer::recordLightingCmb(uint32_t imageIndex)
{
    size_t  lightsVboSize       = m_lightsVbos[m_curFrameIndex]->getUploadedSize();
    VBuffer lightsInstancesVB   = m_lightsVbos[m_curFrameIndex]->getBuffer();

    VkDescriptorSet lightDescriptorSets[] = {m_renderView.getDescriptorSet(m_curFrameIndex),
                                             VTexturesManager::descriptorSet(m_curFrameIndex,false,true),
                                             m_renderGraph.getDescriptorSet(m_lightingPass,imageIndex/*m_curFrameIndex*/) };

    /// Lighting of opac fragments
    VkCommandBuffer cmb = m_renderGraph.startRecording(m_lightingPass, imageIndex, m_curFrameIndex);

        vkCmdBindDescriptorSets(cmb,VK_PIPELINE_BIND_POINT_GRAPHICS,
                                m_lightingPipeline.getLayout(),0,3, lightDescriptorSets, 0, nullptr);

        m_lightingPipeline.bind(cmb);

        if(lightsVboSize != 0)
        {
            vkCmdBindVertexBuffers(cmb, 0, 1, &lightsInstancesVB.buffer, &lightsInstancesVB.offset);

            m_lightingPipeline.bind(cmb);

            for(auto renderingInstance : m_renderingInstances)
            {
                m_renderView.setupViewport(renderingInstance->getViewInfo(), cmb);
                renderingInstance->pushCamPosAndZoom(cmb, m_lightingPipeline.getLayout(),
                                                    VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);


                vkCmdDraw(cmb, LIGHT_TRIANGLECOUNT+2, renderingInstance->getLightsVboSize(),
                               0, renderingInstance->getLightsVboOffset());
            }
            //vkCmdDraw(cmb, LIGHT_TRIANGLECOUNT+2, lightsVboSize, 0, 0);
        }

    if(!m_renderGraph.endRecording(m_lightingPass))
        return (false);


    /*/// Lighting of alpha fragments
    lightDescriptorSets[2] = m_renderGraph.getDescriptorSet(m_alphaLightingPass,imageIndex);

    cmb = m_renderGraph.startRecording(m_alphaLightingPass, imageIndex, m_curFrameIndex);

        vkCmdBindDescriptorSets(cmb,VK_PIPELINE_BIND_POINT_GRAPHICS,
                                m_alphaLightingPipeline.getLayout(),0,3, lightDescriptorSets, 0, nullptr);

        m_alphaLightingPipeline.bind(cmb);

        if(lightsVboSize != 0)
        {
            vkCmdBindVertexBuffers(cmb, 0, 1, &lightsInstancesVB.buffer, &lightsInstancesVB.offset);

            m_alphaLightingPipeline.bind(cmb);

            for(auto renderingInstance : m_renderingInstances)
            {
                m_renderView.setupViewport(renderingInstance->getViewInfo(), cmb);
                renderingInstance->pushCamPosAndZoom(cmb, m_alphaLightingPipeline.getLayout(),
                                                    VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);

                vkCmdDraw(cmb, LIGHT_TRIANGLECOUNT+2, renderingInstance->getLightsVboSize(),
                               0, renderingInstance->getLightsVboOffset());
            }
            //vkCmdDraw(cmb, LIGHT_TRIANGLECOUNT+2, lightsVboSize, 0, 0);
        }

    if(!m_renderGraph.endRecording(m_alphaLightingPass))
        return (false);

    /// SSGI Lighting
    cmb = m_renderGraph.startRecording(m_ssgiLightingPass, imageIndex, m_curFrameIndex);

        m_ssgiLightingPipeline.bind(cmb);

        VkDescriptorSet descSets[] = {m_renderGraph.getDescriptorSet(m_ssgiLightingPass,imageIndex)};

        vkCmdBindDescriptorSets(cmb, VK_PIPELINE_BIND_POINT_GRAPHICS, m_ssgiLightingPipeline.getLayout(),
                                0, 1, descSets, 0, NULL);

            for(auto renderingInstance : m_renderingInstances)
            {
                m_renderView.setupViewport(renderingInstance->getViewInfo(), cmb);
                renderingInstance->pushCamPosAndZoom(cmb, m_ssgiLightingPipeline.getLayout(),
                                                     VK_SHADER_STAGE_FRAGMENT_BIT);

                vkCmdDraw(cmb, 3, 1, 0, 0);
            }

    if(!m_renderGraph.endRecording(m_ssgiLightingPass))
        return (false);*/

    return (true);
}

/*bool SceneRenderer::recordSsgiBnCmb(uint32_t imageIndex)
{
    ///SSGI BN
    VkCommandBuffer cmb = m_renderGraph.startRecording(m_ssgiBNPass, imageIndex, m_curFrameIndex);

        m_ssgiBNPipeline.bind(cmb);

        ///m_renderView set shouldnt be by IMAGE !!! Need to update renderView or split set in two !!
        VkDescriptorSet descSets[] = {  m_renderView.getDescriptorSet(m_curFrameIndex),
                                        m_renderGraph.getDescriptorSet(m_ssgiBNPass,imageIndex)};

        vkCmdBindDescriptorSets(cmb, VK_PIPELINE_BIND_POINT_GRAPHICS, m_ssgiBNPipeline.getLayout(),
                                0, 2, descSets, 0, NULL);

        int pc = static_cast<int>(imageIndex);
        vkCmdPushConstants(cmb, m_ssgiBNPipeline.getLayout(),
            VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(int), (void*)&pc);

        vkCmdDraw(cmb, 3, 1, 0, 0);

    if(!m_renderGraph.endRecording(m_ssgiBNPass))
        return (false);


    ///SSGI BN Blur
    for(size_t i = 0 ; i < 2 ; ++i)
    {
        cmb = m_renderGraph.startRecording(m_ssgiBNBlurPasses[i], imageIndex, m_curFrameIndex);

            m_ssgiBNBlurPipelines[i].bind(cmb);

            VkDescriptorSet descSets[] = {m_renderGraph.getDescriptorSet(m_ssgiBNBlurPasses[i],imageIndex)};

            vkCmdBindDescriptorSets(cmb, VK_PIPELINE_BIND_POINT_GRAPHICS, m_ssgiBNBlurPipelines[i].getLayout(),
                                    0, 1, descSets, 0, NULL);

            vkCmdDraw(cmb, 3, 1, 0, 0);

        if(!m_renderGraph.endRecording(m_ssgiBNBlurPasses[i]))
            return (false);
    }


    ///SSGI Lighting Blur
    for(size_t i = 0 ; i < 2 ; ++i)
    {
        cmb = m_renderGraph.startRecording(m_ssgiLigghtingBlurPasses[i], imageIndex, m_curFrameIndex);

            m_ssgiLightingBlurPipelines[i].bind(cmb);

            VkDescriptorSet descSets[] = {m_renderGraph.getDescriptorSet(m_ssgiLigghtingBlurPasses[i],imageIndex)};

            vkCmdBindDescriptorSets(cmb, VK_PIPELINE_BIND_POINT_GRAPHICS, m_ssgiLightingBlurPipelines[i].getLayout(),
                                    0, 1, descSets, 0, NULL);

            vkCmdDraw(cmb, 3, 1, 0, 0);

        if(!m_renderGraph.endRecording(m_ssgiLigghtingBlurPasses[i]))
            return (false);
    }

    return (true);
}*/

bool SceneRenderer::recordAmbientLightingCmb(uint32_t imageIndex)
{
    VkCommandBuffer cmb = m_renderGraph.startRecording(m_ambientLightingPass, imageIndex, m_curFrameIndex);

        m_ambientLightingPipeline.bind(cmb);

        VkDescriptorSet descSets[] = {m_renderGraph.getDescriptorSet(m_ambientLightingPass,imageIndex)};

        vkCmdBindDescriptorSets(cmb, VK_PIPELINE_BIND_POINT_GRAPHICS, m_ambientLightingPipeline.getLayout(),
                                0, 1, descSets, 0, NULL);

        for(auto renderingInstance : m_renderingInstances)
        {
            m_renderView.setupViewport(renderingInstance->getViewInfo(), cmb);
            renderingInstance->pushCamPosAndZoom(cmb, m_ambientLightingPipeline.getLayout(),
                                                VK_SHADER_STAGE_FRAGMENT_BIT);

            VkDescriptorSet descSetsBis[] = {renderingInstance->getRenderingData()->getAmbientLightingDescSet(m_curFrameIndex)};

            vkCmdBindDescriptorSets(cmb, VK_PIPELINE_BIND_POINT_GRAPHICS, m_ambientLightingPipeline.getLayout(),
                                    1, 1, descSetsBis, 0, NULL);

            vkCmdDraw(cmb, 3, 1, 0, 0);
        }

    return m_renderGraph.endRecording(m_ambientLightingPass);
}

bool SceneRenderer::recordBloomCmb(uint32_t imageIndex)
{
    bool r = true;

    VkCommandBuffer cmb = m_renderGraph.startRecording(m_bloomPass, imageIndex, m_curFrameIndex);

        m_bloomPipeline.bind(cmb);
        VkDescriptorSet descSets[] = {m_renderGraph.getDescriptorSet(m_bloomPass,imageIndex)};
        vkCmdBindDescriptorSets(cmb, VK_PIPELINE_BIND_POINT_GRAPHICS, m_bloomPipeline.getLayout(),
                                0, 1, descSets, 0, NULL);
        vkCmdDraw(cmb, 3, 1, 0, 0);

    r = r & m_renderGraph.endRecording(m_bloomPass);

    for(int i = 0 ; i < 4 ; ++i)
    {
        VkCommandBuffer cmb = m_renderGraph.startRecording(m_bloomBlurPasses[i], imageIndex, m_curFrameIndex);

            m_bloomBlurPipelines[i%2].bind(cmb);
            VkDescriptorSet descSets[] = {m_renderGraph.getDescriptorSet(m_bloomBlurPasses[i],imageIndex)};
            vkCmdBindDescriptorSets(cmb, VK_PIPELINE_BIND_POINT_GRAPHICS, m_bloomBlurPipelines[i%2].getLayout(),
                                    0, 1, descSets, 0, NULL);

            ///CHANGE BY SOMETHING GIVEN IN OPTION OR SOMETHING
            float radius = (i/2 == 0) ? 4.0f : 16.0f;
            if(i%2 == 0)
                radius /= m_targetWindow->getSwapchainExtent().width;
            else
                radius /= m_targetWindow->getSwapchainExtent().height;

            m_bloomBlurPipelines[i%2].updatePushConstant(cmb, 0, (char*)&radius);

            vkCmdDraw(cmb, 3, 1, 0, 0);

        r = r & m_renderGraph.endRecording(m_bloomBlurPasses[i]);
    }

    return r;
}

/*bool SceneRenderer::updateUbos(uint32_t imageIndex)
{
    VBuffersAllocator::writeBuffer(m_ambientLightingUbo[imageIndex],
                                  &m_ambientLightingData,
                                   sizeof(AmbientLightingData));
    return (true);
}*/

bool SceneRenderer::init()
{
    m_renderView.setDepthFactor(1024 /* *1024 */);
    m_renderView.setScreenOffset(glm::vec3(0.0f, 0.0f, 0.5f));

    size_t framesCount = m_targetWindow->getFramesCount();

    /*m_spriteShadowBufs.resize(m_targetWindow->getFramesCount());

    m_spriteShadowGenerationVbos.resize(framesCount);
    for(auto &vbo : m_spriteShadowGenerationVbos)
        vbo = new DynamicVBO<SpriteShadowGenerationDatum>(32);

    m_spriteShadowsVbos.resize(framesCount);
    for(auto &vbo : m_spriteShadowsVbos)
        vbo = new DynamicVBO<SpriteShadowDatum>(128);*/

    m_spritesVbos.resize(framesCount);
    for(auto &vbo : m_spritesVbos)
        vbo = new DynamicVBO<SpriteDatum>(256);

    m_meshesVbos.resize(framesCount);

    m_lightsVbos.resize(framesCount);
    for(auto &vbo : m_lightsVbos)
        vbo = new DynamicVBO<LightDatum>(128);

    if(!this->createAttachments())
        return (false);

    if(!AbstractRenderer::init())
        return (false);

    for(size_t i = 0 ; i < m_targetWindow->getSwapchainSize() ; ++i)
    {
        //this->recordSsgiBnCmb(i);
        this->recordBentNormalsCmb(i);
        this->recordBloomCmb(i);
        this->recordToneMappingCmb(i);
    }

    return (true);
}

void SceneRenderer::prepareRenderPass()
{
    this->prepareShadowRenderPass();
    this->prepareDeferredRenderPass();
    /*this->prepareAlphaDetectRenderPass();
    this->prepareAlphaDeferredRenderPass();
    this->prepareSsgiBNRenderPasses();*/
    this->prepareBentNormalsRenderPass();
    this->prepareLightingRenderPass();
    /*this->prepareAlphaLightingRenderPass();
    this->prepareSsgiLightingRenderPass();*/
    this->prepareAmbientLightingRenderPass();
    this->prepareBloomRenderPasses();
    this->prepareToneMappingRenderPass();
}

bool SceneRenderer::createGraphicsPipeline()
{
    /*if(!this->createSpriteShadowsGenPipeline())
        return (false);*/
    if(!this->createSpriteShadowsPipeline())
        return (false);
    if(!this->createMeshDirectShadowsPipeline())
        return (false);

    if(!this->createShadowMapsBlurPipelines())
        return (false);

    if(!this->createDeferredSpritesPipeline())
        return (false);
    if(!this->createDeferredMeshesPipeline())
        return (false);
    /*if(!this->createAlphaDetectPipeline())
        return (false);
    if(!this->createAlphaDeferredPipeline())
        return (false);
    if(!this->createSsgiBNPipelines())
        return (false);*/
    if(!this->createBentNormalsPipeline())
        return (false);
    if(!this->createLightingPipeline())
        return (false);
    /*if(!this->createAlphaLightingPipeline())
        return (false);
    if(!this->createSsgiLightingPipeline())
        return (false);*/
    if(!this->createAmbientLightingPipeline())
        return (false);
    if(!this->createBloomPipelines())
        return (false);
    if(!this->createToneMappingPipeline())
        return (false);

    return (true);
}

bool SceneRenderer::createAttachments()
{
    uint32_t width      = m_targetWindow->getSwapchainExtent().width;
    uint32_t height     = m_targetWindow->getSwapchainExtent().height;

    if(!VulkanHelpers::createAttachment(width, height, VK_FORMAT_D24_UNORM_S8_UINT,
                                    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, m_deferredDepthAttachment/*[i]*/))
        return (false);

    if(!
            VulkanHelpers::createAttachment(width, height, VK_FORMAT_R8G8B8A8_UNORM,
                                            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, m_albedoAttachment) &
            VulkanHelpers::createAttachment(width, height, VK_FORMAT_R16G16B16A16_SFLOAT,
                                            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, m_positionAttachment) &
            VulkanHelpers::createAttachment(width, height, VK_FORMAT_R16G16B16A16_SFLOAT,
                                            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, m_normalAttachment) &
            VulkanHelpers::createAttachment(width, height, VK_FORMAT_R8G8B8A8_UNORM,
                                            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, m_rmeAttachment) &
            VulkanHelpers::createAttachment(width, height, VK_FORMAT_R16G16B16A16_SFLOAT,
                                            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, m_hdrAttachement) &
            VulkanHelpers::createAttachment(width/2, height/2, VK_FORMAT_R8G8B8A8_UNORM,
                                            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, m_bentNormalsAttachment)
        )
        return (false);

    for(int i = 0 ; i < 2 ; ++i)
        if(! VulkanHelpers::createAttachment(width, height, VK_FORMAT_R16G16B16A16_SFLOAT,
                                            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, m_bloomHdrAttachements[i]) &
             VulkanHelpers::createAttachment(width, height, VK_FORMAT_R8G8B8A8_UNORM,
                                            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, m_bentNormalsBlurAttachments[i])
           )
           return (false);

    /*for(size_t a = 0 ; a < NBR_ALPHA_LAYERS ; ++a)
    {
        if(!
            VulkanHelpers::createAttachment(width, height, VK_FORMAT_R8G8B8A8_UNORM,
                                            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, m_albedoAttachments[a]) &
            VulkanHelpers::createAttachment(width, height, VK_FORMAT_R16G16B16A16_SFLOAT,
                                            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, m_positionAttachments[a]) &
            VulkanHelpers::createAttachment(width, height, VK_FORMAT_R16G16B16A16_SFLOAT,
                                            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, m_normalAttachments[a]) &
            VulkanHelpers::createAttachment(width, height, VK_FORMAT_R8G8B8A8_UNORM,
                                            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, m_rmeAttachments[a]) &
            VulkanHelpers::createAttachment(width, height, VK_FORMAT_R16G16B16A16_SFLOAT,
                                            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, m_hdrAttachements[a])
        )
        return (false);
    }*/

    /*if(!VulkanHelpers::createAttachment(width*SSGI_SIZE_FACTOR, height*SSGI_SIZE_FACTOR, VK_FORMAT_R16G16B16A16_SNORM,
                                        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, m_ssgiBentNormalsAttachment))
        return (false);*/

    /*for(size_t i = 0 ; i < NBR_SSGI_SAMPLES ; ++i)
        if(!VulkanHelpers::createAttachment(width*SSGI_SIZE_FACTOR, height*SSGI_SIZE_FACTOR, VK_FORMAT_R16G16B16A16_UNORM,
                                        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, m_ssgiCollisionsAttachments[i]))
            return (false);

    for(size_t i = 0 ; i < 2 ; ++i)
        if(!VulkanHelpers::createAttachment(width, height, VK_FORMAT_R16G16B16A16_SNORM,
                                            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, m_SSGIBlurBentNormalsAttachments[i]))
            return (false);

    if(!VulkanHelpers::createAttachment(width*SSGI_SIZE_FACTOR, height*SSGI_SIZE_FACTOR, VK_FORMAT_R16G16B16A16_SFLOAT,
                                        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, m_ssgiAccuLightingAttachment))
        return (false);

    for(size_t i = 0 ; i < 2 ; ++i)
        if(!VulkanHelpers::createAttachment(width, height, VK_FORMAT_R16G16B16A16_SFLOAT,
                                            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, m_SSGIBlurLightingAttachments[i]))
            return (false);*/

    return (true);
}

void SceneRenderer::prepareShadowRenderPass()
{
    ///Sprite  shadows tracing
    /*VFramebufferAttachmentType spriteShadowType;
    spriteShadowType.format = VK_FORMAT_R8G8B8A8_UNORM;
    spriteShadowType.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    m_spriteShadowsPass = m_renderGraph.addDynamicRenderPass();
    m_renderGraph.addAttachmentType(m_spriteShadowsPass, spriteShadowType,
                                    VK_ATTACHMENT_STORE_OP_STORE, false);*/

    ///Shadow maps rendering
    VFramebufferAttachmentType shadowMapType0, shadowMapType1;

    shadowMapType0.format = VK_FORMAT_R32G32_SFLOAT;//VK_FORMAT_D32_SFLOAT;//VK_FORMAT_D24_UNORM_S8_UINT;
    shadowMapType0.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    shadowMapType1.format = VK_FORMAT_D16_UNORM;//VK_FORMAT_D32_SFLOAT;//VK_FORMAT_D24_UNORM_S8_UINT;
    shadowMapType1.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;


    m_shadowMapsPass = m_renderGraph.addDynamicRenderPass();

    m_renderGraph.addAttachmentType(m_shadowMapsPass, shadowMapType0,
                                    VK_ATTACHMENT_STORE_OP_STORE, true,
                                    VK_ATTACHMENT_LOAD_OP_CLEAR, true);
    m_renderGraph.addAttachmentType(m_shadowMapsPass, shadowMapType1,
                                    VK_ATTACHMENT_STORE_OP_STORE, true,
                                    VK_ATTACHMENT_LOAD_OP_CLEAR, true);

   // m_renderGraph.setClearValue(m_shadowMapsPass, 0, glm::vec4(1.0), glm::vec2(0.0));
   // m_renderGraph.setClearValue(m_shadowMapsPass, 1, glm::vec4(1.0), glm::vec2(0.0));

    ///Blur

  /*  /// blur horizontal
    m_shadowMapsBlurPasses[0] = m_renderGraph.addDynamicRenderPass();

    m_renderGraph.addAttachmentType(m_shadowMapsBlurPasses[0], shadowMapType0,
                                    VK_ATTACHMENT_STORE_OP_STORE, true,
                                    VK_ATTACHMENT_LOAD_OP_CLEAR, true);
    m_renderGraph.addAttachmentType(m_shadowMapsBlurPasses[0], shadowMapType1,
                                    VK_ATTACHMENT_STORE_OP_STORE, true,
                                    VK_ATTACHMENT_LOAD_OP_CLEAR, true);*/
    /*m_renderGraph.transferAttachmentsToUniforms(m_shadowMapsPass, m_shadowMapsBlurPasses[0], 0);
    m_renderGraph.transferAttachmentsToUniforms(m_shadowMapsPass, m_shadowMapsBlurPasses[0], 1);*/

    /// blur vertical
   /* m_shadowMapsBlurPasses[1] = m_renderGraph.addDynamicRenderPass();

    m_renderGraph.addAttachmentType(m_shadowMapsBlurPasses[1], shadowMapType0,
                                    VK_ATTACHMENT_STORE_OP_STORE, true,
                                    VK_ATTACHMENT_LOAD_OP_CLEAR, true);
    m_renderGraph.addAttachmentType(m_shadowMapsBlurPasses[1], shadowMapType1,
                                    VK_ATTACHMENT_STORE_OP_STORE, true,
                                    VK_ATTACHMENT_LOAD_OP_CLEAR, true);*/
    /*m_renderGraph.transferAttachmentsToUniforms(m_shadowMapsBlurPasses[0], m_shadowMapsBlurPasses[1], 0);
    m_renderGraph.transferAttachmentsToUniforms(m_shadowMapsBlurPasses[0], m_shadowMapsBlurPasses[1], 1);*/
}

void SceneRenderer::prepareDeferredRenderPass()
{
    m_deferredPass = m_renderGraph.addRenderPass(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    m_renderGraph.addNewAttachments(m_deferredPass, m_albedoAttachment);
    m_renderGraph.addNewAttachments(m_deferredPass, m_positionAttachment);
    m_renderGraph.addNewAttachments(m_deferredPass, m_normalAttachment);
    m_renderGraph.addNewAttachments(m_deferredPass, m_rmeAttachment);

    /*m_renderGraph.addNewAttachments(m_deferredPass, m_positionAttachments[0]);
    m_renderGraph.addNewAttachments(m_deferredPass, m_normalAttachments[0]);
    m_renderGraph.addNewAttachments(m_deferredPass, m_rmeAttachments[0]);*/

    m_renderGraph.addNewAttachments(m_deferredPass, m_deferredDepthAttachment);
}

/*void SceneRenderer::prepareAlphaDetectRenderPass()
{
    m_alphaDetectPass = m_renderGraph.addRenderPass(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    m_renderGraph.transferAttachmentsToAttachments(m_deferredPass, m_alphaDetectPass, 1); //We will store existence of truly transparent pixel in position.a
    //m_renderGraph.addNewAttachments(m_alphaDetectPass, m_alphaDetectAttachments, VK_ATTACHMENT_STORE_OP_STORE);//
    //m_renderGraph.transferAttachmentsToAttachments(m_deferredPass, m_alphaDetectPass, 4);
}

void SceneRenderer::prepareAlphaDeferredRenderPass()
{
    m_alphaDeferredPass = m_renderGraph.addRenderPass(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    m_renderGraph.addNewAttachments(m_alphaDeferredPass, m_albedoAttachments[1]);
    m_renderGraph.addNewAttachments(m_alphaDeferredPass, m_positionAttachments[1]);
    m_renderGraph.addNewAttachments(m_alphaDeferredPass, m_normalAttachments[1]);
    m_renderGraph.addNewAttachments(m_alphaDeferredPass, m_rmeAttachments[1]);
    //m_renderGraph.transferAttachmentsToAttachments(m_alphaDetectPass, m_alphaDeferredPass, 1);
    m_renderGraph.transferAttachmentsToAttachments(m_deferredPass, m_alphaDeferredPass, 4);

    m_renderGraph.transferAttachmentsToUniforms(m_alphaDetectPass, m_alphaDeferredPass, 0); //This contains opac position in xyz and truly trans in w
    //m_renderGraph.transferAttachmentsToUniforms(m_deferredPass, m_alphaDeferredPass, 1);
}

void SceneRenderer::prepareSsgiBNRenderPasses()
{
    /// compute bent normals
    m_ssgiBNPass = m_renderGraph.addRenderPass();

    m_renderGraph.addNewAttachments(m_ssgiBNPass, m_ssgiBentNormalsAttachment,
                                    VK_ATTACHMENT_STORE_OP_STORE);

    for(size_t i = 0 ; i < NBR_SSGI_SAMPLES ; ++i)
        m_renderGraph.addNewAttachments(m_ssgiBNPass, m_ssgiCollisionsAttachments[i]);

    m_renderGraph.transferAttachmentsToUniforms(m_alphaDetectPass, m_ssgiBNPass, 0); //Position
    m_renderGraph.transferAttachmentsToUniforms(m_deferredPass, m_ssgiBNPass, 2); //Normal

    /// blur horizontal
    m_ssgiBNBlurPasses[0] = m_renderGraph.addRenderPass();

    m_renderGraph.addNewAttachments(m_ssgiBNBlurPasses[0], m_SSGIBlurBentNormalsAttachments[0]);
    m_renderGraph.transferAttachmentsToUniforms(m_ssgiBNPass        , m_ssgiBNBlurPasses[0], 0);
    m_renderGraph.transferAttachmentsToUniforms(m_alphaDetectPass   , m_ssgiBNBlurPasses[0], 0); //Position
    m_renderGraph.transferAttachmentsToUniforms(m_alphaDeferredPass , m_ssgiBNBlurPasses[0], 1); //Position

    /// blur vertical
    m_ssgiBNBlurPasses[1] = m_renderGraph.addRenderPass();

    m_renderGraph.addNewAttachments(m_ssgiBNBlurPasses[1], m_SSGIBlurBentNormalsAttachments[1]);
    m_renderGraph.transferAttachmentsToUniforms(m_ssgiBNBlurPasses[0], m_ssgiBNBlurPasses[1], 0);
    m_renderGraph.transferAttachmentsToUniforms(m_alphaDetectPass    , m_ssgiBNBlurPasses[1], 0); //Position
    m_renderGraph.transferAttachmentsToUniforms(m_alphaDeferredPass  , m_ssgiBNBlurPasses[1], 1); //Position

}*/

void SceneRenderer::prepareBentNormalsRenderPass()
{
    m_bentNormalsPass = m_renderGraph.addRenderPass();

    m_renderGraph.addNewAttachments(m_bentNormalsPass, m_bentNormalsAttachment, VK_ATTACHMENT_STORE_OP_STORE);

    m_renderGraph.transferAttachmentsToUniforms(m_deferredPass, m_bentNormalsPass, 1);
    m_renderGraph.transferAttachmentsToUniforms(m_deferredPass, m_bentNormalsPass, 2);

    /*** BLUR ***/

    /// blur horizontal
    m_bentNormalsBlurPasses[0] = m_renderGraph.addRenderPass();

    m_renderGraph.addNewAttachments(m_bentNormalsBlurPasses[0], m_bentNormalsBlurAttachments[0]);
    m_renderGraph.transferAttachmentsToUniforms(m_bentNormalsPass        , m_bentNormalsBlurPasses[0], 0);
    m_renderGraph.transferAttachmentsToUniforms(m_deferredPass   , m_bentNormalsBlurPasses[0], 1); //Position

    /// blur vertical
    m_bentNormalsBlurPasses[1] = m_renderGraph.addRenderPass();

    m_renderGraph.addNewAttachments(m_bentNormalsBlurPasses[1], m_bentNormalsBlurAttachments[1]);
    m_renderGraph.transferAttachmentsToUniforms(m_bentNormalsBlurPasses[0], m_bentNormalsBlurPasses[1], 0);
    m_renderGraph.transferAttachmentsToUniforms(m_deferredPass    , m_bentNormalsBlurPasses[1], 1); //Position
}

void SceneRenderer::prepareLightingRenderPass()
{
    m_lightingPass = m_renderGraph.addRenderPass(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    m_renderGraph.addNewAttachments(m_lightingPass, m_hdrAttachement);

    m_renderGraph.transferAttachmentsToUniforms(m_deferredPass, m_lightingPass, 0);
    m_renderGraph.transferAttachmentsToUniforms(m_deferredPass, m_lightingPass, 1);
    m_renderGraph.transferAttachmentsToUniforms(m_deferredPass, m_lightingPass, 2);
    m_renderGraph.transferAttachmentsToUniforms(m_deferredPass, m_lightingPass, 3);
    m_renderGraph.transferAttachmentsToUniforms(m_bentNormalsBlurPasses[1], m_lightingPass, 0);

    /*m_renderGraph.transferAttachmentsToUniforms(m_alphaDetectPass, m_lightingPass, 0);
    m_renderGraph.transferAttachmentsToUniforms(m_deferredPass, m_lightingPass, 2);
    m_renderGraph.transferAttachmentsToUniforms(m_deferredPass, m_lightingPass, 3);*/

   // m_renderGraph.transferAttachmentsToUniforms(m_ssgiBNBlurPasses[1], m_lightingPass, 0);
}

/*void SceneRenderer::prepareAlphaLightingRenderPass()
{
    m_alphaLightingPass = m_renderGraph.addRenderPass(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    m_renderGraph.addNewAttachments(m_alphaLightingPass, m_hdrAttachements[1]);
    m_renderGraph.transferAttachmentsToAttachments(m_alphaDeferredPass, m_alphaLightingPass, 4);

    m_renderGraph.transferAttachmentsToUniforms(m_alphaDeferredPass, m_alphaLightingPass, 0);
    m_renderGraph.transferAttachmentsToUniforms(m_alphaDeferredPass, m_alphaLightingPass, 1);
    m_renderGraph.transferAttachmentsToUniforms(m_alphaDeferredPass, m_alphaLightingPass, 2);
    m_renderGraph.transferAttachmentsToUniforms(m_alphaDeferredPass, m_alphaLightingPass, 3);

    m_renderGraph.transferAttachmentsToUniforms(m_ssgiBNBlurPasses[1], m_alphaLightingPass, 0);

    //m_renderGraph.addNewUniforms(m_alphaLightingPass, m_ambientLightingUbo);
}

void SceneRenderer::prepareSsgiLightingRenderPass()
{

    ///SSGI lighting
    m_ssgiLightingPass = m_renderGraph.addRenderPass();

    m_renderGraph.addNewAttachments(m_ssgiLightingPass, m_ssgiAccuLightingAttachment,
                                    VK_ATTACHMENT_STORE_OP_STORE;

    m_renderGraph.transferAttachmentsToUniforms(m_deferredPass, m_ssgiLightingPass, 0);
    m_renderGraph.transferAttachmentsToUniforms(m_alphaDetectPass, m_ssgiLightingPass, 0);
    m_renderGraph.transferAttachmentsToUniforms(m_deferredPass, m_ssgiLightingPass, 2);
    m_renderGraph.transferAttachmentsToUniforms(m_deferredPass, m_ssgiLightingPass, 3);

    m_renderGraph.transferAttachmentsToUniforms(m_alphaDeferredPass, m_ssgiLightingPass, 0);
    m_renderGraph.transferAttachmentsToUniforms(m_alphaDeferredPass, m_ssgiLightingPass, 1);
    m_renderGraph.transferAttachmentsToUniforms(m_alphaDeferredPass, m_ssgiLightingPass, 2);
    m_renderGraph.transferAttachmentsToUniforms(m_alphaDeferredPass, m_ssgiLightingPass, 3);

    m_renderGraph.transferAttachmentsToUniforms(m_lightingPass, m_ssgiLightingPass, 0);
    m_renderGraph.transferAttachmentsToUniforms(m_alphaLightingPass, m_ssgiLightingPass, 0);

    for(size_t i = 0 ; i < NBR_SSGI_SAMPLES ; ++i)
        m_renderGraph.transferAttachmentsToUniforms(m_ssgiBNPass, m_ssgiLightingPass, 1+i);

    ///Blur

    /// blur horizontal
    m_ssgiLigghtingBlurPasses[0] = m_renderGraph.addRenderPass();

    m_renderGraph.addNewAttachments(m_ssgiLigghtingBlurPasses[0], m_SSGIBlurLightingAttachments[0]);
    m_renderGraph.transferAttachmentsToUniforms(m_ssgiLightingPass  , m_ssgiLigghtingBlurPasses[0], 0);
    m_renderGraph.transferAttachmentsToUniforms(m_alphaDetectPass   , m_ssgiLigghtingBlurPasses[0], 0); //Position
    m_renderGraph.transferAttachmentsToUniforms(m_alphaDeferredPass , m_ssgiLigghtingBlurPasses[0], 1); //Position

    /// blur vertical
    m_ssgiLigghtingBlurPasses[1] = m_renderGraph.addRenderPass();

    m_renderGraph.addNewAttachments(m_ssgiLigghtingBlurPasses[1], m_SSGIBlurLightingAttachments[1]);
    m_renderGraph.transferAttachmentsToUniforms(m_ssgiLigghtingBlurPasses[0], m_ssgiLigghtingBlurPasses[1], 0);
    m_renderGraph.transferAttachmentsToUniforms(m_alphaDetectPass    , m_ssgiLigghtingBlurPasses[1], 0); //Position
    m_renderGraph.transferAttachmentsToUniforms(m_alphaDeferredPass  , m_ssgiLigghtingBlurPasses[1], 1); //Position
}*/

void SceneRenderer::prepareAmbientLightingRenderPass()
{
    m_ambientLightingPass = m_renderGraph.addRenderPass(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    m_renderGraph.transferAttachmentsToAttachments(m_lightingPass, m_ambientLightingPass, 0);
    //m_renderGraph.transferAttachmentsToAttachments(m_alphaLightingPass, m_ambientLightingPass, 0);

    m_renderGraph.transferAttachmentsToUniforms(m_deferredPass, m_ambientLightingPass, 0);
    m_renderGraph.transferAttachmentsToUniforms(m_deferredPass, m_ambientLightingPass, 1);
    m_renderGraph.transferAttachmentsToUniforms(m_deferredPass, m_ambientLightingPass, 2);
    m_renderGraph.transferAttachmentsToUniforms(m_deferredPass, m_ambientLightingPass, 3);
    m_renderGraph.transferAttachmentsToUniforms(m_bentNormalsBlurPasses[1], m_ambientLightingPass, 0);

    /*m_renderGraph.transferAttachmentsToUniforms(m_alphaDeferredPass, m_ambientLightingPass, 0);
    m_renderGraph.transferAttachmentsToUniforms(m_alphaDeferredPass, m_ambientLightingPass, 1);
    m_renderGraph.transferAttachmentsToUniforms(m_alphaDeferredPass, m_ambientLightingPass, 2);
    m_renderGraph.transferAttachmentsToUniforms(m_alphaDeferredPass, m_ambientLightingPass, 3);

    m_renderGraph.transferAttachmentsToUniforms(m_ssgiBNBlurPasses[1], m_ambientLightingPass, 0);
    m_renderGraph.transferAttachmentsToUniforms(m_ssgiLigghtingBlurPasses[1], m_ambientLightingPass, 0);*/


    size_t texturesCount = m_targetWindow->getSwapchainSize();
    std::vector<VkImageView> brdflut(texturesCount, PBRToolbox::getBrdflut().view);
    m_renderGraph.addNewUniforms(m_ambientLightingPass, brdflut);
}

void SceneRenderer::prepareBloomRenderPasses()
{
    m_bloomPass = m_renderGraph.addRenderPass(0);

    m_renderGraph.addNewAttachments(m_bloomPass, m_bloomHdrAttachements[0]);
    m_renderGraph.transferAttachmentsToUniforms(m_ambientLightingPass, m_bloomPass, 0);

    ///Blur 1
    m_bloomBlurPasses[0] = m_renderGraph.addRenderPass(0);

    m_renderGraph.addNewAttachments(m_bloomBlurPasses[0], m_bloomHdrAttachements[1]);
    m_renderGraph.transferAttachmentsToUniforms(m_bloomPass, m_bloomBlurPasses[0] , 0);

    ///Blur 2
    m_bloomBlurPasses[1] = m_renderGraph.addRenderPass(0);

    m_renderGraph.addNewAttachments(m_bloomBlurPasses[1], m_bloomHdrAttachements[0]);
    //m_renderGraph.addAttachmentType(m_bloomBlurPasses[1], m_bloomHdrAttachements[0]);
    m_renderGraph.transferAttachmentsToUniforms(m_bloomBlurPasses[0], m_bloomBlurPasses[1] , 0);

    ///Blur 3
    m_bloomBlurPasses[2] = m_renderGraph.addRenderPass(0);

    m_renderGraph.addNewAttachments(m_bloomBlurPasses[2], m_bloomHdrAttachements[1]);
    m_renderGraph.transferAttachmentsToUniforms(m_bloomBlurPasses[1], m_bloomBlurPasses[2] , 0);

    ///Blur 4
    m_bloomBlurPasses[3] = m_renderGraph.addRenderPass(0);

    m_renderGraph.addNewAttachments(m_bloomBlurPasses[3], m_bloomHdrAttachements[0]);
    m_renderGraph.transferAttachmentsToUniforms(m_bloomBlurPasses[2], m_bloomBlurPasses[3] , 0);
}

void SceneRenderer::prepareToneMappingRenderPass()
{
    m_toneMappingPass = m_renderGraph.addRenderPass(0);

    m_renderGraph.addNewAttachments(m_toneMappingPass, m_targetWindow->getSwapchainAttachments(), VK_ATTACHMENT_STORE_OP_STORE);
    m_renderGraph.addNewAttachments(m_toneMappingPass, m_targetWindow->getSwapchainDepthAttachments(), VK_ATTACHMENT_STORE_OP_STORE);
    m_renderGraph.transferAttachmentsToUniforms(m_ambientLightingPass, m_toneMappingPass, 0);
    m_renderGraph.transferAttachmentsToUniforms(m_bloomBlurPasses[3], m_toneMappingPass, 0);
    //m_renderGraph.transferAttachmentsToUniforms(m_ambientLightingPass, m_toneMappingPass, 1);
}

/*bool SceneRenderer::createSpriteShadowsGenPipeline()
{
    {
        std::ostringstream vertShaderPath,fragShaderPath;
        vertShaderPath << VApp::DEFAULT_SHADERPATH << SPRITE_SHADOWGEN_VERTSHADERFILE;
        fragShaderPath << VApp::DEFAULT_SHADERPATH << SPRITE_SHADOWGEN_FRAGSHADERFILE;

        m_spriteShadowsGenPipeline.createShader(vertShaderPath.str(), VK_SHADER_STAGE_VERTEX_BIT);
        m_spriteShadowsGenPipeline.createShader(fragShaderPath.str(), VK_SHADER_STAGE_FRAGMENT_BIT);

        m_spriteShadowsGenPipeline.addSpecializationDatum(uint32_t(256), 1); //Nbr ray steps
        m_spriteShadowsGenPipeline.addSpecializationDatum(uint32_t(4), 1); //Nbr search steps
        m_spriteShadowsGenPipeline.addSpecializationDatum(0.01f, 1); //Ray collision threshold

        auto bindingDescription = SpriteShadowGenerationDatum::getBindingDescription();
        auto attributeDescriptions = SpriteShadowGenerationDatum::getAttributeDescriptions();
        m_spriteShadowsGenPipeline.setVertexInput(1, &bindingDescription,
                                        attributeDescriptions.size(), attributeDescriptions.data());

        m_spriteShadowsGenPipeline.attachDescriptorSetLayout(m_renderView.getDescriptorSetLayout());
        m_spriteShadowsGenPipeline.attachDescriptorSetLayout(VTexturesManager::descriptorSetLayout());

        if(!m_spriteShadowsGenPipeline.init(m_renderGraph.getRenderPass(m_spriteShadowsPass)))
            return (false);
    }

    {
        std::ostringstream vertShaderPath,fragShaderPath;
        vertShaderPath << VApp::DEFAULT_SHADERPATH << SPRITE_SHADOWFILT_VERTSHADERFILE;
        fragShaderPath << VApp::DEFAULT_SHADERPATH << SPRITE_SHADOWFILT_FRAGSHADERFILE;

        m_spriteShadowFilteringPipeline.createShader(vertShaderPath.str(), VK_SHADER_STAGE_VERTEX_BIT);
        m_spriteShadowFilteringPipeline.createShader(fragShaderPath.str(), VK_SHADER_STAGE_FRAGMENT_BIT);

        auto bindingDescription = SpriteShadowGenerationDatum::getBindingDescription();
        auto attributeDescriptions = SpriteShadowGenerationDatum::getAttributeDescriptions();
        m_spriteShadowFilteringPipeline.setVertexInput(1, &bindingDescription,
                                        attributeDescriptions.size(), attributeDescriptions.data());

        m_spriteShadowFilteringPipeline.attachDescriptorSetLayout(m_renderView.getDescriptorSetLayout());
        m_spriteShadowFilteringPipeline.attachDescriptorSetLayout(VTexturesManager::descriptorSetLayout());

        if(!m_spriteShadowFilteringPipeline.init(m_renderGraph.getRenderPass(m_spriteShadowsPass)))
            return (false);
    }

    return (true);
}*/

bool SceneRenderer::createSpriteShadowsPipeline()
{
    std::ostringstream vertShaderPath,fragShaderPath;
    vertShaderPath << VApp::DEFAULT_SHADERPATH << SPRITE_SHADOW_VERTSHADERFILE;
    fragShaderPath << VApp::DEFAULT_SHADERPATH << SPRITE_SHADOW_FRAGSHADERFILE;

    m_spriteShadowsPipeline.createShader(vertShaderPath.str(), VK_SHADER_STAGE_VERTEX_BIT);
    m_spriteShadowsPipeline.createShader(fragShaderPath.str(), VK_SHADER_STAGE_FRAGMENT_BIT);

    auto bindingDescription = SpriteDatum::getBindingDescription();
    auto attributeDescriptions = SpriteDatum::getAttributeDescriptions();
    m_spriteShadowsPipeline.setVertexInput(1, &bindingDescription,
                                    attributeDescriptions.size(), attributeDescriptions.data());

    m_spriteShadowsPipeline.setInputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP, true);

    m_spriteShadowsPipeline.attachDescriptorSetLayout(m_renderView.getDescriptorSetLayout());
    m_spriteShadowsPipeline.attachDescriptorSetLayout(VTexturesManager::descriptorSetLayout());

    m_spriteShadowsPipeline.attachPushConstant(VK_SHADER_STAGE_VERTEX_BIT , sizeof(glm::vec4));
    m_spriteShadowsPipeline.attachPushConstant(VK_SHADER_STAGE_VERTEX_BIT , sizeof(glm::vec2));
    m_spriteShadowsPipeline.attachPushConstant(VK_SHADER_STAGE_VERTEX_BIT , sizeof(glm::vec2));

    m_spriteShadowsPipeline.setDepthTest(true, true, VK_COMPARE_OP_GREATER);

    return m_spriteShadowsPipeline.init(m_renderGraph.getRenderPass(m_shadowMapsPass));
}

bool SceneRenderer::createMeshDirectShadowsPipeline()
{
    std::ostringstream vertShaderPath,fragShaderPath;
    vertShaderPath << VApp::DEFAULT_SHADERPATH << MESH_DIRECTSHADOW_VERTSHADERFILE;
    fragShaderPath << VApp::DEFAULT_SHADERPATH << MESH_DIRECTSHADOW_FRAGSHADERFILE;

    m_meshDirectShadowsPipeline.createShader(vertShaderPath.str(), VK_SHADER_STAGE_VERTEX_BIT);
    m_meshDirectShadowsPipeline.createShader(fragShaderPath.str(), VK_SHADER_STAGE_FRAGMENT_BIT);

    std::vector<VkVertexInputBindingDescription> bindingDescriptions =
                {  MeshVertex::getBindingDescription(),
                    MeshDatum::getBindingDescription() };

    auto vertexAttributeDescriptions = MeshVertex::getAttributeDescriptions();
    auto instanceAttributeDescriptions = MeshDatum::getAttributeDescriptions();

    std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

    attributeDescriptions.insert(attributeDescriptions.end(),
                                 vertexAttributeDescriptions.begin(),
                                 vertexAttributeDescriptions.end());

    attributeDescriptions.insert(attributeDescriptions.end(),
                                 instanceAttributeDescriptions.begin(),
                                 instanceAttributeDescriptions.end());

    m_meshDirectShadowsPipeline.setVertexInput(bindingDescriptions.size(), bindingDescriptions.data(),
                                            attributeDescriptions.size(), attributeDescriptions.data());

    m_meshDirectShadowsPipeline.setInputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, false);

    m_meshDirectShadowsPipeline.attachDescriptorSetLayout(m_renderView.getDescriptorSetLayout());
    m_meshDirectShadowsPipeline.attachDescriptorSetLayout(VTexturesManager::descriptorSetLayout());

    m_meshDirectShadowsPipeline.attachPushConstant(VK_SHADER_STAGE_VERTEX_BIT , sizeof(glm::vec4));
    m_meshDirectShadowsPipeline.attachPushConstant(VK_SHADER_STAGE_VERTEX_BIT , sizeof(glm::vec2));
    m_meshDirectShadowsPipeline.attachPushConstant(VK_SHADER_STAGE_VERTEX_BIT , sizeof(glm::vec2));

    m_meshDirectShadowsPipeline.setDepthTest(true, true, VK_COMPARE_OP_GREATER);
    //m_meshDirectShadowsPipeline.setCullMode(VK_CULL_MODE_FRONT_BIT);

    return m_meshDirectShadowsPipeline.init(m_renderGraph.getRenderPass(m_shadowMapsPass));
}

bool SceneRenderer::createShadowMapsBlurPipelines()
{
    for(size_t i = 0 ; i < 2 ; ++i)
    {
        std::ostringstream vertShaderPath,fragShaderPath;
        vertShaderPath << VApp::DEFAULT_SHADERPATH << BLUR_VERTSHADERFILE;

        //if(i == 0)
            fragShaderPath << VApp::DEFAULT_SHADERPATH << SHADOWMAPBLUR_FRAGSHADERFILE;
        //else
          //  fragShaderPath << VApp::DEFAULT_SHADERPATH << BLUR_FRAGSHADERFILE;

        m_shadowMapBlurPipelines[i].createShader(vertShaderPath.str(), VK_SHADER_STAGE_VERTEX_BIT);
        m_shadowMapBlurPipelines[i].createShader(fragShaderPath.str(), VK_SHADER_STAGE_FRAGMENT_BIT);

        //m_shadowMapBlurPipelines[i].setSpecializationInfo(specializationInfo, 1);

        //float radius = 4.0f;
        //if(i == 0) radius /= float(m_SSGIBlurBentNormalsAttachments[0].extent.width);
        //if(i == 1) radius /= float(m_SSGIBlurBentNormalsAttachments[0].extent.height);

      //  m_shadowMapBlurPipelines[i].addSpecializationDatum(radius ,1); //Radius
      //  m_shadowMapBlurPipelines[i].addSpecializationDatum(15.0f,1); //Smart thresold


        m_shadowMapBlurPipelines[i].addSpecializationDatum(static_cast<bool>(i),1); //Vertical

        //if(i == 0)
        {
            m_shadowMapBlurPipelines[i].attachPushConstant(VK_SHADER_STAGE_FRAGMENT_BIT , sizeof(glm::uvec2)); //texId
        }

        m_shadowMapBlurPipelines[i].attachPushConstant(VK_SHADER_STAGE_FRAGMENT_BIT , sizeof(float)); //radius

        //m_shadowMapBlurPipelines[i].setStaticExtent(m_targetWindow->getSwapchainExtent());

        //if(i == 0)
            m_shadowMapBlurPipelines[i].attachDescriptorSetLayout(VTexturesManager::descriptorSetLayout(true));
        //else
          //  m_shadowMapBlurPipelines[i].attachDescriptorSetLayout(m_renderGraph.getDescriptorLayout(m_shadowMapsPass));


        //m_shadowMapBlurPipelines[i].setDepthTest(true, true, VK_COMPARE_OP_ALWAYS);

        if(!m_shadowMapBlurPipelines[i].init(m_renderGraph.getRenderPass(m_shadowMapsPass/*m_shadowMapsBlurPasses[0]*/)))
            return (false);
    }

    return (true);
}

bool SceneRenderer::createDeferredSpritesPipeline()
{
    std::ostringstream vertShaderPath,fragShaderPath;
    vertShaderPath << VApp::DEFAULT_SHADERPATH << SPRITE_DEFERRED_VERTSHADERFILE;
    fragShaderPath << VApp::DEFAULT_SHADERPATH << SPRITE_DEFERRED_FRAGSHADERFILE;

    m_deferredSpritesPipeline.createShader(vertShaderPath.str(), VK_SHADER_STAGE_VERTEX_BIT);
    m_deferredSpritesPipeline.createShader(fragShaderPath.str(), VK_SHADER_STAGE_FRAGMENT_BIT);

    auto bindingDescription = SpriteDatum::getBindingDescription();
    auto attributeDescriptions = SpriteDatum::getAttributeDescriptions();
    m_deferredSpritesPipeline.setVertexInput(1, &bindingDescription,
                                    attributeDescriptions.size(), attributeDescriptions.data());

    m_deferredSpritesPipeline.setInputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP, true);

    m_deferredSpritesPipeline.attachDescriptorSetLayout(m_renderView.getDescriptorSetLayout());
    m_deferredSpritesPipeline.attachDescriptorSetLayout(VTexturesManager::descriptorSetLayout());

    m_deferredSpritesPipeline.attachPushConstant(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(glm::vec4));

    m_deferredSpritesPipeline.setDepthTest(true, true, VK_COMPARE_OP_GREATER_OR_EQUAL);
    m_deferredSpritesPipeline.setBlendMode(BlendMode_Alpha,0);
    m_deferredSpritesPipeline.setBlendMode(BlendMode_Alpha,1);
    m_deferredSpritesPipeline.setBlendMode(BlendMode_Alpha,2);
    m_deferredSpritesPipeline.setBlendMode(BlendMode_Alpha,3);

    return m_deferredSpritesPipeline.init(m_renderGraph.getRenderPass(m_deferredPass));
}

bool SceneRenderer::createDeferredMeshesPipeline()
{
    std::ostringstream vertShaderPath,fragShaderPath;
    vertShaderPath << VApp::DEFAULT_SHADERPATH << MESH_DEFERRED_VERTSHADERFILE;
    fragShaderPath << VApp::DEFAULT_SHADERPATH << MESH_DEFERRED_FRAGSHADERFILE;

    m_deferredMeshesPipeline.createShader(vertShaderPath.str(), VK_SHADER_STAGE_VERTEX_BIT);
    m_deferredMeshesPipeline.createShader(fragShaderPath.str(), VK_SHADER_STAGE_FRAGMENT_BIT);

    std::vector<VkVertexInputBindingDescription> bindingDescriptions =
                {  MeshVertex::getBindingDescription(),
                    MeshDatum::getBindingDescription() };

    auto vertexAttributeDescriptions = MeshVertex::getAttributeDescriptions();
    auto instanceAttributeDescriptions = MeshDatum::getAttributeDescriptions();

    std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

    attributeDescriptions.insert(attributeDescriptions.end(),
                                 vertexAttributeDescriptions.begin(),
                                 vertexAttributeDescriptions.end());

    attributeDescriptions.insert(attributeDescriptions.end(),
                                 instanceAttributeDescriptions.begin(),
                                 instanceAttributeDescriptions.end());

    m_deferredMeshesPipeline.setVertexInput(bindingDescriptions.size(), bindingDescriptions.data(),
                                            attributeDescriptions.size(), attributeDescriptions.data());

    m_deferredMeshesPipeline.setInputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, false);

    m_deferredMeshesPipeline.attachDescriptorSetLayout(m_renderView.getDescriptorSetLayout());
    m_deferredMeshesPipeline.attachDescriptorSetLayout(VTexturesManager::descriptorSetLayout());

    m_deferredMeshesPipeline.attachPushConstant(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(glm::vec4));

    m_deferredMeshesPipeline.setDepthTest(true, true, VK_COMPARE_OP_GREATER);

    //m_deferredMeshesPipeline.setCullMode(VK_CULL_MODE_BACK_BIT);
    m_deferredMeshesPipeline.setCullMode(VK_CULL_MODE_NONE);

    return m_deferredMeshesPipeline.init(m_renderGraph.getRenderPass(m_deferredPass));
}

/*bool SceneRenderer::createAlphaDetectPipeline()
{
    std::ostringstream vertShaderPath,fragShaderPath;
    vertShaderPath << VApp::DEFAULT_SHADERPATH << SPRITE_ALPHADETECT_VERTSHADERFILE;
    fragShaderPath << VApp::DEFAULT_SHADERPATH << SPRITE_ALPHADETECT_FRAGSHADERFILE;

    m_alphaDetectPipeline.createShader(vertShaderPath.str(), VK_SHADER_STAGE_VERTEX_BIT);
    m_alphaDetectPipeline.createShader(fragShaderPath.str(), VK_SHADER_STAGE_FRAGMENT_BIT);

    auto bindingDescription = SpriteDatum::getBindingDescription();
    auto attributeDescriptions = SpriteDatum::getAttributeDescriptions();
    m_alphaDetectPipeline.setVertexInput(1, &bindingDescription,
                                    attributeDescriptions.size(), attributeDescriptions.data());

    m_alphaDetectPipeline.setInputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP, true);

    //m_alphaDetectPipeline.setStaticExtent(m_targetWindow->getSwapchainExtent(), true);

    m_alphaDetectPipeline.attachDescriptorSetLayout(m_renderView.getDescriptorSetLayout());
    m_alphaDetectPipeline.attachDescriptorSetLayout(VTexturesManager::descriptorSetLayout());

    m_alphaDetectPipeline.attachPushConstant(VK_SHADER_STAGE_VERTEX_BIT, sizeof(glm::vec4));


    m_alphaDetectPipeline.setWriteMask(VK_COLOR_COMPONENT_A_BIT);

   /// m_alphaDetectPipeline.setDepthTest(false, true, VK_COMPARE_OP_GREATER);


    return m_alphaDetectPipeline.init(m_renderGraph.getRenderPass(m_alphaDetectPass));
}

bool SceneRenderer::createAlphaDeferredPipeline()
{
    std::ostringstream vertShaderPath,fragShaderPath;
    vertShaderPath << VApp::DEFAULT_SHADERPATH << SPRITE_ALPHADEFERRED_VERTSHADERFILE;
    fragShaderPath << VApp::DEFAULT_SHADERPATH << SPRITE_ALPHADEFERRED_FRAGSHADERFILE;

    m_alphaDeferredPipeline.createShader(vertShaderPath.str(), VK_SHADER_STAGE_VERTEX_BIT);
    m_alphaDeferredPipeline.createShader(fragShaderPath.str(), VK_SHADER_STAGE_FRAGMENT_BIT);

    auto bindingDescription = SpriteDatum::getBindingDescription();
    auto attributeDescriptions = SpriteDatum::getAttributeDescriptions();
    m_alphaDeferredPipeline.setVertexInput(1, &bindingDescription,
                                    attributeDescriptions.size(), attributeDescriptions.data());

    m_alphaDeferredPipeline.setInputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP, true);

    //m_alphaDeferredPipeline.setStaticExtent(m_targetWindow->getSwapchainExtent(), true);

    m_alphaDeferredPipeline.attachDescriptorSetLayout(m_renderView.getDescriptorSetLayout());
    m_alphaDeferredPipeline.attachDescriptorSetLayout(VTexturesManager::descriptorSetLayout());
    m_alphaDeferredPipeline.attachDescriptorSetLayout(m_renderGraph.getDescriptorLayout(m_alphaDeferredPass));

    m_alphaDeferredPipeline.attachPushConstant(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(glm::vec4));

    m_alphaDeferredPipeline.setDepthTest(true, true, VK_COMPARE_OP_GREATER_OR_EQUAL);


    VkStencilOpState stencil = {};
    stencil.compareOp   = VK_COMPARE_OP_ALWAYS;
	stencil.failOp      = VK_STENCIL_OP_REPLACE;
	stencil.depthFailOp = VK_STENCIL_OP_REPLACE;//VK_STENCIL_OP_ZERO;
	stencil.passOp      = VK_STENCIL_OP_REPLACE;
	stencil.compareMask = 0xff;
	stencil.writeMask   = 0xff;
    stencil.reference   = 1;
    m_alphaDeferredPipeline.setStencilTest(true, stencil);

    return m_alphaDeferredPipeline.init(m_renderGraph.getRenderPass(m_alphaDeferredPass));
}

bool SceneRenderer::createSsgiBNPipelines()
{
    ///Bent normals computation
    {
        std::ostringstream vertShaderPath,fragShaderPath;
        vertShaderPath << VApp::DEFAULT_SHADERPATH << SSGI_BN_VERTSHADERFILE;
        fragShaderPath << VApp::DEFAULT_SHADERPATH << SSGI_BN_FRAGSHADERFILE;

        m_ssgiBNPipeline.addSpecializationDatum(15.0f, 1); //Ray length
        m_ssgiBNPipeline.addSpecializationDatum(1.0f, 1); //Ray length factor
        m_ssgiBNPipeline.addSpecializationDatum(30.0f, 1); //Ray threshold
        m_ssgiBNPipeline.addSpecializationDatum(2.0f, 1); //Ray threshold factor
        m_ssgiBNPipeline.addSpecializationDatum(SSGI_SIZE_FACTOR, 1);

        m_ssgiBNPipeline.createShader(vertShaderPath.str(), VK_SHADER_STAGE_VERTEX_BIT);
        m_ssgiBNPipeline.createShader(fragShaderPath.str(), VK_SHADER_STAGE_FRAGMENT_BIT);

        VkExtent2D extent = m_targetWindow->getSwapchainExtent();
        extent.width  *= SSGI_SIZE_FACTOR;
        extent.height *= SSGI_SIZE_FACTOR;
        m_ssgiBNPipeline.setStaticExtent(extent);

        m_ssgiBNPipeline.attachDescriptorSetLayout(m_renderView.getDescriptorSetLayout());
        m_ssgiBNPipeline.attachDescriptorSetLayout(m_renderGraph.getDescriptorLayout(m_ssgiBNPass));

        m_ssgiBNPipeline.attachPushConstant(VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(int));

       // m_ssgiBNPipeline.setBlendMode(BlendMode_Accu, 0);

        if(!m_ssgiBNPipeline.init(m_renderGraph.getRenderPass(m_ssgiBNPass)))
            return (false);
    }

    ///Horizontal and vertical blur
    for(size_t i = 0 ; i < 2 ; ++i)
    {
        std::ostringstream vertShaderPath,fragShaderPath;
        vertShaderPath << VApp::DEFAULT_SHADERPATH << BLUR_VERTSHADERFILE;
        fragShaderPath << VApp::DEFAULT_SHADERPATH << BLUR_FRAGSHADERFILE;

        m_ssgiBNBlurPipelines[i].createShader(vertShaderPath.str(), VK_SHADER_STAGE_VERTEX_BIT);
        m_ssgiBNBlurPipelines[i].createShader(fragShaderPath.str(), VK_SHADER_STAGE_FRAGMENT_BIT);

        //m_ssgiBNBlurPipelines[i].setSpecializationInfo(specializationInfo, 1);

        float radius = 4.0f;
        if(i == 0) radius /= float(m_SSGIBlurBentNormalsAttachments[0].extent.width);
        if(i == 1) radius /= float(m_SSGIBlurBentNormalsAttachments[0].extent.height);

        m_ssgiBNBlurPipelines[i].addSpecializationDatum(radius ,1); //Radius
        m_ssgiBNBlurPipelines[i].addSpecializationDatum(15.0f,1); //Smart thresold
        m_ssgiBNBlurPipelines[i].addSpecializationDatum(static_cast<bool>(i),1); //Vertical

        m_ssgiBNBlurPipelines[i].setStaticExtent(m_targetWindow->getSwapchainExtent());

        m_ssgiBNBlurPipelines[i].attachDescriptorSetLayout(m_renderGraph.getDescriptorLayout(m_ssgiBNBlurPasses[i]));

        if(!m_ssgiBNBlurPipelines[i].init(m_renderGraph.getRenderPass(m_ssgiBNBlurPasses[i])))
            return (false);
    }


    return (true);
}*/

bool SceneRenderer::createBentNormalsPipeline()
{
    ///Bent normals computation
    {
        std::ostringstream vertShaderPath,fragShaderPath;
        vertShaderPath << VApp::DEFAULT_SHADERPATH << BENTNORMALS_VERTSHADERFILE;
        fragShaderPath << VApp::DEFAULT_SHADERPATH << BENTNORMALS_FRAGSHADERFILE;

        /**m_bentNormalsPipeline.addSpecializationDatum(15.0f, 1); //Ray length
        m_bentNormalsPipeline.addSpecializationDatum(1.0f, 1); //Ray length factor
        m_bentNormalsPipeline.addSpecializationDatum(30.0f, 1); //Ray threshold
        m_bentNormalsPipeline.addSpecializationDatum(2.0f, 1); //Ray threshold factor
        m_bentNormalsPipeline.addSpecializationDatum(0.5f, 1); //Half screen res**/

        m_bentNormalsPipeline.addSpecializationDatum(15.0f, 1); //Ray length
        m_bentNormalsPipeline.addSpecializationDatum(1.0f, 1); //Ray length factor
        m_bentNormalsPipeline.addSpecializationDatum(30.0f, 1); //Ray threshold
        m_bentNormalsPipeline.addSpecializationDatum(2.0f, 1); //Ray threshold factor
        m_bentNormalsPipeline.addSpecializationDatum(0.5f, 1); //Half screen res

        m_bentNormalsPipeline.createShader(vertShaderPath.str(), VK_SHADER_STAGE_VERTEX_BIT);
        m_bentNormalsPipeline.createShader(fragShaderPath.str(), VK_SHADER_STAGE_FRAGMENT_BIT);

        VkExtent2D extent = m_targetWindow->getSwapchainExtent();
        extent.width  *= 0.5;
        extent.height *= 0.5;
        m_bentNormalsPipeline.setStaticExtent(extent);

        m_bentNormalsPipeline.attachDescriptorSetLayout(m_renderView.getDescriptorSetLayout());
        m_bentNormalsPipeline.attachDescriptorSetLayout(m_renderGraph.getDescriptorLayout(m_bentNormalsPass));

       /// m_bentNormalsPipeline.attachPushConstant(VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(int));

       // m_ssgiBNPipeline.setBlendMode(BlendMode_Accu, 0);

        if(!m_bentNormalsPipeline.init(m_renderGraph.getRenderPass(m_bentNormalsPass)))
            return (false);
    }

    ///Horizontal and vertical blur
    for(size_t i = 0 ; i < 2 ; ++i)
    {
        std::ostringstream vertShaderPath,fragShaderPath;
        vertShaderPath << VApp::DEFAULT_SHADERPATH << SMARTBLUR_VERTSHADERFILE;
        fragShaderPath << VApp::DEFAULT_SHADERPATH << SMARTBLUR_FRAGSHADERFILE;

        m_bentNormalsBlurPipelines[i].createShader(vertShaderPath.str(), VK_SHADER_STAGE_VERTEX_BIT);
        m_bentNormalsBlurPipelines[i].createShader(fragShaderPath.str(), VK_SHADER_STAGE_FRAGMENT_BIT);

        //m_ssgiBNBlurPipelines[i].setSpecializationInfo(specializationInfo, 1);

        float radius = 4.0f;
        if(i == 0) radius /= float(m_bentNormalsBlurAttachments[0].extent.width);
        if(i == 1) radius /= float(m_bentNormalsBlurAttachments[0].extent.height);

        m_bentNormalsBlurPipelines[i].addSpecializationDatum(radius ,1); //Radius
        m_bentNormalsBlurPipelines[i].addSpecializationDatum(15.0f,1); //Smart thresold
        m_bentNormalsBlurPipelines[i].addSpecializationDatum(static_cast<bool>(i),1); //Vertical

        m_bentNormalsBlurPipelines[i].setStaticExtent(m_targetWindow->getSwapchainExtent());

        m_bentNormalsBlurPipelines[i].attachDescriptorSetLayout(m_renderGraph.getDescriptorLayout(m_bentNormalsBlurPasses[i]));

        if(!m_bentNormalsBlurPipelines[i].init(m_renderGraph.getRenderPass(m_bentNormalsBlurPasses[i])))
            return (false);
    }

    return (true);
}

bool SceneRenderer::createLightingPipeline()
{
    std::ostringstream vertShaderPath,fragShaderPath;
    vertShaderPath << VApp::DEFAULT_SHADERPATH << LIGHTING_VERTSHADERFILE;
    fragShaderPath << VApp::DEFAULT_SHADERPATH << LIGHTING_FRAGSHADERFILE;

    m_lightingPipeline.createShader(vertShaderPath.str(), VK_SHADER_STAGE_VERTEX_BIT);
    m_lightingPipeline.createShader(fragShaderPath.str(), VK_SHADER_STAGE_FRAGMENT_BIT);

    //m_lightingPipeline.addSpecializationDatum(/*2*/0.0f, 1); //SSAO intensity
    //m_lightingPipeline.addSpecializationDatum(4.0f, 1); //GIAO intensity

    auto bindingDescription = LightDatum::getBindingDescription();
    auto attributeDescriptions = LightDatum::getAttributeDescriptions();
    m_lightingPipeline.setVertexInput(1, &bindingDescription,
                                    attributeDescriptions.size(), attributeDescriptions.data());

    m_lightingPipeline.setInputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN, false);

    m_lightingPipeline.attachDescriptorSetLayout(m_renderView.getDescriptorSetLayout());
    m_lightingPipeline.attachDescriptorSetLayout(VTexturesManager::descriptorSetLayout(true));
    m_lightingPipeline.attachDescriptorSetLayout(m_renderGraph.getDescriptorLayout(m_lightingPass));

    m_lightingPipeline.attachPushConstant(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(glm::vec4));

    m_lightingPipeline.setBlendMode(BlendMode_Add,0);
    //m_lightingPipeline.setBlendMode(BlendMode_Add,1);

    return m_lightingPipeline.init(m_renderGraph.getRenderPass(m_lightingPass));
}

/*bool SceneRenderer::createAlphaLightingPipeline()
{
    std::ostringstream vertShaderPath,fragShaderPath;
    vertShaderPath << VApp::DEFAULT_SHADERPATH << LIGHTING_VERTSHADERFILE;
    fragShaderPath << VApp::DEFAULT_SHADERPATH << LIGHTING_FRAGSHADERFILE;

    m_alphaLightingPipeline.createShader(vertShaderPath.str(), VK_SHADER_STAGE_VERTEX_BIT);
    m_alphaLightingPipeline.createShader(fragShaderPath.str(), VK_SHADER_STAGE_FRAGMENT_BIT);

    m_alphaLightingPipeline.addSpecializationDatum(2.0f, 1); //SSAO intensity
    m_alphaLightingPipeline.addSpecializationDatum(4.0f, 1); //GIAO intensity


    auto bindingDescription = LightDatum::getBindingDescription();
    auto attributeDescriptions = LightDatum::getAttributeDescriptions();
    m_alphaLightingPipeline.setVertexInput(1, &bindingDescription,
                                    attributeDescriptions.size(), attributeDescriptions.data());

    m_alphaLightingPipeline.setInputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN, false);

    //m_alphaLightingPipeline.setStaticExtent(m_targetWindow->getSwapchainExtent(), true);

    m_alphaLightingPipeline.attachDescriptorSetLayout(m_renderView.getDescriptorSetLayout());
    m_alphaLightingPipeline.attachDescriptorSetLayout(VTexturesManager::descriptorSetLayout());
    m_alphaLightingPipeline.attachDescriptorSetLayout(m_renderGraph.getDescriptorLayout(m_alphaLightingPass));
    //m_deferredSpritesPipeline.attachDescriptorSetLayout(VTexturesManager::descriptorSetLayout());

    m_alphaLightingPipeline.attachPushConstant(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(glm::vec4));

    m_alphaLightingPipeline.setBlendMode(BlendMode_Add,0);

    VkStencilOpState stencil = {};
    stencil.compareOp   = VK_COMPARE_OP_EQUAL;//VK_COMPARE_OP_EQUAL;
	stencil.failOp      = VK_STENCIL_OP_KEEP;
	stencil.depthFailOp = VK_STENCIL_OP_KEEP;
	stencil.passOp      = VK_STENCIL_OP_REPLACE;
	stencil.compareMask = 0xff;
	stencil.writeMask   = 0xff;
    stencil.reference   = 1;
    m_alphaLightingPipeline.setStencilTest(true, stencil);

    return m_alphaLightingPipeline.init(m_renderGraph.getRenderPass(m_alphaLightingPass));
}

bool SceneRenderer::createSsgiLightingPipeline()
{
    {
        std::ostringstream vertShaderPath,fragShaderPath;
        vertShaderPath << VApp::DEFAULT_SHADERPATH << SSGI_LIGHTING_VERTSHADERFILE;
        fragShaderPath << VApp::DEFAULT_SHADERPATH << SSGI_LIGHTING_FRAGSHADERFILE;

        m_ssgiLightingPipeline.createShader(vertShaderPath.str(), VK_SHADER_STAGE_VERTEX_BIT);
        m_ssgiLightingPipeline.createShader(fragShaderPath.str(), VK_SHADER_STAGE_FRAGMENT_BIT);

        //m_ambientLightingPipeline.setStaticExtent(m_targetWindow->getSwapchainExtent(), true);

        m_ssgiLightingPipeline.attachDescriptorSetLayout(m_renderGraph.getDescriptorLayout(m_ssgiLightingPass));

        m_ssgiLightingPipeline.attachPushConstant(VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(glm::vec4));

        if(!m_ssgiLightingPipeline.init(m_renderGraph.getRenderPass(m_ssgiLightingPass)))
            return (false);
    }


    ///Horizontal and vertical blur
    for(size_t i = 0 ; i < 2 ; ++i)
    {
        std::ostringstream vertShaderPath,fragShaderPath;
        vertShaderPath << VApp::DEFAULT_SHADERPATH << BLUR_VERTSHADERFILE;
        fragShaderPath << VApp::DEFAULT_SHADERPATH << BLUR_FRAGSHADERFILE;

        m_ssgiLightingBlurPipelines[i].createShader(vertShaderPath.str(), VK_SHADER_STAGE_VERTEX_BIT);
        m_ssgiLightingBlurPipelines[i].createShader(fragShaderPath.str(), VK_SHADER_STAGE_FRAGMENT_BIT);

        //m_ssgiBNBlurPipelines[i].setSpecializationInfo(specializationInfo, 1);

        float radius = 4.0f;
        if(i == 0) radius /= float(m_SSGIBlurLightingAttachments[0].extent.width);
        if(i == 1) radius /= float(m_SSGIBlurLightingAttachments[0].extent.height);

        m_ssgiLightingBlurPipelines[i].addSpecializationDatum(radius ,1); //Radius
        m_ssgiLightingBlurPipelines[i].addSpecializationDatum(15.0f,1); //Smart thresold
        m_ssgiLightingBlurPipelines[i].addSpecializationDatum(static_cast<bool>(i),1); //Vertical

        m_ssgiLightingBlurPipelines[i].setStaticExtent(m_targetWindow->getSwapchainExtent());

        m_ssgiLightingBlurPipelines[i].attachDescriptorSetLayout(m_renderGraph.getDescriptorLayout(m_ssgiLigghtingBlurPasses[i]));

        if(!m_ssgiLightingBlurPipelines[i].init(m_renderGraph.getRenderPass(m_ssgiLigghtingBlurPasses[i])))
            return (false);
    }

    return (true);
}*/

bool SceneRenderer::createAmbientLightingPipeline()
{
    std::ostringstream vertShaderPath,fragShaderPath;
    vertShaderPath << VApp::DEFAULT_SHADERPATH << AMBIENTLIGHTING_VERTSHADERFILE;
    fragShaderPath << VApp::DEFAULT_SHADERPATH << AMBIENTLIGHTING_FRAGSHADERFILE;

    //m_ambientLightingPipeline.addSpecializationDatum(PBRToolbox::ENVMAP_FILTERINGMIPSCOUNT, 1);
    //m_ambientLightingPipeline.addSpecializationDatum(2.0f, 1); //SSAO intensity
    //m_ambientLightingPipeline.addSpecializationDatum(1.0f, 1); //GIAO intensity


    m_ambientLightingPipeline.createShader(vertShaderPath.str(), VK_SHADER_STAGE_VERTEX_BIT);
    m_ambientLightingPipeline.createShader(fragShaderPath.str(), VK_SHADER_STAGE_FRAGMENT_BIT);

    //m_ambientLightingPipeline.setStaticExtent(m_targetWindow->getSwapchainExtent(), true);

    m_ambientLightingPipeline.attachDescriptorSetLayout(m_renderGraph.getDescriptorLayout(m_ambientLightingPass));
    m_ambientLightingPipeline.attachDescriptorSetLayout(SceneRenderingData::ambientLightingDescSetLayout());
    //m_ambientLightingPipeline.attachDescriptorSetLayout(VTexturesManager::descriptorSetLayout());

    m_ambientLightingPipeline.attachPushConstant(VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(glm::vec4));

    m_ambientLightingPipeline.setBlendMode(BlendMode_Add,0);
    //m_ambientLightingPipeline.setBlendMode(BlendMode_Add,1);

    return m_ambientLightingPipeline.init(m_renderGraph.getRenderPass(m_ambientLightingPass));
}


bool SceneRenderer::createBloomPipelines()
{
    bool r = true;

    {
        std::ostringstream vertShaderPath,fragShaderPath;
        vertShaderPath << VApp::DEFAULT_SHADERPATH << BLOOM_VERTSHADERFILE;
        fragShaderPath << VApp::DEFAULT_SHADERPATH << BLOOM_FRAGSHADERFILE;

        m_bloomPipeline.createShader(vertShaderPath.str(), VK_SHADER_STAGE_VERTEX_BIT);
        m_bloomPipeline.createShader(fragShaderPath.str(), VK_SHADER_STAGE_FRAGMENT_BIT);

        m_bloomPipeline.setStaticExtent(m_targetWindow->getSwapchainExtent());

        m_bloomPipeline.setBlendMode(BlendMode_None);

        m_bloomPipeline.attachDescriptorSetLayout(m_renderGraph.getDescriptorLayout(m_bloomPass));

        r = r & m_bloomPipeline.init(m_renderGraph.getRenderPass(m_bloomPass));
    }

    for(size_t i = 0 ; i < 2 ; ++i)
    {
        std::ostringstream vertShaderPath,fragShaderPath;
        vertShaderPath << VApp::DEFAULT_SHADERPATH << BLUR_VERTSHADERFILE;
        fragShaderPath << VApp::DEFAULT_SHADERPATH << BLUR_FRAGSHADERFILE;

        m_bloomBlurPipelines[i].createShader(vertShaderPath.str(), VK_SHADER_STAGE_VERTEX_BIT);
        m_bloomBlurPipelines[i].createShader(fragShaderPath.str(), VK_SHADER_STAGE_FRAGMENT_BIT);


        m_bloomBlurPipelines[i].addSpecializationDatum(static_cast<bool>(i),1); //Vertical
        m_bloomBlurPipelines[i].attachPushConstant(VK_SHADER_STAGE_FRAGMENT_BIT , sizeof(float)); //radius

        m_bloomBlurPipelines[i].setStaticExtent(m_targetWindow->getSwapchainExtent());
        m_bloomBlurPipelines[i].attachDescriptorSetLayout(m_renderGraph.getDescriptorLayout(m_bloomBlurPasses[i]));

        r = r & m_bloomBlurPipelines[i].init(m_renderGraph.getRenderPass(m_bloomBlurPasses[i]));
    }

    return r;
}


bool SceneRenderer::createToneMappingPipeline()
{
    std::ostringstream vertShaderPath,fragShaderPath;
    vertShaderPath << VApp::DEFAULT_SHADERPATH << TONEMAPPING_VERTSHADERFILE;
    fragShaderPath << VApp::DEFAULT_SHADERPATH << TONEMAPPING_FRAGSHADERFILE;

    m_toneMappingPipeline.createShader(vertShaderPath.str(), VK_SHADER_STAGE_VERTEX_BIT);
    m_toneMappingPipeline.createShader(fragShaderPath.str(), VK_SHADER_STAGE_FRAGMENT_BIT);

    m_toneMappingPipeline.setStaticExtent(m_targetWindow->getSwapchainExtent());

    m_toneMappingPipeline.setBlendMode(BlendMode_None);

    //m_toneMappingPipeline.attachDescriptorSetLayout(m_hdrDescriptorSetLayout);
    m_toneMappingPipeline.attachDescriptorSetLayout(m_renderGraph.getDescriptorLayout(m_toneMappingPass));

    return m_toneMappingPipeline.init(m_renderGraph.getRenderPass(m_toneMappingPass));
}

void SceneRenderer::cleanup()
{
    for(auto pingPong : m_shadowMapBlurPingPongs)
        VTexturesManager::freeTexture(pingPong.second);
    m_shadowMapBlurPingPongs.clear();

    /*for(auto vbo : m_spriteShadowGenerationVbos)
        delete vbo;
    m_spriteShadowGenerationVbos.clear();

    for(auto vbo : m_spriteShadowsVbos)
        delete vbo;
    m_spriteShadowsVbos.clear();*/

    for(auto vbo : m_spritesVbos)
        delete vbo;
    m_spritesVbos.clear();

    for(auto meshVboMap : m_meshesVbos)
        for(auto meshVbo : meshVboMap)
            delete meshVbo.second;
    m_meshesVbos.clear();

    VulkanHelpers::destroyAttachment(m_deferredDepthAttachment);
    VulkanHelpers::destroyAttachment(m_albedoAttachment);
    VulkanHelpers::destroyAttachment(m_positionAttachment);
    VulkanHelpers::destroyAttachment(m_normalAttachment);
    VulkanHelpers::destroyAttachment(m_rmeAttachment);
    VulkanHelpers::destroyAttachment(m_bentNormalsAttachment);
    VulkanHelpers::destroyAttachment(m_bentNormalsBlurAttachments[0]);
    VulkanHelpers::destroyAttachment(m_bentNormalsBlurAttachments[1]);
    VulkanHelpers::destroyAttachment(m_bloomHdrAttachements[0]);
    VulkanHelpers::destroyAttachment(m_bloomHdrAttachements[1]);
    VulkanHelpers::destroyAttachment(m_hdrAttachement);

    /*for(size_t a = 0 ; a < NBR_ALPHA_LAYERS ; ++a)
    {
        VulkanHelpers::destroyAttachment(m_albedoAttachments[a]);
        VulkanHelpers::destroyAttachment(m_positionAttachments[a]);
        VulkanHelpers::destroyAttachment(m_normalAttachments[a]);
        VulkanHelpers::destroyAttachment(m_rmeAttachments[a]);
        VulkanHelpers::destroyAttachment(m_hdrAttachements[a]);
    }*/

    /*VulkanHelpers::destroyAttachment(m_ssgiBentNormalsAttachment);
    VulkanHelpers::destroyAttachment(m_ssgiAccuLightingAttachment);
    for(size_t i = 0 ; i < NBR_SSGI_SAMPLES ; ++i)
        VulkanHelpers::destroyAttachment(m_ssgiCollisionsAttachments[i]);
    for(size_t i = 0 ; i < 2 ; ++i)
        VulkanHelpers::destroyAttachment(m_SSGIBlurBentNormalsAttachments[i]);
    for(size_t i = 0 ; i < 2 ; ++i)
        VulkanHelpers::destroyAttachment(m_SSGIBlurLightingAttachments[i]);*/

    /*m_spriteShadowsGenPipeline.destroy();
    m_spriteShadowFilteringPipeline.destroy();*/
    m_spriteShadowsPipeline.destroy();
    m_meshDirectShadowsPipeline.destroy();

    m_shadowMapBlurPipelines[0].destroy();
    m_shadowMapBlurPipelines[1].destroy();

    m_deferredSpritesPipeline.destroy();
    m_deferredMeshesPipeline.destroy();
    /*m_alphaDetectPipeline.destroy();
    m_alphaDeferredPipeline.destroy();
    m_ssgiBNPipeline.destroy();*/
    m_bentNormalsPipeline.destroy();
    m_bentNormalsBlurPipelines[0].destroy();
    m_bentNormalsBlurPipelines[1].destroy();
    m_lightingPipeline.destroy();
    //m_ssgiLightingPipeline.destroy();
    m_ambientLightingPipeline.destroy();

    m_bloomPipeline.destroy();
    m_bloomBlurPipelines[0].destroy();
    m_bloomBlurPipelines[1].destroy();

    m_toneMappingPipeline.destroy();

    SceneRenderingData::cleanStatic();

    AbstractRenderer::cleanup();
}


}
