#include "PouEngine/ui/UiElement.h"

namespace pou
{

UiElement::UiElement(UserInterface *interface) :
    m_interface(interface),
    m_size(0.0f)
{
}


UiElement::~UiElement()
{
    //dtor
}

void UiElement::setSize(glm::vec2 s)
{
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
    for(auto node : m_childs)
        std::dynamic_pointer_cast<UiElement>(node)->render(renderer);
}


void UiElement::handleEvents(const EventsManager *eventManager)
{
    for(auto node : m_childs)
        std::dynamic_pointer_cast<UiElement>(node)->handleEvents(eventManager);
}





}
