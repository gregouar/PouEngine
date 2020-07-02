#include "PouEngine/ui/UiTextInput.h"

namespace pou
{

UiTextInput::UiTextInput(UserInterface *interface) : UiText(interface)
{
    //ctor
}

UiTextInput::~UiTextInput()
{
    //dtor
}

void UiTextInput::update(const Time &elapsedTime, uint32_t localTime)
{
    UiText::update(elapsedTime, localTime);
}

}
