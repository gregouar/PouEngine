#ifndef UIELEMENT_H
#define UIELEMENT_H

#include "PouEngine/Types.h"
//#include "PouEngine/system/SimpleNode.h"
#include "PouEngine/core/EventsManager.h"
#include "PouEngine/math/TransformComponent.h"

#include <vector>

namespace pou
{

class UiRenderer;
class UserInterface;

class UiElement : public NotificationListener, public NotificationSender //: public SimpleNode
{
    friend class UserInterface;

    public:
        UiElement(/*UserInterface *interface*/);
        virtual ~UiElement();

        //void addChildElement(UiElement *child);
        void addChildElement(std::shared_ptr<UiElement> child);
        void removeChildElement(UiElement *child);
        void removeFromParent();

        virtual void setSize(glm::vec2 s); //use this one for children
        void setSize(float x, float y);

        void setSizeAndCenter(glm::vec2 s);
        void setSizeAndCenter(float x, float y);

        const glm::vec2 &getSize();

        virtual void update(const Time &elapsedTime = Time(0));
        virtual void handleEvents(const EventsManager *eventsManager);
        virtual void handleEventsInvisible(const EventsManager *eventsManager);
        virtual void render(UiRenderer *renderer);

        virtual void show();
        virtual void hide();
        virtual void setVisible(bool visible);

        bool isVisible();
        bool isMouseHover();

        TransformComponent *transform();

    protected:
        void notify(NotificationSender *sender, int notificationType, void* data);

        void setInterface(UserInterface *interface);
        void setParentElement(UiElement *parent);

    protected:
        UserInterface *m_interface;
        bool m_canHaveFocus;

        UiElement *m_parentElement;
        std::vector< std::shared_ptr<UiElement> > m_childElements;

    private:
        glm::vec2 m_size;
        bool m_isVisible;
        bool m_isMouseHover;

        //TransformComponent *m_transformComponent;
        TransformComponent m_transformComponent;
};

}

#endif // UIELEMENT_H
