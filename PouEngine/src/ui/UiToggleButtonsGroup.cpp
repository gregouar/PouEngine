#include "PouEngine/ui/UiToggleButtonsGroup.h"

namespace pou
{

UiToggleButtonsGroup::UiToggleButtonsGroup(/*UserInterface *interface*/) :
    UiElement(/*interface*/)
{
    //ctor
}

UiToggleButtonsGroup::~UiToggleButtonsGroup()
{
    //dtor
}

/*void UiToggleButtonsGroup::addButton(UiButton *button, int value)
{
    m_toggleButtons.push_back({button, value});
    button->setToggable(true, true);
    this->addChildElement(button);
    this->startListeningTo(button, NotificationType_Ui_ButtonClicked);
}*/

void UiToggleButtonsGroup::addButton(std::shared_ptr<UiButton> button, int value)
{
    m_toggleButtons.push_back({button.get(), value});
    button->setToggable(true, true);
    this->addChildElement(button);
    this->startListeningTo(button.get(), NotificationType_Ui_ButtonClicked);
}


void UiToggleButtonsGroup::toggleButton(UiButton* button, bool activate)
{
    for(auto &buttonIt : m_toggleButtons)
    {
        if(buttonIt.first != button)
            buttonIt.first->setToggled(false);
        else if(activate)
            buttonIt.first->setToggled(true);
    }
}

int UiToggleButtonsGroup::getValue()
{
    for(auto &buttonIt : m_toggleButtons)
        if(buttonIt.first->isClicked())
            return buttonIt.second;

    return (0);
}

///
///Protected
///


void UiToggleButtonsGroup::notify(pou::NotificationSender* sender, int notificationType,
                    void* data)
{
    UiElement::notify(sender, notificationType, data);

    if(notificationType == NotificationType_Ui_ButtonClicked)
    {
        for(auto &buttonIt : m_toggleButtons)
            if(buttonIt.first == sender)
                this->toggleButton((UiButton*)sender, false/*, buttonIt.first->isClicked()*/);
    }
}

}
