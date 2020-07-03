#include "PouEngine/ui/UiTextInput.h"

#include "PouEngine/ui/UiPicture.h"
#include "PouEngine/ui/UserInterface.h"

namespace pou
{

const float UiTextInput::DEFAULT_CURSOR_BLINK_DELAY = 0.5f;

UiTextInput::UiTextInput(UserInterface *interface) : UiText(interface),
    m_curState(UiTextInputState_Rest),
    m_cursorPosition(0),
    m_startingCursorPosition(0),
    m_needToUpdateCursor(true),
    m_cursorBlinkDelay(DEFAULT_CURSOR_BLINK_DELAY),
    m_maxTextLength(-1)
{
    m_canHaveFocus = true;

    m_cursorPicture = std::make_shared<UiPicture>(m_interface);
    m_cursorPicture->setSize(1,this->getFontSize());
    this->addChildNode(m_cursorPicture);

    m_textSelectPicture = std::make_shared<UiPicture>(m_interface);
    m_textSelectPicture->setSize(1,this->getFontSize());
    this->addChildNode(m_textSelectPicture);

    m_cursorPicture->hide();
    m_textSelectPicture->hide();

    this->setColor(glm::vec4(1));
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
        if(m_needToUpdateCursor)
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

void UiTextInput::setText(const std::string &text)
{
    UiText::setText(text);
    this->verifyCursorPosition();

    m_needToUpdateCursor = true;
}

void UiTextInput::setFontSize(int pt)
{
    UiText::setFontSize(pt);
    m_needToUpdateCursor = true;
}

void UiTextInput::setColor(const glm::vec4 &color)
{
    UiText::setColor(color);
    m_cursorPicture->setColor(color);

    ///Change this, maybe just create setSelectColor
    m_textSelectPicture->setColor(color * glm::vec4(0.5,0.5,0.5,1.0));
}


void UiTextInput::setMaxTextLength(int length)
{
    if(length < 0)
        length = -1;
    m_maxTextLength = length;
}

void UiTextInput::setCursorPosition(size_t pos, bool autoMoveSelect)
{
    m_cursorPosition = pos;
    this->verifyCursorPosition();
    this->showCursor();

    if(autoMoveSelect)
        m_startingCursorPosition = m_cursorPosition;

    m_needToUpdateCursor = true;
}

void UiTextInput::moveCursor(size_t m, bool autoMoveSelect)
{
    this->setCursorPosition(m_cursorPosition + m, autoMoveSelect);
}

void UiTextInput::setCursorBlinkDelay(float t)
{
    if(t >= 0)
        m_cursorBlinkDelay = t;
}

void UiTextInput::insertText(size_t pos, const std::string &textToInsert, bool autoMoveCursor)
{
    auto textLength = (int)this->getText().length();
    if(m_maxTextLength != -1 && textLength >= m_maxTextLength)
        return;

    std::string text = this->getText();

    if(m_maxTextLength != -1 && textLength + (int)textToInsert.length() > m_maxTextLength)
        textToInsert.substr(0, m_maxTextLength - textLength);

    text.insert(m_cursorPosition, textToInsert);
    this->setText(text);

    if(autoMoveCursor)
        this->setCursorPosition(pos+textToInsert.length());
    //this->moveCursor(eventsManager->getTextEntered().length());
}

std::string UiTextInput::getSelectedText()
{
    auto startPos = m_startingCursorPosition;
    auto endPos = m_cursorPosition;

    if(startPos > endPos)
    {
        endPos = m_startingCursorPosition;
        startPos = m_cursorPosition;
    }

    return this->getText().substr(startPos, endPos - startPos);
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
            m_textSelectPicture->hide();
            m_cursorBlinkTimer.reset();
        }
        if(state == UiTextInputState_Edit)
        {
           // this->showCursor();
            this->setCursorPosition(this->getText().length());
            m_textSelectPicture->show();
        }

        m_curState = state;
    }
}

void UiTextInput::verifyCursorPosition()
{
    if(m_cursorPosition < 0)
        m_cursorPosition = 0;

    if(m_cursorPosition > (int)this->getText().length())
        m_cursorPosition = this->getText().length();
}

