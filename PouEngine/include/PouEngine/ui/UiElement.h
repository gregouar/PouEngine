#ifndef UIELEMENT_H
#define UIELEMENT_H

#include "PouEngine/Types.h"
#include "PouEngine/utils/SimpleNode.h"

namespace pou
{

class UiRenderer;

class UiElement : public SimpleNode
{
    public:
        UiElement(const NodeTypeId);
        virtual ~UiElement();

        void setSize(glm::vec2 s);
        void setSize(float x, float y);

        const glm::vec2 &getSize();

        virtual void render(UiRenderer *renderer);

    protected:
        virtual SimpleNode* nodeAllocator(NodeTypeId);

    private:
        glm::vec2 m_size;
};

}

#endif // UIELEMENT_H
