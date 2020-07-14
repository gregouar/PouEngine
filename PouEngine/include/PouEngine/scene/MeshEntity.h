#ifndef MESHENTITY_H
#define MESHENTITY_H

#include <Vulkan/vulkan.h>
#include <array>

#include "PouEngine/scene/ShadowCaster.h"

namespace pou
{

struct MeshDatum
{
    glm::vec4 model_0;
    glm::vec4 model_1;
    glm::vec4 model_2;
    glm::vec4 model_3;

    glm::vec4 albedo_color;
    glm::vec3 rme_color;

    glm::uvec2 albedo_texId;
    glm::uvec2 height_texId;
    glm::uvec2 normal_texId;
    glm::uvec2 rme_texId;

    float texThickness;

    static VkVertexInputBindingDescription getBindingDescription();
    static std::array<VkVertexInputAttributeDescription, 11> getAttributeDescriptions();
};

class MeshEntity : public ShadowCaster
{
    friend class SceneNode;

    public:
        MeshEntity();
        virtual ~MeshEntity();

        virtual std::shared_ptr<SceneObject> createCopy();

        bool setMeshModel(AssetTypeId meshId);
        bool setMeshModel(MeshAsset* mesh);

        void setColor(Color color);
        void setRme(glm::vec3 rme);

        void setScale(float scale);
        void setScale(glm::vec3 scale);

        MeshDatum getMeshDatum();
        glm::vec3 getScale();

        MeshAsset *getMeshModel();

        virtual void notify(NotificationSender* , int notificationType,
                            void* data = nullptr) override;

        virtual void generateRenderingData(SceneRenderingInstance *renderingInstance);

        virtual glm::vec2 castShadow(SceneRenderer *renderer, LightEntity* light) override;


    protected:
        void cleanup();
        virtual void updateDatum();

    protected:
        MeshAsset *m_mesh;
        MeshDatum  m_datum;

        Color       m_color;
        glm::vec3   m_rme;

        glm::vec3   m_scale;
};

}


#endif // MESHENTITY_H
