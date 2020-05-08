#include "PouEngine/ui/UserInterface.h"


namespace pou
{

UserInterface::UserInterface()
{
    m_rootElement   = new UiElement(0,this);
    m_curNewId      = 0;
}

UserInterface::~UserInterface()
{
    this->cleanup();
}

void UserInterface::cleanup()
{
    delete m_rootElement;
    for(auto *element : m_createdElements)
        delete element;
    m_createdElements.clear();
}

void UserInterface::handleEvents(const EventsManager *eventManager)
{
    m_rootElement->handleEvents(eventManager);
}

void UserInterface::update(const Time &elapsedTime)
{
    m_rootElement->update(elapsedTime);
}

void UserInterface::render(UiRenderer *renderer)
{
    m_rootElement->render(renderer);
}

UiPicture* UserInterface::createUiPicture(bool addToInterface)
{
    auto *element = new UiPicture(++m_curNewId, this);
    if(addToInterface)
        m_rootElement->addChildNode(element);
    return element;
}

UiProgressBar* UserInterface::createProgressBar(bool addToInterface)
{
    auto *element = new UiProgressBar(++m_curNewId, this);
    if(addToInterface)
        m_rootElement->addChildNode(element);
    return element;
}



}
