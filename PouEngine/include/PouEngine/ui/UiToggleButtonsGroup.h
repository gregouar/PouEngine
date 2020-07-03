#ifndef UITOGGLEBUTTONSGROUP_H
#define UITOGGLEBUTTONSGROUP_H

#include "PouEngine/ui/UiElement.h"
#include "PouEngine/ui/UiButton.h"

namespace pou
{

class UiToggleButtonsGroup : public UiElement
{
    public:
        UiToggleButtonsGroup(UserInterface *interface);
        virtual ~UiToggleButtonsGroup();

        void addButton(std::shared_ptr<UiButton> button, int value);

        void toggleButton(UiButton *button, bool activate = true);
        int getValue();

    protected:
        virtual void notify(pou::NotificationSender*, int notificationType,
                            void* data);

    private:
        std::list< std::pair<std::shared_ptr<UiButton>, int> > m_toggleButtons;
};

}

#endif // UITOGGLEBUTTONSGROUP_H
