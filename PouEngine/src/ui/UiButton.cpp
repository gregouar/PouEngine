#include "PouEngine/ui/UiButton.h"

#include "PouEngine/assets/AssetHandler.h"
#include "PouEngine/assets/TextureAsset.h"
#include "PouEngine/ui/UserInterface.h"

namespace pou
{

UiButton::UiButton(/*UserInterface *interface*/) : UiElement(/*interface*/),
    m_curState(UiButtonState_Rest),
    m_lastShowedState(NBR_UIBUTTONSTATES),
    m_lastShowedTextureState(NBR_UIBUTTONSTATES),
    m_toggable(false),
    m_isToggled(false),
    m_disableUntoggle(false)
{
    m_canHaveFocus = true;

    for(size_t i = 0 ; i < NBR_UIBUTTONSTATES ; ++i)
        m_stateElements[i] = nullptr;
}

UiButton::~UiButton()
{
    //dtor
}

void UiButton::update(const Time &elapsedTime)
{
    UiElement::update(elapsedTime);

    ///Need to update somewhere size of textures
}

void UiButton::handleEvents(const EventsManager *eventsManager)
{
    UiElement::handleEvents(eventsManager);

    if(this->isMouseHover())
    {
        if(eventsManager->mouseButtonReleased(GLFW_MOUSE_BUTTON_1))
        {
            if(!m_interface->isFocusedOn(this))
                return;

            this->activate();

            /*if(m_isToggled)
                this->switchState(UiButtonState_Rest);
            else
            {
                this->switchState(UiButtonState_Released);
                this->sendNotification(NotificationType_Ui_ButtonClicked);
            }*/
        }
        else if(m_interface->isFocusedOn(this) && eventsManager->mouseButtonIsPressed(GLFW_MOUSE_BUTTON_1))
            this->switchState(UiButtonState_Pressed);
        else
            this->switchState(UiButtonState_Hover);
    }
    else
    {
        if(m_isToggled)
            this->switchState(UiButtonState_Released);
        else
            this->switchState(UiButtonState_Rest);
    }
}

void UiButton::handleEventsInvisible(const EventsManager *eventsManager)
{
    UiElement::handleEventsInvisible(eventsManager);

    this->switchState(UiButtonState_Rest);
}

void UiButton::setSize(glm::vec2 s)
{
    UiElement::setSize(s);

    for(int i = 0 ; i < NBR_UIBUTTONSTATES ; ++i)
        if(m_stateTextureElements[i])
            m_stateTextureElements[i]->setSize(s);

    if(m_textLabel)
        m_textLabel->setSize(s);
}

void UiButton::setTexture(UiButtonState state, TextureAsset *texture,
                glm::vec2 extent, glm::vec2 pos)
{
    if(state == NBR_UIBUTTONSTATES)
        return;

    auto pictureElement = std::make_shared<UiPicture>(/*m_interface*/);
    this->addChildElement(pictureElement);

    if(m_curState != state)
        pictureElement->hide();

    pictureElement->setSize(this->getSize());
    pictureElement->setTexture(texture);

    if(extent == glm::vec2(0) && texture)
        extent = texture->getExtent();

    pictureElement->setTextureExtent(extent);
    pictureElement->setTexturePosition(pos);
    pictureElement->setTexture(texture);

    if(m_stateTextureElements[state])
        m_stateTextureElements[state]->removeFromParent();
    m_stateTextureElements[state] = pictureElement;
}

void UiButton::setColor(UiButtonState state, const glm::vec4 &color)
{
    if(state == NBR_UIBUTTONSTATES)
        return;

    if(!m_stateTextureElements[state])
        this->setTexture(state, nullptr /*TexturesHandler::dummyAsset()*/);

    m_stateTextureElements[state]->setColor(color);
}

void UiButton::setStateElement(UiButtonState state, std::shared_ptr<UiElement> uiElement)
{
    if(state == NBR_UIBUTTONSTATES)
        return;

    m_stateElements[state] = uiElement;
}

void UiButton::setLabel(const std::string &label, int fontSize, const glm::vec4 &color, FontAsset *font)
{
    if(!m_textLabel)
    {
        if(!font || fontSize == 0)
            return;

        m_textLabel = std::make_shared<UiText>(/*m_interface*/);
        m_textLabel->setSize(this->getSize());
        m_textLabel->transform()->setPosition(0,0,1);
        m_textLabel->setTextAlign(TextAlignType_Center);
        m_textLabel->setVerticalAlign(true);
        this->addChildElement(m_textLabel);
    }

    if(fontSize != 0)
        m_textLabel->setFontSize(fontSize);

    if(color != glm::vec4(0))
        m_textLabel->setColor(color);

    if(font)
        m_textLabel->setFont(font);

    m_textLabel->setText(label);
}

void UiButton::setToggable(bool toggable, bool disableUntoggle)
{
    m_toggable = toggable;
    m_disableUntoggle = disableUntoggle;
}

void UiButton::setToggled(bool toggled)
{
    if(!m_toggable)
        return;

    m_isToggled = toggled;

    if(!m_isToggled)
        this->switchState(UiButtonState_Rest);
    else
    {
        this->switchState(UiButtonState_Released);
        this->sendNotification(NotificationType_Ui_ButtonClicked);
    }
}


UiText *UiButton::getLabel()
{
    return m_textLabel.get();
}

void UiButton::activate()
{
    if(m_isToggled && !m_disableUntoggle)
        this->switchState(UiButtonState_Rest);
    else
    {
        this->switchState(UiButtonState_Released);
        this->sendNotification(NotificationType_Ui_ButtonClicked);
    }
}

bool UiButton::isClicked()
{
    return (m_curState == UiButtonState_Released);
}



///
///Protected
///

void UiButton::switchState(UiButtonState state)
{
    if(state == m_curState)
        return;

    if(m_toggable)
    {
        if(state == UiButtonState_Rest)
            m_isToggled = false;
        if(state == UiButtonState_Released)
            m_isToggled = true;
    }

    if(m_stateElements[state])
    {
        m_stateElements[state]->show();
        if(m_lastShowedState < NBR_UIBUTTONSTATES && m_stateElements[m_curState])
            m_stateElements[m_lastShowedState]->hide();

        m_lastShowedState = state;
    }

    if(m_stateTextureElements[state])
    {
        m_stateTextureElements[state]->show();
        if(m_lastShowedTextureState < NBR_UIBUTTONSTATES && m_stateTextureElements[m_curState])
            m_stateTextureElements[m_lastShowedTextureState]->hide();

        m_lastShowedTextureState = state;
    }

    m_curState = state;
}

}

