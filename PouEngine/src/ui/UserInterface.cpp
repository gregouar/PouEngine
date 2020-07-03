#include "PouEngine/ui/UserInterface.h"

#include "PouEngine/ui/UiElement.h"

namespace pou
{

UserInterface::UserInterface() :
    m_rootElement(this),
    m_focus(nullptr)
{
}

UserInterface::~UserInterface()
{
    this->cleanup();
}

void UserInterface::cleanup()
{
    m_rootElement.removeAllChilds();
}

void UserInterface::handleEvents(const EventsManager *eventsManager)
{
    if(eventsManager->mouseButtonPressed(GLFW_MOUSE_BUTTON_1))
    {
        m_focusWeight = UiFocusWeight();
        m_focus = nullptr;
    }

    m_rootElement.handleEvents(eventsManager);
}

void UserInterface::update(const Time &elapsedTime)
{
    m_rootElement.update(elapsedTime);
}

void UserInterface::render(UiRenderer *renderer)
{
    m_rootElement.render(renderer);
}

void UserInterface::addRootElement(std::shared_ptr<UiElement> element)
{
    m_rootElement.addChildNode(element);
}

std::shared_ptr<UiPicture> UserInterface::createUiPicture(bool addToInterface)
{
    auto element = std::make_shared<UiPicture>(this);
    if(addToInterface)
        m_rootElement.addChildNode(element);
    return element;
}

std::shared_ptr<UiProgressBar> UserInterface::createProgressBar(bool addToInterface)
{
    auto element = std::make_shared<UiProgressBar>(this);
    if(addToInterface)
        m_rootElement.addChildNode(element);
    return element;
}

void UserInterface::setFocus(UiElement *element)
{
    auto focusWeight = this->computeFocusWeight(element);
    if(!(focusWeight < m_focusWeight))
    {
        m_focusWeight = focusWeight;
        m_focus = element;
    }
}

bool UserInterface::isFocusedOn(UiElement *element)
{
    return (m_focus == element);
}

///
///Protected
///

UiFocusWeight UserInterface::computeFocusWeight(UiElement *element)
{
    return {element->getGlobalPosition().z, (float)element->getTreeDepth()};
}

}
