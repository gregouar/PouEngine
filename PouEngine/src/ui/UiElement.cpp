#include "PouEngine/ui/UiElement.h"

#include "PouEngine/ui/UserInterface.h"

namespace pou
{

UiElement::UiElement(UserInterface *interface) :
    m_interface(interface),
    m_canHaveFocus(false),
    m_size(0.0f),
    m_isVisible(true),
    m_isMouseHover(false)
{
    assert(interface);
}


UiElement::~UiElement()
{
    //dtor
}

void UiElement::setSize(glm::vec2 s)
{
    if(s.x < 0)
    {
        this->move(s.x, 0);
        ///this->setScale(-1,0);
        s.x = -s.x;
    }

    if(s.y < 0)
    {
        this->move(s.y, 0);
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

const glm::vec2 &UiElement::getSize()
{
    return m_size;
}


std::shared_ptr<SimpleNode> UiElement::nodeAllocator()
{
    return std::make_shared<UiElement>(m_interface);
}

void UiElement::render(UiRenderer *renderer)
{
    if(!m_isVisible)
        return;

    for(auto node : m_childs)
        std::dynamic_pointer_cast<UiElement>(node)->render(renderer);
}


void UiElement::handleEvents(const EventsManager *eventsManager)
{
    m_isMouseHover = false;

    if(!m_isVisible)
        return;

    auto mousePos = eventsManager->mousePosition();
    auto &globalPos = this->getGlobalPosition();
    auto &size      = this->getSize();
    if(mousePos.x >= globalPos.x && mousePos.x <= globalPos.x + size.x
    && mousePos.y >= globalPos.y && mousePos.y <= globalPos.y + size.y)
        m_isMouseHover = true;

    if(m_canHaveFocus && m_isMouseHover
    && eventsManager->mouseButtonPressed(GLFW_MOUSE_BUTTON_1))
        m_interface->setFocus(this);

    for(auto node : m_childs)
        std::dynamic_pointer_cast<UiElement>(node)->handleEvents(eventsManager);
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




}
