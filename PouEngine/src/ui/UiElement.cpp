#include "PouEngine/ui/UiElement.h"

namespace pou
{

UiElement::UiElement(const NodeTypeId id, UserInterface *interface) :
    SimpleNode(id),
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


SimpleNode* UiElement::nodeAllocator(NodeTypeId id)
{
    return new UiElement(id,m_interface);
}

void UiElement::render(UiRenderer *renderer)
{
    for(auto node : m_childs)
        dynamic_cast<UiElement*>(node.second)->render(renderer);
}


void UiElement::handleEvents(const EventsManager *eventManager)
{
    for(auto node : m_childs)
        dynamic_cast<UiElement*>(node.second)->handleEvents(eventManager);
}





}
