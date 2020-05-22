#ifndef ABSTRACTRENDERER_H
#define ABSTRACTRENDERER_H

#include "PouEngine/renderers/RenderWindow.h"
#include "PouEngine/renderers/RenderView.h"
#include "PouEngine/renderers/RenderGraph.h"

namespace pou
{

struct PingPongFormatsList
{
    std::vector<VTextureFormat> formats;
    VRenderPass* renderPass;

    bool operator==(const PingPongFormatsList &rhs) const
    {
        if(renderPass != rhs.renderPass)
            return (false);

        if(formats.size() != rhs.formats.size())
            return (false);

        for(size_t i = 0 ; i < formats.size() ; ++i)
            if(!(rhs.formats[i] == formats[i]))
                return (false);

        return (true);
    }

    bool operator<(const PingPongFormatsList &rhs) const
    {
        if(renderPass < rhs.renderPass)
            return (true);
        if(renderPass > rhs.renderPass)
            return (false);

        if(formats.size() < rhs.formats.size())
            return (true);
        if(formats.size() > rhs.formats.size())
            return (false);

        for(size_t i = 0 ; i < formats.size() ; ++i)
        {
            if(formats[i] < rhs.formats[i])
                return (true);
            if(rhs.formats[i] < formats[i])
                return (false);
        }

        return (false);
    }
};

class AbstractRenderer
{
    friend class RenderWindow;

    public:
        AbstractRenderer(RenderWindow *targetWindow, RendererName name, RenderereOrder order);
        virtual ~AbstractRenderer();

        virtual void update(size_t frameIndex);
        virtual void render(uint32_t imageIndex);

        virtual void setView(ViewInfo viewInfo);

        virtual std::vector<FullRenderPass*> getFinalPasses();

        size_t getFramesCount();

        RendererName    getName();

    protected:
        virtual void    prepareRenderPass();

        virtual bool    initRenderGraph();
        virtual bool    createGraphicsPipeline() = 0;
        virtual bool    createRenderView();

        /*virtual bool    createDescriptorSetLayouts();
        virtual bool    createDescriptorPool();
        virtual bool    createDescriptorSets();*/

        virtual bool    init();
        virtual bool    reinit();
        virtual void    cleanup();

        virtual bool    recordPrimaryCmb(uint32_t imageIndex) = 0;

        //VFramebufferAttachment *getPingPongAttachment(const VTextureFormat &format);
        VRenderTarget *getPingPongRenderTarget(std::vector<VTextureFormat> &formats, VRenderPass *renderPass);


    protected:
        //bool m_useDynamicView;

        RenderWindow   *m_targetWindow;
        RenderView      m_renderView;
        RenderGraph     m_renderGraph;
        size_t          m_defaultPass;

        /*std::vector<VkDescriptorPoolSize>   m_descriptorPoolSizes;
        VkDescriptorPool                    m_descriptorPool;*/

        //I should remove it and use targetWindow->getFrameIndex();
        size_t          m_curFrameIndex;

    private:
        RenderereOrder  m_order;
        RendererName    m_name;

        std::vector<FullRenderPass*> m_finalPasses;

        //std::map<VTextureFormat, VFramebufferAttachment> m_pingPongAttachments;
        std::map<PingPongFormatsList, VRenderTarget*> m_pingPongRenderTargets;
};

}

#endif // ABSTRACTRENDERER_H
