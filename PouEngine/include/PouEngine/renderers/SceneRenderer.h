/**
* This class create the graphic pipeline necessary to render scenes.
**/

#ifndef SCENERENDERER_H
#define SCENERENDERER_H

#include "PouEngine/renderers/AbstractRenderer.h"
#include "PouEngine/renderers/RenderGraph.h"

#include "PouEngine/scene/Scene.h"
#include "PouEngine/scene/SpriteEntity.h"
#include "PouEngine/scene/MeshEntity.h"
#include "PouEngine/scene/LightEntity.h"

#include "PouEngine/vulkanImpl/DynamicVBO.h"

namespace pou
{

struct ShadowMapRenderingInstance
{
    /*glm::vec4 lightPosition;
    glm::vec2 shadowShift;*/
    LightDatum lightDatum;
    glm::vec2 extent;
    size_t    spritesVboSize;
    size_t    spritesVboOffset;
    std::unordered_map<VMesh*, std::pair<size_t,size_t> > meshesVboSizeAndOffset;

    VRenderableTexture *pingPongTex;
};

class SceneRenderer : public AbstractRenderer
{
    public:
        SceneRenderer(RenderWindow *targetWindow, RendererName name, RenderereOrder order);
        virtual ~SceneRenderer();

        virtual void update(size_t frameIndex) override;

        void addRenderingInstance(SceneRenderingInstance *renderingInstance);

        //I'll need light position and so forth (maybe I'll use push constants, it's like cam pos)...
        void addShadowMapToRender(VRenderTarget* shadowMap, const LightDatum &datum);
        /*void addSpriteShadowToRender(VRenderTarget* spriteShadow, const SpriteShadowGenerationDatum &datum);
        void addToSpriteShadowsVbo(const SpriteShadowDatum &datum);*/
        void addToSpriteShadowsVbo(const SpriteDatum &datum);
        void addToMeshShadowsVbo(VMesh *mesh, const MeshDatum &datum);

        void addToSpritesVbo(const SpriteDatum &datum);
        void addToMeshesVbo(VMesh *mesh, const MeshDatum &datum);
        void addToLightsVbo(const LightDatum &datum);

        size_t getSpritesVboSize();
        size_t getMeshesVboSize(VMesh *mesh);
        size_t getLightsVboSize();

        //VRenderPass *getSpriteShadowsRenderPass();
        VRenderPass *getShadowMapsRenderPass();

    protected:
        virtual bool init();
        virtual void cleanup();

        virtual void prepareRenderPass() override;
        virtual bool createGraphicsPipeline() override;

        bool createAttachments();


        ///Render passes
        void prepareShadowRenderPass();
        //Deferred
        void prepareDeferredRenderPass();
        /*void prepareAlphaDetectRenderPass();
        void prepareAlphaDeferredRenderPass();*/
        //Lighting
        /*void prepareSsgiBNRenderPasses();*/
        void prepareLightingRenderPass();
        /*void prepareAlphaLightingRenderPass();
        void prepareSsgiLightingRenderPass();*/
        void prepareAmbientLightingRenderPass();
        //Final pass
        void prepareToneMappingRenderPass();

        ///Pipelines
        /*bool createSpriteShadowsGenPipeline();*/
        bool createSpriteShadowsPipeline();
        bool createMeshDirectShadowsPipeline();
        bool createShadowMapsBlurPipelines();
        //Deferred
        bool createDeferredSpritesPipeline();
        bool createDeferredMeshesPipeline();
        /*bool createAlphaDetectPipeline();
        bool createAlphaDeferredPipeline();*/
        //Lighting
        /*bool createSsgiBNPipelines();*/
        bool createLightingPipeline();
        /*bool createAlphaLightingPipeline();
        bool createSsgiLightingPipeline();*/
        bool createAmbientLightingPipeline();
        //Tone mapping
        bool createToneMappingPipeline();

        virtual bool    recordPrimaryCmb(uint32_t imageIndex) override;

        virtual void    uploadVbos();

        virtual bool    recordShadowCmb(uint32_t imageIndex);
        virtual bool    recordDeferredCmb(uint32_t imageIndex);
        virtual bool    recordLightingCmb(uint32_t imageIndex);
        /*virtual bool    recordSsgiBnCmb(uint32_t imageIndex);*/
        virtual bool    recordAmbientLightingCmb(uint32_t imageIndex);
        virtual bool    recordToneMappingCmb(uint32_t imageIndex);

