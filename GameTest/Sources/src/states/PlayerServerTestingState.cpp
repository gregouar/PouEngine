#include "states/PlayerServerTestingState.h"


#include "PouEngine/Types.h"
#include "PouEngine/system/Clock.h"

#include "PouEngine/assets/AssetHandler.h"
#include "PouEngine/assets/SoundBankAsset.h"

#include "PouEngine/renderers/UiRenderer.h"

#include "states/MainMenuState.h"
#include "states/InGameState.h"

#include "logic/GameData.h"

PlayerServerTestingState::PlayerServerTestingState() :
    m_firstEntering(true)
{
    //ctor
}

PlayerServerTestingState::~PlayerServerTestingState()
{
    this->leaving();
}


void PlayerServerTestingState::init()
{
    m_firstEntering = false;
}

void PlayerServerTestingState::entered()
{
    if(m_firstEntering)
        this->init();

    m_gameServer.create(m_port,true);
    m_gameServer.generateWorld();

    int localClientNbr = m_gameServer.addLocalPlayer(m_playerSave);

    auto inGameState = InGameState::instance();
    inGameState->setClientId(localClientNbr);

    m_manager->pushState(inGameState);
    auto [clientInfos, world]  = m_gameServer.getClientInfosAndWorld(localClientNbr);
    inGameState->changeWorld(world, clientInfos->player_id);
}

void PlayerServerTestingState::leaving()
{
    m_gameServer.shutdown();
}

void PlayerServerTestingState::revealed()
{

}

void PlayerServerTestingState::obscuring()
{

}


void PlayerServerTestingState::handleEvents(const EventsManager *eventsManager)
{
    if(eventsManager->keyReleased(GLFW_KEY_ESCAPE))
        m_manager->switchState(MainMenuState::instance());

    if(eventsManager->isAskingToClose())
        m_manager->stop();

    if(eventsManager->keyPressed(GLFW_KEY_I))
        m_gameServer.sendMsgTest(false,true);
    if(eventsManager->keyPressed(GLFW_KEY_O))
        m_gameServer.sendMsgTest(true,false);
}


void PlayerServerTestingState::update(const pou::Time &elapsedTime)
{
    m_gameServer.update(elapsedTime);
}

void PlayerServerTestingState::draw(pou::RenderWindow *renderWindow)
{
}

///
///Protected
///

void PlayerServerTestingState::setConnectionData(int port, std::shared_ptr<PlayerSave> playerSave)
{
    m_port = port;
    m_playerSave = playerSave;
}

