#include "states/ServerTestingState.h"

#include "PouEngine/Types.h"
#include "PouEngine/utils/Clock.h"

ServerTestingState::ServerTestingState() :
    m_firstEntering(true)
{
    //ctor
}

ServerTestingState::~ServerTestingState()
{
    this->leaving();
}


void ServerTestingState::init()
{
    m_firstEntering = false;

    m_gameServer.create(46969);
    m_gameServer.generateWorld();
}

void ServerTestingState::entered()
{
    if(m_firstEntering)
        this->init();
}


void ServerTestingState::leaving()
{
    m_gameServer.shutdown();
}

void ServerTestingState::revealed()
{

}

void ServerTestingState::obscuring()
{

}


void ServerTestingState::handleEvents(const EventsManager *eventsManager)
{
    if(eventsManager->keyReleased(GLFW_KEY_ESCAPE))
        m_manager->stop();

    if(eventsManager->isAskingToClose())
        m_manager->stop();

    if(eventsManager->keyPressed(GLFW_KEY_O))
        m_gameServer.sendMsgTest(true,false);
}



void ServerTestingState::update(const pou::Time &elapsedTime)
{
    m_gameServer.update(elapsedTime);
}

void ServerTestingState::draw(pou::RenderWindow *renderWindow)
{
    /*if(renderWindow->getRenderer(pou::Renderer_Scene) != nullptr)
    {
        pou::SceneRenderer *renderer = dynamic_cast<pou::SceneRenderer*>(renderWindow->getRenderer(pou::Renderer_Scene));
        m_scene->render(renderer, m_camera);
    }*/

    /*if(renderWindow->getRenderer(pou::Renderer_Ui) != nullptr)
    {
        pou::UiRenderer *renderer = dynamic_cast<pou::UiRenderer*>(renderWindow->getRenderer(pou::Renderer_Ui));
        m_mainInterface->render(renderer);
    }*/
}


