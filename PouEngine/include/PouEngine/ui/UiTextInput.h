#ifndef UITEXTINPUT_H
#define UITEXTINPUT_H

#include "PouEngine/ui/UiText.h"
#include "PouEngine/system/Timer.h"

namespace pou
{

enum UiTextInputState
{
    UiTextInputState_Rest,
    UiTextInputState_Edit,
};

class UiTextInput : public UiText
{
    public:
        UiTextInput(UserInterface *interface);
        virtual ~UiTextInput();

        virtual void handleEvents(const EventsManager *eventsManager) override;

        virtual void update(const Time &elapsedTime = Time(0), uint32_t localTime = -1) override;

        virtual void setColor(const glm::vec4 &color);

        void setMaxTextLength(int length);
        void setCursorPosition(size_t pos);
        void moveCursor(size_t m);
        void setCursorBlinkDelay(float t);

        void insertText(size_t pos, const std::string &text, bool autoMoveCursor = false);

    protected:
        void switchState(UiTextInputState state);
        void updateCursorPositionAndSize();

        void showCursor();

        void handleCursor(const EventsManager *eventsManager);
        void handleTextEdit(const EventsManager *eventsManager);

        void deleteCharacter(int charPos);

    private:
        UiTextInputState m_curState;

        int m_cursorPosition;
        std::shared_ptr<UiPicture> m_cursorPicture;

        float m_cursorBlinkDelay;
        Timer m_cursorBlinkTimer;

        int m_maxTextLength;

    public:
        static const float DEFAULT_CURSOR_BLINK_DELAY;
};

}

#endif // UITEXTINPUT_H
