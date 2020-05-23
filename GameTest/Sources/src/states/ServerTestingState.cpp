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
}

void ServerTestingState::entered()
{
    m_totalTime = pou::TimeZero();

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

    if(m_scene == nullptr)
        return;

    if(eventsManager->keyPressed(GLFW_KEY_O))
        m_gameServer.sendMsgTest(true,false);
}



void ServerTestingState::update(const pou::Time &elapsedTime)
{
    m_totalTime += elapsedTime;

    pou::Time remainingTime = elapsedTime;
    pou::Time maxTickTime(.03);

    while(remainingTime > maxTickTime)
    {
        m_gameServer.update(maxTickTime);
        if(m_scene)
            m_scene->update(maxTickTime);
        remainingTime -= maxTickTime;
    }
    m_gameServer.update(remainingTime);
    if(m_scene)
        m_scene->update(remainingTime);
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


