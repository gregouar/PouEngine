#include "states/ClientTestingState.h"

#include "PouEngine/Types.h"
#include "PouEngine/utils/Clock.h"

#include "PouEngine/assets/AssetHandler.h"
#include "PouEngine/assets/SoundBankAsset.h"

#include "PouEngine/renderers/UiRenderer.h"

ClientTestingState::ClientTestingState() :
    m_firstEntering(true)
{
    //ctor
}

ClientTestingState::~ClientTestingState()
{
    this->leaving();
}


void ClientTestingState::init()
{
    m_firstEntering = false;

    pou::SoundBanksHandler::loadAssetFromFile("../data/MasterSoundBank.bank");

    m_gameClient.create(42420);
    m_gameClient.connectToServer(pou::NetAddress(127,0,0,1,46969));

    m_gameUi.init();
}

void ClientTestingState::entered()
{
    if(m_firstEntering)
        this->init();
}

void ClientTestingState::leaving()
{
    m_gameClient.disconnectFromServer();
}

void ClientTestingState::revealed()
{

}

void ClientTestingState::obscuring()
{

}

void ClientTestingState::handleEvents(const EventsManager *eventsManager)
{
    m_gameUi.handleEvents(eventsManager);

    if(eventsManager->keyReleased(GLFW_KEY_ESCAPE))
        m_manager->stop();

    if(eventsManager->isAskingToClose())
        m_manager->stop();

    //glm::vec2 worldMousePos = m_scene->convertScreenToWorldCoord(eventsManager->centeredMousePosition(), m_camera);

    if(eventsManager->keyPressed(GLFW_KEY_Z))
        m_gameClient.disconnectFromServer();
    if(eventsManager->keyPressed(GLFW_KEY_X))
        m_gameClient.connectToServer(pou::NetAddress(127,0,0,1,46969));

    if(eventsManager->keyPressed(GLFW_KEY_I))
        m_gameClient.sendMsgTest(false,true);
}



void ClientTestingState::update(const pou::Time &elapsedTime)
{
    //m_gameUi.updateCharacterLife(m_character->getAttributes().life,
    //                             m_character->getAttributes().maxLife);


    m_gameUi.update(elapsedTime);
    m_gameClient.update(elapsedTime);
}

void ClientTestingState::draw(pou::RenderWindow *renderWindow)
{
    m_gameClient.render(renderWindow);

    if(renderWindow->getRenderer(pou::Renderer_Ui) != nullptr)
    {
        pou::UiRenderer *renderer = dynamic_cast<pou::UiRenderer*>(renderWindow->getRenderer(pou::Renderer_Ui));
        m_gameUi.render(renderer);
    }

}



