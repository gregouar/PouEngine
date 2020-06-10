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

    char dot;
    int a,b,c,d,port;
    std::cout<<"Server address: ";
    if (std::cin.peek() != '\n')
        std::cin>>a>>dot>>b>>dot>>c>>dot>>d;
    else
        a=127, b=0, c=0, d=1;

    if(std::cin.peek() != '\n')
       std::cin>>dot>>port;
    else
        port=46969;

    std::cout<<a<<"."<<b<<"."<<c<<"."<<d<<":"<<port<<std::endl;

    m_gameClient.create();
    m_gameClient.connectToServer(pou::NetAddress(a,b,c,d,port));

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
        m_gameClient.connectToServer(m_gameClient.getServerAddress()/*pou::NetAddress(192,168,0,5,46969)*/);

    if(eventsManager->keyPressed(GLFW_KEY_I))
        m_gameClient.sendMsgTest(false,true);

    if(eventsManager->keyPressed(GLFW_KEY_1))
        m_gameClient.processPlayerAction(PlayerAction(PlayerActionType_UseItem,1));
    if(eventsManager->keyPressed(GLFW_KEY_2))
        m_gameClient.processPlayerAction(PlayerAction(PlayerActionType_UseItem,2));
    if(eventsManager->keyPressed(GLFW_KEY_3))
        m_gameClient.processPlayerAction(PlayerAction(PlayerActionType_UseItem,3));
    if(eventsManager->keyPressed(GLFW_KEY_4))
        m_gameClient.processPlayerAction(PlayerAction(PlayerActionType_UseItem,4));
    if(eventsManager->keyPressed(GLFW_KEY_5))
        m_gameClient.processPlayerAction(PlayerAction(PlayerActionType_UseItem,5));

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
        m_gameClient.processPlayerAction(PlayerAction(PlayerActionType_Dash));
    m_gameClient.processPlayerAction(PlayerAction(PlayerActionType_Walk, charDirection));

    m_gameClient.processPlayerAction(PlayerAction(PlayerActionType_CursorMove, eventsManager->centeredMousePosition()));

    if(eventsManager->mouseButtonIsPressed(GLFW_MOUSE_BUTTON_1))
        m_gameClient.processPlayerAction(PlayerAction(PlayerActionType_Look, eventsManager->centeredMousePosition()));
    if(eventsManager->mouseButtonIsPressed(GLFW_MOUSE_BUTTON_2))
        m_gameClient.processPlayerAction(PlayerAction(PlayerActionType_Attack, eventsManager->centeredMousePosition()));
    if(eventsManager->keyIsPressed(GLFW_KEY_LEFT_SHIFT))
        m_gameClient.processPlayerAction(PlayerAction(PlayerActionType_CombatMode));
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



