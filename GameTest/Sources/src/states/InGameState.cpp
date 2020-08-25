#include "states/InGameState.h"


#include "PouEngine/Types.h"
#include "PouEngine/system/Clock.h"

#include "PouEngine/assets/AssetHandler.h"
#include "PouEngine/assets/SoundBankAsset.h"

#include "PouEngine/core/MessageBus.h"

#include "PouEngine/renderers/UiRenderer.h"

#include "states/MainMenuState.h"

#include "logic/GameMessageTypes.h"
#include "logic/GameData.h"

InGameState::InGameState() :
    m_firstEntering(true),
    m_clientId(0),
    m_world(nullptr)
{
}

InGameState::~InGameState()
{
    this->leaving();
}


void InGameState::init()
{
    m_firstEntering = false;

    pou::MessageBus::addListener(this, GameMessageType_Game_ChangeWorld);

    m_gameUi.init();
}

void InGameState::entered()
{
    if(m_firstEntering)
        this->init();
}

void InGameState::leaving()
{
    m_world = nullptr;
    m_gameUi.clearPlayersList();
    //m_gameUi.setPlayer(nullptr);
}

void InGameState::revealed()
{

}

void InGameState::obscuring()
{

}


void InGameState::handleEvents(const EventsManager *eventsManager)
{
    m_gameUi.handleEvents(eventsManager);

    if(eventsManager->keyReleased(GLFW_KEY_ESCAPE))
        m_manager->switchState(MainMenuState::instance());

    if(eventsManager->isAskingToClose())
        m_manager->stop();

    if(!m_world)
        return;

    auto cursorCoord = eventsManager->centeredMousePosition();
    auto worldCursorCoord = m_world->convertScreenToWorldCoord(cursorCoord);
    auto playerCoord = m_world->getPlayer(m_playerId)->transform()->getGlobalXYPosition();
    auto localPlayerCursorCoord = worldCursorCoord - playerCoord;

    /*std::cout<<"cursorCoord: "<<cursorCoord.x<<" "<<cursorCoord.y<<std::endl;
    std::cout<<"worldCursorCoord: "<<worldCursorCoord.x<<" "<<worldCursorCoord.y<<std::endl;
    std::cout<<"localPlayerCursorCoord: "<<localPlayerCursorCoord.x<<" "<<localPlayerCursorCoord.y<<std::endl;
    std::cout<<std::endl;*/

    if(eventsManager->keyPressed(GLFW_KEY_1))
        m_world->addPlayerAction(m_playerId, PlayerAction(PlayerActionType_UseItem,1));
    if(eventsManager->keyPressed(GLFW_KEY_2))
        m_world->addPlayerAction(m_playerId, PlayerAction(PlayerActionType_UseItem,2));
    if(eventsManager->keyPressed(GLFW_KEY_3))
        m_world->addPlayerAction(m_playerId, PlayerAction(PlayerActionType_UseItem,3));
    if(eventsManager->keyPressed(GLFW_KEY_4))
        m_world->addPlayerAction(m_playerId, PlayerAction(PlayerActionType_UseItem,4));
    if(eventsManager->keyPressed(GLFW_KEY_5))
        m_world->addPlayerAction(m_playerId, PlayerAction(PlayerActionType_UseItem,5));

    glm::vec2 charDirection = {0,0};
    if(eventsManager->keyIsPressed(GLFW_KEY_S))
        charDirection.y = 1;
    if(eventsManager->keyIsPressed(GLFW_KEY_W))
        charDirection.y = -1;
    if(eventsManager->keyIsPressed(GLFW_KEY_A))
        charDirection.x = -1;
    if(eventsManager->keyIsPressed(GLFW_KEY_D))
        charDirection.x = 1;
    if(eventsManager->keyPressed(GLFW_KEY_LEFT_ALT))
        m_world->addPlayerAction(m_playerId,
                                         PlayerAction(PlayerActionType_Dash));
    m_world->addPlayerAction(m_playerId,
                                     PlayerAction(PlayerActionType_Walk, charDirection));

    m_world->addPlayerAction(m_playerId,
                                     PlayerAction(PlayerActionType_CursorMove, localPlayerCursorCoord));

    if(eventsManager->mouseButtonIsPressed(GLFW_MOUSE_BUTTON_1))
        m_world->addPlayerAction(m_playerId,
                                         PlayerAction(PlayerActionType_Look, localPlayerCursorCoord));
    if(eventsManager->mouseButtonIsPressed(GLFW_MOUSE_BUTTON_2))
        m_world->addPlayerAction(m_playerId,
                                        PlayerAction(PlayerActionType_Attack, localPlayerCursorCoord));
    if(eventsManager->keyIsPressed(GLFW_KEY_LEFT_SHIFT))
        m_world->addPlayerAction(m_playerId,
                                         PlayerAction(PlayerActionType_CombatMode));

    if(m_gameUi.askForRespawn())
        m_world->addPlayerAction(m_playerId,
                                         PlayerAction(PlayerActionType_Respawn));
}


void InGameState::update(const pou::Time &elapsedTime)
{
    /*if(m_world)
    {
        auto player = m_world->getPlayer(m_playerId);
        m_gameUi->setPlayer(player);
        if(player)
            m_gameUi.updatePlayerLife(player->getAttributes().life,
                                      player->getModelAttributes().maxLife);
    }*/

    m_gameUi.update(elapsedTime);
}

void InGameState::draw(pou::RenderWindow *renderWindow)
{
    //m_gameServer.render(renderWindow, m_playerId);
    if(m_world)
        m_world->render(renderWindow);

    if(renderWindow->getRenderer(pou::Renderer_Ui) != nullptr)
    {
        pou::UiRenderer *renderer = dynamic_cast<pou::UiRenderer*>(renderWindow->getRenderer(pou::Renderer_Ui));
        m_gameUi.render(renderer);
    }
}


void InGameState::changeWorld(GameWorld *world, int playerId)
{
    m_world = world;
    m_playerId = playerId;

    Player *player = nullptr;
    if(m_world)
        player = m_world->getPlayer(m_playerId);
    m_gameUi.setPlayer(player);
    //m_gameUi.clearPlayerList();
}

void InGameState::setClientId(int clientId)
{
    m_clientId = clientId;
}

void InGameState::setRTTInfo(float RTT)
{
    m_gameUi.setRTTInfo(RTT);
}

///
///Protected
///

void InGameState::notify(pou::NotificationSender*, int notificationType, void* data)
{
    if(notificationType == GameMessageType_Game_ChangeWorld)
    {
        auto *gameMsg = static_cast<GameMessage_Game_ChangeWorld*>(data);

        if(gameMsg->clientId != (int)m_clientId)
            return;

        this->changeWorld(gameMsg->world, gameMsg->playerId);
    }
}