void UiTextInput::updateCursorPositionAndSize()
{
    m_cursorPicture->setSize(1, this->getFontSize());

    auto x = this->computeTextSize(0, m_cursorPosition);
    m_cursorPicture->setPosition(x, 0);

    auto startSelectX = this->computeTextSize(0, m_startingCursorPosition);
    m_textSelectPicture->setPosition(startSelectX, 0);
    m_textSelectPicture->setSize(x - startSelectX, this->getFontSize());

    m_needToUpdateCursor = false;
}

void UiTextInput::showCursor()
{
    m_cursorPicture->show();
    m_cursorBlinkTimer.reset(m_cursorBlinkDelay);
}


void UiTextInput::handleCursor(const EventsManager *eventsManager)
{
    if(eventsManager->keyPressed(GLFW_KEY_RIGHT, true))
        this->moveCursor(1, !eventsManager->keyMod(GLFW_MOD_SHIFT));
    if(eventsManager->keyPressed(GLFW_KEY_LEFT, true))
        this->moveCursor(-1, !eventsManager->keyMod(GLFW_MOD_SHIFT));

    //if(this->isMouseHover())
    if(eventsManager->mouseButtonIsPressed(GLFW_MOUSE_BUTTON_1))
    {
        auto relPos = eventsManager->mousePosition() - this->getGlobalXYPosition();
        this->setCursorPosition(this->computeCharPos(relPos), false);

        if(eventsManager->mouseButtonPressed(GLFW_MOUSE_BUTTON_1))
            m_startingCursorPosition = m_cursorPosition;
    }

    /* could add stuff to put cursor where we click, text select, etc */
}

void UiTextInput::handleTextEdit(const EventsManager *eventsManager)
{
    if(eventsManager->keyPressed(GLFW_KEY_BACKSPACE, true))
    {
        if(this->isSelectionEmpty())
        {
            this->moveCursor(-1);
            this->deleteCharacter(m_cursorPosition);
        }
        else
            this->deleteSelectedText();
    }

    if(eventsManager->keyPressed(GLFW_KEY_DELETE, true))
    {
        if(this->isSelectionEmpty())
            this->deleteCharacter(m_cursorPosition);
        else
            this->deleteSelectedText();
    }

    if(!eventsManager->getTextEntered().empty())
        this->insertText(m_cursorPosition, eventsManager->getTextEnteredAsUtf8(), true);

    if(eventsManager->keyPressed(GLFW_KEY_V) && eventsManager->keyMod(GLFW_MOD_CONTROL))
        this->insertText(m_cursorPosition, eventsManager->getClipBoard(), true);

    if(eventsManager->keyPressed(GLFW_KEY_C) && eventsManager->keyMod(GLFW_MOD_CONTROL)
    && !this->isSelectionEmpty())
        eventsManager->setClipBoard(this->getSelectedText());

    if(eventsManager->keyPressed(GLFW_KEY_X) && eventsManager->keyMod(GLFW_MOD_CONTROL)
    && !this->isSelectionEmpty())
    {
        eventsManager->setClipBoard(this->getSelectedText());
        this->deleteSelectedText();
    }
}

bool UiTextInput::isSelectionEmpty()
{
    return (m_startingCursorPosition == m_cursorPosition);
}

void UiTextInput::deleteCharacter(int charPos)
{
    this->deleteText(charPos, charPos+1);
}

void UiTextInput::deleteText(int startPos, int endPos)
{
    if(startPos < 0)
        startPos = 0;

    std::string text = this->getText();

    if(endPos >= (int)text.length())
        endPos = (int)text.length();

    if(startPos == endPos)
        return;

    text.erase(startPos,endPos - startPos);
    this->setText(text);
}

void UiTextInput::deleteSelectedText()
{
    auto startPos = m_startingCursorPosition;
    auto endPos = m_cursorPosition;

    if(startPos > endPos)
    {
        endPos = m_startingCursorPosition;
        startPos = m_cursorPosition;
    }

    this->deleteText(startPos, endPos);
    m_startingCursorPosition = startPos;
    m_cursorPosition = startPos;
}

}
