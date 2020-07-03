#ifndef UIBUTTON_H
#define UIBUTTON_H

#include "PouEngine/ui/UiElement.h"
#include "PouEngine/ui/UiPicture.h"
#include "PouEngine/ui/UiText.h"

namespace pou
{

///Could add Toggled state
enum UiButtonState
{
    UiButtonState_Rest,
    UiButtonState_Hover,
    UiButtonState_Pressed,
    UiButtonState_Released,
    NBR_UIBUTTONSTATES,
};

///Add release state timer

class UiButton : public UiElement
{
    public:
        UiButton(UserInterface *interface);
        virtual ~UiButton();

        virtual void update(const Time &elapsedTime = Time(0), uint32_t localTime = -1) override;

        virtual void handleEvents(const EventsManager *eventsManager) override;

        using UiElement::setSize;
        virtual void setSize(glm::vec2 s);

        void setTexture(UiButtonState state, TextureAsset *texture,
                        glm::vec2 extent = glm::vec2(0), glm::vec2 pos = glm::vec2(0));
        void setColor(UiButtonState state, const glm::vec4 &color);
        void setStateElement(UiButtonState state, std::shared_ptr<UiElement> uiElement);
        void setLabel(const std::string &label, int fontSize = 0, const glm::vec4 &color = glm::vec4(0), FontAsset *font = nullptr);
        void setToggable(bool toggable = true, bool disableUntoggle = false);
        void setToggled(bool toggled = true);

        std::shared_ptr<UiText> getLabel();

        void activate();
        bool isClicked();

    protected:
        void switchState(UiButtonState state);

    private:
        UiButtonState m_curState;
        UiButtonState m_lastShowedState;
        UiButtonState m_lastShowedTextureState;

        std::shared_ptr<UiElement> m_stateElements[NBR_UIBUTTONSTATES];
        std::shared_ptr<UiPicture> m_stateTextureElements[NBR_UIBUTTONSTATES];

        std::shared_ptr<UiText> m_textLabel;

        bool m_toggable;
        bool m_isToggled;
        bool m_disableUntoggle;
};

}

#endif // UIBUTTON_H
