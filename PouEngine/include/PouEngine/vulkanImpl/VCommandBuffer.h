#ifndef VCOMMANDBUFFER_H
#define VCOMMANDBUFFER_H

#include "PouEngine/vulkanImpl/VulkanHelpers.h"
#include "PouEngine/vulkanImpl/VRenderPass.h"
#include "PouEngine/vulkanImpl/VRenderTarget.h"

namespace pou
{

class VCommandBuffer
{
    public:
        VCommandBuffer();
        virtual ~VCommandBuffer();

        VCommandBuffer( const VCommandBuffer& ) = delete;
        VCommandBuffer& operator=( const VCommandBuffer& ) = delete;

        bool init(size_t buffersCount);

        void    setCmbUsage(VkFlags usage);
        VkFlags getCmbUsage();

        const VkCommandBuffer *getVkCommandBuffer(size_t cmbIndex);

        VkCommandBuffer startRecording(size_t cmbIndex);
        VkCommandBuffer startRecording(size_t cmbIndex, size_t framebufferIndex, VkSubpassContents contents,
                                       VRenderPass* renderPass, VRenderTarget *renderTarget);
        void            nextRenderPass(size_t framebufferIndex, VkSubpassContents contents,
                                       VRenderPass* renderPass, VRenderTarget *renderTarget);
        bool            endRecording();

    protected:
        bool createBuffers();

    protected:
        size_t      m_buffersCount;
        VkFlags     m_cmbUsage;

        size_t      m_lastRecording;
        bool        m_inRenderPass;

        std::vector<VkCommandBuffer> m_cmbs;
};

}

#endif // VCOMMANDBUFFER_H
