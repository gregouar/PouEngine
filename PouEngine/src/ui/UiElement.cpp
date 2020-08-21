#include "PouEngine/ui/UiElement.h"

#include "PouEngine/ui/UserInterface.h"

namespace pou
{

UiElement::UiElement(/*UserInterface *interface*/) :
    m_interface(nullptr /*interface*/),
    m_canHaveFocus(false),
    m_parentElement(nullptr),
    m_size(0.0f),
    m_isVisible(true),
    m_isMouseHover(false)//,
    //m_parentElement(nullptr)
{
    //assert(interface);

    //m_transformComponent = new TransformComponent();
}


UiElement::~UiElement()
{
    if(m_interface)
        m_interface->removeUiElement(this);
    //delete m_transformComponent;
}
/*
void UiElement::addChildElement(UiElement *child)
{
    child->setParentElement(this);
    m_interface->addUiElement(child);
}

void UiElement::addChildElement(std::shared_ptr<UiElement> child)
{
    m_interface->addUiElement(child);
    child->setParentElement(this);
}

void UiElement::removeFromParent()
{
    this->setParentElement(nullptr);
    this->setInterface(nullptr);
}*/

void UiElement::addChildElement(std::shared_ptr<UiElement> child)
{
    m_childElements.push_back(child);
    child->setParentElement(this);
}

void UiElement::removeChildElement(UiElement *child)
{
    for(auto it = m_childElements.begin() ; it != m_childElements.end() ; ++it)
        if(it->get() == child)
        {
            m_childElements.erase(it);
            (*it)->setParentElement(nullptr);
            return;
        }
}

void UiElement::removeFromParent()
{
    if(m_parentElement)
        m_parentElement->removeChildElement(this);
}

void UiElement::setSize(glm::vec2 s)
{
    if(s.x < 0)
    {
        m_transformComponent.move(s.x, 0);
        ///this->setScale(-1,0);
        s.x = -s.x;
    }

    if(s.y < 0)
    {
        m_transformComponent.move(s.y, 0);
        ///this->scale(0,-1);
        s.y = -s.y;
    }

    if(s.x >= 0 && s.y >= 0)
        m_size = s;
}

void UiElement::setSize(float x, float y)
{
    this->setSize({x,y});
}


void UiElement::setSizeAndCenter(glm::vec2 s)
{
    this->setSize(s);
    this->transform()->move(-s*0.5f);
}

void UiElement::setSizeAndCenter(float x, float y)
{
    this->setSizeAndCenter({x,y});
}

const glm::vec2 &UiElement::getSize()
{
    return m_size;
}

void UiElement::render(UiRenderer *renderer)
{
    if(!m_isVisible)
        return;

    for(auto child : m_childElements)
        child->render(renderer);
}

void UiElement::update(const Time &elapsedTime)
{
    m_transformComponent.update(elapsedTime);

    for(auto child : m_childElements)
        child->update(elapsedTime);
}

void UiElement::handleEvents(const EventsManager *eventsManager)
{
    m_isMouseHover = false;

    if(!m_isVisible)
    {
        this->handleEventsInvisible(eventsManager);
        return;
    }

    auto mousePos = eventsManager->mousePosition();
    auto &globalPos = m_transformComponent.getGlobalPosition();
    auto &size      = this->getSize();

    ///I should change mousePos to local coord so that I can rotate...
    if(mousePos.x >= globalPos.x && mousePos.x <= globalPos.x + size.x
    && mousePos.y >= globalPos.y && mousePos.y <= globalPos.y + size.y)
        m_isMouseHover = true;

    if(m_canHaveFocus && m_isMouseHover
    && eventsManager->mouseButtonPressed(GLFW_MOUSE_BUTTON_1))
        m_interface->setFocus(this);

    for(auto child : m_childElements)
        child->handleEvents(eventsManager);
}

void UiElement::handleEventsInvisible(const EventsManager *eventsManager)
{
    for(auto child : m_childElements)
        child->handleEventsInvisible(eventsManager);
}

void UiElement::show()
{
    this->setVisible(true);
}

void UiElement::hide()
{
    this->setVisible(false);
}

void UiElement::setVisible(bool visible)
{
    m_isVisible = visible;
}

bool UiElement::isVisible()
{
    return m_isVisible;
}

bool UiElement::isMouseHover()
{
    return m_isMouseHover;
}


TransformComponent *UiElement::transform()
{
    return &m_transformComponent;
}

///
///Protected
///


void UiElement::notify(NotificationSender *sender, int notificationType, void* data)
{
    /*if(sender == m_parent)
    {
        if(notificationType == NotificationType_SenderDestroyed)
            m_parentElement = nullptr;
    }*/
}

void UiElement::setInterface(UserInterface *interface)
{
    if(m_interface == interface)
        return;

    //if(m_interface)
      //  m_interface->removeUiElement(this);
    m_interface = interface;

    for(auto child : m_childElements)
        child->setInterface(interface);
}

void UiElement::setParentElement(UiElement *parent)
{
    if(m_parentElement == parent)
        return;

    if(parent)
    {
        m_interface = parent->m_interface;
        m_transformComponent.setParent(parent->transform());
    }
    else
    {
        m_interface = nullptr;
        m_transformComponent.setParent(nullptr);
    }

    m_parentElement = parent;
}

}
