#ifndef UIELEMENT_H
#define UIELEMENT_H

#include "PouEngine/Types.h"
#include "PouEngine/utils/SimpleNode.h"
#include "PouEngine/core/EventsManager.h"

namespace pou
{

class UiRenderer;
class UserInterface;

class UiElement : public SimpleNode
{
    public:
        UiElement(const NodeTypeId, UserInterface *interface);
        virtual ~UiElement();

        virtual void setSize(glm::vec2 s); //use this one for children
        void setSize(float x, float y);

        const glm::vec2 &getSize();

        virtual void handleEvents(const EventsManager *eventManager);
        virtual void render(UiRenderer *renderer);

    protected:
        virtual SimpleNode* nodeAllocator(NodeTypeId);

    protected:
        UserInterface *m_interface;

    private:
        glm::vec2 m_size;
};

}

#endif // UIELEMENT_H
