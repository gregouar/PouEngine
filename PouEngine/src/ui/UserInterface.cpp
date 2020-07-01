#include "PouEngine/ui/UserInterface.h"


namespace pou
{

UserInterface::UserInterface() :
    m_rootElement(this)
{
   /// m_rootElement   = new UiElement(/**0,**/this);
    ///m_curNewId      = 0;
}

UserInterface::~UserInterface()
{
    this->cleanup();
}

void UserInterface::cleanup()
{
    ///delete m_rootElement;
    /**for(auto *element : m_createdElements)
        delete element;
    m_createdElements.clear();**/

    m_rootElement.removeAllChilds();
}

void UserInterface::handleEvents(const EventsManager *eventManager)
{
    m_rootElement.handleEvents(eventManager);
}

void UserInterface::update(const Time &elapsedTime)
{
    m_rootElement.update(elapsedTime);
}

void UserInterface::render(UiRenderer *renderer)
{
    m_rootElement.render(renderer);
}

void UserInterface::addRootElement(std::shared_ptr<UiElement> element)
{
    m_rootElement.addChildNode(element);
}

std::shared_ptr<UiPicture> UserInterface::createUiPicture(bool addToInterface)
{
    auto element = std::make_shared<UiPicture>(/**++m_curNewId,**/ this);
    if(addToInterface)
        m_rootElement.addChildNode(element);
    return element;
}

std::shared_ptr<UiProgressBar> UserInterface::createProgressBar(bool addToInterface)
{
    auto element = std::make_shared<UiProgressBar>(/**++m_curNewId,**/ this);
    if(addToInterface)
        m_rootElement.addChildNode(element);
    return element;
}



}
