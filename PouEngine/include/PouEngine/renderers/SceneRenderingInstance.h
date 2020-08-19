#ifndef SCENERENDERINGINSTANCE_H
#define SCENERENDERINGINSTANCE_H

#include "PouEngine/renderers/SceneRenderingData.h"

namespace pou
{

class SceneRenderingInstance
{
    public:
        SceneRenderingInstance(SceneRenderingData*, SceneRenderer*);

        void drawRectangle(glm::vec3 pos, glm::vec2 size, glm::vec4 color=glm::vec4(1.0f));

        void addToSpritesVbo(const SpriteDatum &datum);
        void addToSpritesOrdered(const SpriteDatum &datum, float weight);
        void addToMeshesVbo(VMesh *mesh, const MeshDatum &datum);
        void addToLightsVbo(const LightDatum &datum);

        void addToShadowLightsList(LightEntity *entity);
        void addToShadowCastersList(ShadowCaster *caster);

        void setViewInfo(const ViewInfo &viewInfo, glm::vec3 camPos, float camZoom);

        SceneRenderingData *getRenderingData();

        size_t getSpritesVboSize();
        size_t getSpritesVboOffset();
        size_t getMeshesVboSize(VMesh *mesh);
        size_t getMeshesVboOffset(VMesh *mesh);
        size_t getLightsVboSize();
        size_t getLightsVboOffset();

        const ViewInfo &getViewInfo();
        glm::vec4       getCamPosAndZoom();

        void computeOrdering();

        void prepareShadowsRendering(SceneRenderer *renderer, uint32_t imageIndex);
        void pushCamPosAndZoom(VkCommandBuffer cmb, VkPipelineLayout layout, VkFlags shaderStageBit = VK_SHADER_STAGE_VERTEX_BIT);

    protected:
        SceneRenderer       *m_renderer;
        SceneRenderingData  *m_renderingData;

        ViewInfo    m_viewInfo;
        glm::vec3   m_camPos;
        float       m_camZoom;

        size_t      m_spritesVboSize;
        size_t      m_spritesVboOffset;
        std::unordered_map<VMesh*, size_t>    m_meshesVboSize;
        std::unordered_map<VMesh*, size_t>    m_meshesVboOffset;
        size_t      m_lightsVboSize;
        size_t      m_lightsVboOffset;

        std::multimap<float, SpriteDatum> m_spritesOrdered;

        std::vector<ShadowCaster*>  m_renderedShadowCasters[2]; //Objects that can cast shadows, 0 is omni, 1 is directional
        std::list<LightEntity*>     m_renderedShadowLights; //Lights than can cast shadows
};

}

#endif // SCENERENDERINGINSTANCE_H
