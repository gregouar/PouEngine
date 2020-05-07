#ifndef UIRENDERER_H
#define UIRENDERER_H

#include "PouEngine/renderers/AbstractRenderer.h"
#include "PouEngine/renderers/RenderGraph.h"

#include "PouEngine/ui/RenderableUiElement.h"

#include "PouEngine/vulkanImpl/DynamicVBO.h"

namespace pou
{

struct RenderableUiDatum;

class UiRenderer : public AbstractRenderer
{
    public:
        UiRenderer(RenderWindow *targetWindow, RendererName name, RenderereOrder order);
        virtual ~UiRenderer();


        void addToUiElementsVbo(const RenderableUiDatum &datum);
        void addOrderedUiElements(const RenderableUiDatum &datum, float weight);


    protected:
        virtual bool init();
        virtual void cleanup();

        virtual bool    createGraphicsPipeline();
        virtual bool    recordPrimaryCmb(uint32_t imageIndex);

    private:
        VGraphicsPipeline   m_graphicPipeline;

        std::multimap<float, RenderableUiDatum>     m_orderedUiElements;
        std::vector<DynamicVBO<RenderableUiDatum>*> m_uiElementsVbos;

        static const char *UIRENDERING_VERTSHADERFILE;
        static const char *UIRENDERING_FRAGSHADERFILE;
};

}

#endif // UIRENDERER_H
