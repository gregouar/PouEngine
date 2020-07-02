#include "PouEngine/ui/UiTextInput.h"

#include "PouEngine/ui/UiPicture.h"
#include "PouEngine/ui/UserInterface.h"

namespace pou
{

const float UiTextInput::DEFAULT_CURSOR_BLINK_DELAY = 0.5f;

UiTextInput::UiTextInput(UserInterface *interface) : UiText(interface),
    m_curState(UiTextInputState_Rest),
    m_cursorPosition(0),
    m_cursorBlinkDelay(DEFAULT_CURSOR_BLINK_DELAY),
    m_maxTextLength(-1)
{
    m_canHaveFocus = true;

    m_cursorPicture = std::make_shared<UiPicture>(m_interface);
    m_cursorPicture->setSize(1,this->getFontSize());
    this->addChildNode(m_cursorPicture);

    m_cursorPicture->hide();
}

UiTextInput::~UiTextInput()
{
    //dtor
}

void UiTextInput::handleEvents(const EventsManager *eventsManager)
{
    UiElement::handleEvents(eventsManager);

    if(!m_interface->isFocusedOn(this))
    {
        this->switchState(UiTextInputState_Rest);
        return;
    }

    this->switchState(UiTextInputState_Edit);

    this->handleCursor(eventsManager);
    this->handleTextEdit(eventsManager);
}

void UiTextInput::update(const Time &elapsedTime, uint32_t localTime)
{
    if(m_curState == UiTextInputState_Edit)
    {
        this->updateCursorPositionAndSize();

        if(m_cursorBlinkDelay > 0)
        {
            if(m_cursorBlinkTimer.update(elapsedTime))
                m_cursorPicture->setVisible(!m_cursorPicture->isVisible());
            if(!m_cursorBlinkTimer.isActive())
                m_cursorBlinkTimer.reset(m_cursorBlinkDelay);
        }
    }

    UiText::update(elapsedTime, localTime);
}

void UiTextInput::setColor(const glm::vec4 &color)
{
    UiText::setColor(color);
    m_cursorPicture->setColor(color);
}

void UiTextInput::setMaxTextLength(int length)
{
    if(length < 0)
        length = -1;
    m_maxTextLength = length;
}

void UiTextInput::setCursorPosition(size_t pos)
{
    m_cursorPosition = pos;
    this->showCursor();
}

void UiTextInput::moveCursor(size_t m)
{
    this->setCursorPosition(m_cursorPosition + m);
}

void UiTextInput::setCursorBlinkDelay(float t)
{
    if(t >= 0)
        m_cursorBlinkDelay = t;
}

void UiTextInput::insertText(size_t pos, const std::string &textToInsert, bool autoMoveCursor)
{
    auto textLength = this->getText().length();
    if(m_maxTextLength != -1 && textLength >= m_maxTextLength)
        return;

    std::string text = this->getText();

    if(m_maxTextLength != -1 && text.length() + textToInsert.length() > m_maxTextLength)
        textToInsert.substr(0, m_maxTextLength - textLength);

    text.insert(m_cursorPosition, textToInsert);
    this->setText(text);

    if(autoMoveCursor)
        this->setCursorPosition(pos+textToInsert.length());
    //this->moveCursor(eventsManager->getTextEntered().length());
}

///
///Protected
///

void UiTextInput::switchState(UiTextInputState state)
{
    if(m_curState != state)
    {
        if(state == UiTextInputState_Rest)
        {
            m_cursorPicture->hide();
            m_cursorBlinkTimer.reset();
        }
        if(state == UiTextInputState_Edit)
        {
           // this->showCursor();
            this->setCursorPosition(this->getText().length());
        }

        m_curState = state;
    }
}

void UiTextInput::updateCursorPositionAndSize()
{
    m_cursorPicture->setSize(1, this->getFontSize());

    if(m_cursorPosition < 0)
        m_cursorPosition = 0;

    if(m_cursorPosition > this->getText().length())
        m_cursorPosition = this->getText().length();

    auto x = this->computeTextSize(0, m_cursorPosition);
    m_cursorPicture->setPosition(x, 0);
}

void UiTextInput::showCursor()
{
    m_cursorPicture->show();
    m_cursorBlinkTimer.reset(m_cursorBlinkDelay);
}


void UiTextInput::handleCursor(const EventsManager *eventsManager)
{
    if(eventsManager->keyPressed(GLFW_KEY_RIGHT))
        this->moveCursor(1);
    if(eventsManager->keyPressed(GLFW_KEY_LEFT))
        this->moveCursor(-1);

    /* could add stuff to put cursor where we click, text select, etc */
}

void UiTextInput::handleTextEdit(const EventsManager *eventsManager)
{
    if(eventsManager->keyPressed(GLFW_KEY_BACKSPACE))
    {
        this->deleteCharacter(m_cursorPosition - 1);
        this->moveCursor(-1);
    }

    if(eventsManager->keyPressed(GLFW_KEY_DELETE))
        this->deleteCharacter(m_cursorPosition);

    if(!eventsManager->getTextEntered().empty())
        this->insertText(m_cursorPosition, eventsManager->getTextEnteredAsUtf8(), true);
}


void UiTextInput::deleteCharacter(int charPos)
{
    if(charPos < 0)
        return;

    std::string text = this->getText();

    if(charPos >= text.length())
        return;

    text.erase(charPos,1);
    this->setText(text);
}

}
