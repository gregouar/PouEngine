#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include "PouEngine/ui/UiElement.h"
#include "PouEngine/ui/UiPicture.h"
#include "PouEngine/ui/UiProgressBar.h"

namespace pou
{

struct UiFocusWeight
{
    UiFocusWeight(float p_z, float p_depth) : z(p_z), depth(p_depth){};
    UiFocusWeight() : UiFocusWeight(0,0){};

    bool operator<(const UiFocusWeight &weight) const
    {
        return (z < weight.z) ||
            (z == weight.z && depth < weight.depth);
    }

    float z;
    float depth;
};

class UserInterface
{
    public:
        UserInterface();
        virtual ~UserInterface();

        virtual void handleEvents(const EventsManager *eventManager);
        virtual void update(const Time &elapsedTime);
        virtual void render(UiRenderer *renderer);

        //void addUiElement(UiElement *uiElement);
        void addUiElement(std::shared_ptr<UiElement> uiElement);
        void removeUiElement(UiElement *uiElement);

        //virtual void addRootElement(std::shared_ptr<UiElement> element);

        //std::shared_ptr<UiPicture> createUiPicture(bool addToInterface = false);
        //std::shared_ptr<UiProgressBar> createProgressBar(bool addToInterface = false);

        void setFocus(UiElement *element);
        bool isFocusedOn(UiElement *element);

    protected:
        void cleanup();

        UiFocusWeight computeFocusWeight(UiElement *element);

    private:
        UiElement       m_rootElement;

        UiElement      *m_focus;
        UiFocusWeight   m_focusWeight;

        //TransformsAllocator m_transformsAllocator;
};

}

#endif // USERINTERFACE_H
