#include "PouEngine/ui/UiElement.h"

namespace pou
{

UiElement::UiElement(const NodeTypeId id) :
    SimpleNode(id),
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
    return new UiElement(id);
}

void UiElement::render(UiRenderer *renderer)
{
    this->updateModelMatrix();

    for(auto node : m_childs)
        dynamic_cast<UiElement*>(node.second)->render(renderer);
}



}
