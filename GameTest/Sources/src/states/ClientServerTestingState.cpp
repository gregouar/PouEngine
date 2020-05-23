#include "states/ClientServerTestingState.h"

#include "PouEngine/Types.h"
#include "PouEngine/utils/Clock.h"

#include "PouEngine/renderers/UiRenderer.h"

ClientServerTestingState::ClientServerTestingState() :
    m_firstEntering(true)
{
    //ctor
}

ClientServerTestingState::~ClientServerTestingState()
{
    this->leaving();
}


void ClientServerTestingState::init()
{
    m_firstEntering = false;

    m_gameServer.create(46969);
    m_gameServer.generateWorld();

    m_gameClient.create(42420);
    //m_gameClient.connectToServer(pou::NetAddress(127,0,0,1,m_gameServer.getPort()));

    m_gameUi.init();
}

void ClientServerTestingState::entered()
{
    m_totalTime = pou::TimeZero();

    if(m_firstEntering)
        this->init();
}

void ClientServerTestingState::leaving()
{
    m_gameClient.disconnectFromServer();
    m_gameServer.shutdown();
}

void ClientServerTestingState::revealed()
{

}

void ClientServerTestingState::obscuring()
{

}


void ClientServerTestingState::handleEvents(const EventsManager *eventsManager)
{
    m_gameUi.handleEvents(eventsManager);

    if(eventsManager->keyReleased(GLFW_KEY_ESCAPE))
        m_manager->stop();

    if(eventsManager->isAskingToClose())
        m_manager->stop();

    if(eventsManager->keyPressed(GLFW_KEY_Z))
        m_gameClient.disconnectFromServer();
    if(eventsManager->keyPressed(GLFW_KEY_X))
        m_gameClient.connectToServer(pou::NetAddress(127,0,0,1,m_gameServer.getPort()));

    if(eventsManager->keyPressed(GLFW_KEY_I))
        m_gameClient.sendMsgTest(false,true);
    if(eventsManager->keyPressed(GLFW_KEY_O))
        m_gameServer.sendMsgTest(true,false);
}


void ClientServerTestingState::update(const pou::Time &elapsedTime)
{
    m_totalTime += elapsedTime;

    //m_gameUi.updateCharacterLife(m_character->getAttributes().life,
    //                             m_character->getAttributes().maxLife);

    pou::Time remainingTime = elapsedTime;
    pou::Time maxTickTime(.03);

    while(remainingTime > maxTickTime)
    {
        m_gameUi.update(maxTickTime);
        m_gameServer.update(maxTickTime);
        m_gameClient.update(maxTickTime);
        remainingTime -= maxTickTime;
    }
    m_gameUi.update(remainingTime);
    m_gameServer.update(remainingTime);
    m_gameClient.update(remainingTime);
}

void ClientServerTestingState::draw(pou::RenderWindow *renderWindow)
{
    if(renderWindow->getRenderer(pou::Renderer_Scene) != nullptr)
    {
        pou::SceneRenderer *renderer = dynamic_cast<pou::SceneRenderer*>(renderWindow->getRenderer(pou::Renderer_Scene));
        //if(m_scene)
        //    m_scene->render(renderer, m_camera);
    }

    if(renderWindow->getRenderer(pou::Renderer_Ui) != nullptr)
    {
        pou::UiRenderer *renderer = dynamic_cast<pou::UiRenderer*>(renderWindow->getRenderer(pou::Renderer_Ui));
        m_gameUi.render(renderer);
    }

}


