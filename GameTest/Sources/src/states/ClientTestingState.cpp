#include "states/ClientTestingState.h"

#include "PouEngine/Types.h"
#include "PouEngine/system/Clock.h"

#include "PouEngine/assets/AssetHandler.h"
#include "PouEngine/assets/SoundBankAsset.h"

#include "PouEngine/renderers/UiRenderer.h"

#include "states/MainMenuState.h"
#include "states/InGameState.h"

#include "logic/GameData.h"

ClientTestingState::ClientTestingState() :
    m_firstEntering(true)
{
    //ctor
}

ClientTestingState::~ClientTestingState()
{
    this->leaving();
        m_gameClient.reset();
}


void ClientTestingState::init()
{
    //m_firstEntering = false;

    m_gameClient = std::make_unique<GameClient>();
    m_gameClient->create();
}

void ClientTestingState::entered()
{
    if(m_firstEntering)
        this->init();

    m_gameClient->connectToServer(m_serverAddress, m_playerSave, m_useLockStepMode);

    auto inGameState = InGameState::instance();
    m_manager->pushState(inGameState);
    //auto [clientInfos, world]  = m_gameServer.getClientInfosAndWorld(m_localClientNbr);
    //inGameState->changeWorld(world, clientInfos->player_id);
}

void ClientTestingState::leaving()
{
    if(m_gameClient)
        m_gameClient->disconnectFromServer();
}

void ClientTestingState::revealed()
{

}

void ClientTestingState::obscuring()
{

}

void ClientTestingState::handleEvents(const EventsManager *eventsManager)
{
    if(eventsManager->keyReleased(GLFW_KEY_ESCAPE))
        m_manager->switchState(MainMenuState::instance());

    if(eventsManager->isAskingToClose())
        m_manager->stop();


    if(!m_gameClient)
        return;

    if(eventsManager->keyPressed(GLFW_KEY_Z))
        m_gameClient->disconnectFromServer();
    if(eventsManager->keyPressed(GLFW_KEY_X))
        m_gameClient->connectToServer(m_serverAddress, m_playerSave);

    if(eventsManager->keyPressed(GLFW_KEY_I))
        m_gameClient->sendMsgTest(false,true);
}



void ClientTestingState::update(const pou::Time &elapsedTime)
{
    if(m_gameClient)
        m_gameClient->update(elapsedTime);


    auto inGameState = InGameState::instance();
    inGameState->setRTTInfo(m_gameClient->getRTT());
}

void ClientTestingState::draw(pou::RenderWindow *renderWindow)
{
}


void ClientTestingState::setConnectionData(const pou::NetAddress &serverAddress,
                                           std::shared_ptr<PlayerSave> playerSave,
                                           bool useLockStepMode)
{
    m_serverAddress = serverAddress;
    m_playerSave = playerSave;
    m_useLockStepMode = useLockStepMode;
}



