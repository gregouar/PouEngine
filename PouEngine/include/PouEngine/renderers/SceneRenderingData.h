#ifndef SCENERENDERINGDATA_H
#define SCENERENDERINGDATA_H

#include "PouEngine/scene/SpriteEntity.h"
#include "PouEngine/scene/MeshEntity.h"
#include "PouEngine/scene/LightEntity.h"
#include "PouEngine/renderers/RenderView.h"

namespace pou
{

class SceneRenderer;
class SceneRenderingData;

struct AmbientLightingData
{
    glm::vec4 ambientLight;
};

class SceneRenderingData
{
    public:
        SceneRenderingData();
        virtual ~SceneRenderingData();

        bool init(SceneRenderer *renderer);
        void cleanup();

        bool isInitialized();

        //void update();

        void setAmbientLight(Color color);

        VkDescriptorSet getAmbientLightingDescSet(size_t frameIndex);

        static VkDescriptorSetLayout ambientLightingDescSetLayout();
        static void cleanStatic();

    protected:
        bool createBuffers(size_t framesCount);
        static bool createDescriptorSetLayout();
        bool createDescriptorPool(size_t framesCount);
        bool createDescriptorSets(size_t framesCount);

        void updateAmbientLightingDescSet(size_t frameIndex);
        void updateAmbientLightingUbo(size_t frameIndex);

    protected:
        bool m_isInitialized;

        AmbientLightingData     m_ambientLightingData;
        std::vector<bool>       m_needToUpdateAmbientLightingUbos;
        std::vector<VBuffer>    m_ambientLightingUbos;


        VkDescriptorPool                m_descriptorPool;

        std::vector<bool>               m_needToUpdateAmbientLightingDescSets;
        std::vector<VkDescriptorSet>    m_ambientLightingDescSets;

        static VkDescriptorSetLayout    s_ambientLightingDescSetLayout;
};

}

#endif // SCENERENDERINGDATA_H