    private:
        VGraphicsPipeline   /*m_spriteShadowsGenPipeline,
                            m_spriteShadowFilteringPipeline,*/
                            //m_spriteShadowsBlurPipelines[2],
                            m_spriteShadowsPipeline,
                            m_meshDirectShadowsPipeline,
                            m_shadowMapBlurPipelines[2];

        VGraphicsPipeline   m_deferredSpritesPipeline,
                            m_deferredMeshesPipeline/*,
                            m_alphaDetectPipeline,
                            m_alphaDeferredPipeline*/;

        VGraphicsPipeline   /*m_ssgiBNPipeline,
                            m_ssgiBNBlurPipelines[2],*/
                            m_lightingPipeline,
                            /*m_alphaLightingPipeline,
                            m_ssgiLightingPipeline,
                            m_ssgiLightingBlurPipelines[2],*/
                            m_ambientLightingPipeline,
                            m_toneMappingPipeline;

        //std::vector<std::list<VRenderableTexture> > m_spriteShadowBufs;

        /**VFramebufferAttachment m_deferredDepthAttachment;
        VFramebufferAttachment m_albedoAttachments[NBR_ALPHA_LAYERS],
                                            m_positionAttachments[NBR_ALPHA_LAYERS], //The opac.a contains existence of truly trasparent frag, the alpha.a contains alphaAlbedo.a
                                            m_normalAttachments[NBR_ALPHA_LAYERS], //The opac.a = 0 and alpha.a contains existence of truly transparent frag
                                            m_rmeAttachments[NBR_ALPHA_LAYERS];
        VFramebufferAttachment m_hdrAttachements[NBR_ALPHA_LAYERS];

        VFramebufferAttachment m_ssgiBentNormalsAttachment;
        VFramebufferAttachment m_ssgiAccuLightingAttachment;
        VFramebufferAttachment m_ssgiCollisionsAttachments[NBR_SSGI_SAMPLES];
        VFramebufferAttachment m_SSGIBlurBentNormalsAttachments[2];
        VFramebufferAttachment m_SSGIBlurLightingAttachments[2];**/

        VFramebufferAttachment m_deferredDepthAttachment;
        VFramebufferAttachment m_albedoAttachment,
                               m_positionAttachment,
                               m_normalAttachment,
                               m_rmeAttachment;
        VFramebufferAttachment m_hdrAttachement;

        size_t  m_shadowMapsPass,
                m_deferredPass,
                m_lightingPass,
                m_ambientLightingPass,
                m_toneMappingPass;


        ///I should maybe sort by material (?)
        std::vector<DynamicVBO<SpriteDatum>*>                   m_spritesVbos;
        std::vector<std::unordered_map<VMesh* ,DynamicVBO<MeshDatum>*> >  m_meshesVbos;
        std::vector<DynamicVBO<LightDatum>*>                    m_lightsVbos;

        std::list<SceneRenderingInstance*>      m_renderingInstances;
        std::list<ShadowMapRenderingInstance>   m_shadowMapsInstances;

        std::map<VTextureFormat, VRenderableTexture> m_shadowMapBlurPingPongs;

        static const char *SPRITE_SHADOW_VERTSHADERFILE;
        static const char *SPRITE_SHADOW_FRAGSHADERFILE;
        static const char *MESH_DIRECTSHADOW_VERTSHADERFILE;
        static const char *MESH_DIRECTSHADOW_FRAGSHADERFILE;

        static const char *BLUR_VERTSHADERFILE;
        static const char *SHADOWMAPBLUR_FRAGSHADERFILE;
        static const char *BLUR_FRAGSHADERFILE;

        static const char *SPRITE_DEFERRED_VERTSHADERFILE;
        static const char *SPRITE_DEFERRED_FRAGSHADERFILE;
        static const char *MESH_DEFERRED_VERTSHADERFILE;
        static const char *MESH_DEFERRED_FRAGSHADERFILE;
        static const char *LIGHTING_VERTSHADERFILE;
        static const char *LIGHTING_FRAGSHADERFILE;
        static const char *AMBIENTLIGHTING_VERTSHADERFILE;
        static const char *AMBIENTLIGHTING_FRAGSHADERFILE;
        static const char *TONEMAPPING_VERTSHADERFILE;
        static const char *TONEMAPPING_FRAGSHADERFILE;
};

}

#endif // SCENERENDERER_H
