#include "ai/AiComponent.h"

AiComponent::AiComponent(Character *character) :
    m_character(character)
{
    //ctor
}

AiComponent::~AiComponent()
{
    //dtor
}

void AiComponent::update(const pou::Time &elapsedTime, uint32_t localTime)
{
}
