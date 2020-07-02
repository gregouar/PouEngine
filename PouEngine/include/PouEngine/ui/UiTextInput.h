#ifndef UITEXTINPUT_H
#define UITEXTINPUT_H

#include "PouEngine/ui/UiText.h"

namespace pou
{

class UiTextInput : public UiText
{
    public:
        UiTextInput(UserInterface *interface);
        virtual ~UiTextInput();

        virtual void update(const Time &elapsedTime = Time(0), uint32_t localTime = -1) override;


    protected:

    private:
};

}

#endif // UITEXTINPUT_H
