#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include "PouEngine/ui/UiElement.h"
#include "PouEngine/ui/UiPicture.h"
#include "PouEngine/ui/UiProgressBar.h"

namespace pou
{

class UserInterface
{
    public:
        UserInterface();
        virtual ~UserInterface();

        virtual void handleEvents(const EventsManager *eventManager);
        virtual void update(const Time &elapsedTime);
        virtual void render(UiRenderer *renderer);

        virtual void addRootElement(std::shared_ptr<UiElement> element);

        std::shared_ptr<UiPicture> createUiPicture(bool addToInterface = false);
        std::shared_ptr<UiProgressBar> createProgressBar(bool addToInterface = false);

    protected:
        void cleanup();

    private:
        UiElement m_rootElement;

       /// NodeTypeId m_curNewId;
        ///std::list<UiElement*> m_createdElements;
};

}

#endif // USERINTERFACE_H
