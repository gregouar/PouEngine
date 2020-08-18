#include "PouEngine/ui/UserInterface.h"

#include "PouEngine/ui/UiElement.h"

namespace pou
{

UserInterface::UserInterface() :
    //m_rootElement(this),
    m_focus(nullptr)
{
    m_rootElement.setInterface(this);
}

UserInterface::~UserInterface()
{
    this->cleanup();
}

void UserInterface::cleanup()
{
    //m_sharedUiElements.clear();
    //m_uiElements.clear();
    ///m_rootElement.removeAllChilds();
}

void UserInterface::handleEvents(const EventsManager *eventsManager)
{
    if(eventsManager->mouseButtonPressed(GLFW_MOUSE_BUTTON_1))
    {
        m_focusWeight = UiFocusWeight();
        m_focus = nullptr;
    }

    //for(auto uiElement : m_uiElements)
      //  uiElement->handleEvents(eventsManager);

    m_rootElement.handleEvents(eventsManager);
}

void UserInterface::update(const Time &elapsedTime)
{
   // for(auto uiElement : m_uiElements)
     //   uiElement->update(elapsedTime);
    m_rootElement.update(elapsedTime);
}

void UserInterface::render(UiRenderer *renderer)
{
    //for(auto uiElement : m_uiElements)
      //  uiElement->render(renderer);
    m_rootElement.render(renderer);
}

/*void UserInterface::addUiElement(UiElement *uiElement)
{
    this->removeUiElement(uiElement);
    m_uiElements.push_back(uiElement);
    uiElement->setInterface(this);
}

void UserInterface::addUiElement(std::shared_ptr<UiElement> uiElement)
{
    this->addUiElement(uiElement.get());
    m_sharedUiElements.push_back(uiElement);
}

void UserInterface::removeUiElement(UiElement *uiElement)
{
    for(auto it = m_uiElements.begin() ; it != m_uiElements.end() ; ++it)
    if(*it == uiElement)
    {
        m_uiElements.erase(it);
        return;
    }
}*/

void UserInterface::addUiElement(std::shared_ptr<UiElement> element)
{
    m_rootElement.addChildElement(element);
}

void UserInterface::removeUiElement(UiElement *uiElement)
{
    m_rootElement.removeChildElement(uiElement);
}

/*std::shared_ptr<UiPicture> UserInterface::createUiPicture(bool addToInterface)
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
}*/

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
    return {element->transform()->getGlobalPosition().z,
            (float)element->transform()->getTreeDepth()};
}

}
