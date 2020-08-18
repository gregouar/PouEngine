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
        UiTextInput(/*UserInterface *interface*/);
        virtual ~UiTextInput();

        virtual void handleEvents(const EventsManager *eventsManager) override;

        virtual void update(const Time &elapsedTime = Time(0)) override;

        virtual void setText(const std::string &text);
        virtual void setFontSize(int pt);
        virtual void setColor(const glm::vec4 &color);

        void setMaxTextLength(int length);
        void setCursorPosition(size_t pos, bool autoMoveSelect = true);
        void moveCursor(size_t m, bool autoMoveSelect = true);
        void setCursorBlinkDelay(float t);

        void insertText(size_t pos, const std::string &text, bool autoMoveCursor = false);

        std::string getSelectedText();

    protected:
        void switchState(UiTextInputState state);
        void verifyCursorPosition();
        void updateCursorPositionAndSize();

        void showCursor();

        void handleCursor(const EventsManager *eventsManager);
        void handleTextEdit(const EventsManager *eventsManager);

        bool isSelectionEmpty();

        void deleteCharacter(int charPos);
        void deleteText(int startPos, int endPos);
        void deleteSelectedText();

    private:
        UiTextInputState m_curState;

        int m_cursorPosition;
        int m_startingCursorPosition;

        bool m_needToUpdateCursor;
        std::shared_ptr<UiPicture> m_cursorPicture;
        std::shared_ptr<UiPicture> m_textSelectPicture;

        float m_cursorBlinkDelay;
        Timer m_cursorBlinkTimer;

        int m_maxTextLength;

    public:
        static const float DEFAULT_CURSOR_BLINK_DELAY;
};

}

#endif // UITEXTINPUT_H
