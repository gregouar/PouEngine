#include "states/MainMenuState.h"


#include "PouEngine/Types.h"
#include "PouEngine/system/Clock.h"

#include "PouEngine/assets/AssetHandler.h"
#include "PouEngine/assets/SoundBankAsset.h"
#include "PouEngine/assets/FontAsset.h"

#include "PouEngine/renderers/UiRenderer.h"

#include "PouEngine/ui/UiText.h"
#include "PouEngine/ui/UiTextInput.h"


#include "states/ClientTestingState.h"
#include "states/PlayerServerTestingState.h"

#include "logic/GameData.h"



MainMenuState::MainMenuState() :
    m_firstEntering(true)
{
    //ctor
}

MainMenuState::~MainMenuState()
{
    this->leaving();
}


void MainMenuState::init()
{
    m_firstEntering = false;

    pou::SoundBanksHandler::loadAssetFromFile("../data/MasterSoundBank.bank");

    auto font = pou::FontsHandler::loadAssetFromFile("../data/UASQUARE.TTF");

    auto title = std::make_shared<pou::UiText>(&m_ui);
    title->setText("ProjectW");
    title->setFontSize(64);
    title->setFont(font);
    title->setPosition(1024/2,100);
    title->setTextAlign(pou::TextAlignType_Center);
    m_ui.addRootElement(title);

    auto serverAddressTxt = std::make_shared<pou::UiText>(&m_ui);
    serverAddressTxt->setText("Server address:");
    serverAddressTxt->setFontSize(24);
    serverAddressTxt->setFont(font);
    serverAddressTxt->setPosition(200,300);
    m_ui.addRootElement(serverAddressTxt);

    m_serverAddressInput = std::make_shared<pou::UiTextInput>(&m_ui);
    m_serverAddressInput->setText("127.0.0.1"/*GameData::serverAddress.getIpAddressString()*/);
    m_serverAddressInput->setFontSize(24);
    m_serverAddressInput->setFont(font);
    m_serverAddressInput->setPosition(400,300);
    m_serverAddressInput->setSize(160,30);
    m_serverAddressInput->setMaxTextLength(15);
    m_ui.addRootElement(m_serverAddressInput);

    auto serverAddressInput_background = std::make_shared<pou::UiPicture>(&m_ui);
    serverAddressInput_background->setPosition(-8,0,-1);
    serverAddressInput_background->setSize(176,30);
    serverAddressInput_background->setColor({.25,.25,.25,1});
    m_serverAddressInput->addChildNode(serverAddressInput_background);

    auto serverAddressTxt_col = std::make_shared<pou::UiText>(&m_ui);
    serverAddressTxt_col->setText(":");
    serverAddressTxt_col->setFontSize(24);
    serverAddressTxt_col->setFont(font);
    serverAddressTxt_col->setPosition(576,300);
    m_ui.addRootElement(serverAddressTxt_col);

    m_serverPortInput = std::make_shared<pou::UiTextInput>(&m_ui);
    m_serverPortInput->setText("46969"/*GameData::serverAddress.getPortString()*/);
    m_serverPortInput->setFontSize(24);
    m_serverPortInput->setFont(font);
    m_serverPortInput->setPosition(596,300);
    m_serverPortInput->setSize(100,30);
    m_serverPortInput->setMaxTextLength(5);
    m_ui.addRootElement(m_serverPortInput);

    auto serverPortInput_background = std::make_shared<pou::UiPicture>(&m_ui);
    serverPortInput_background->setPosition(-8,0,-1);
    serverPortInput_background->setSize(116,30);
    serverPortInput_background->setColor({.25,.25,.25,1});
    m_serverPortInput->addChildNode(serverPortInput_background);

    ///
    ///Character name choice
    ///
    auto characterNameText = std::make_shared<pou::UiText>(&m_ui);
    characterNameText->setText("Character name:");
    characterNameText->setFontSize(24);
    characterNameText->setFont(font);
    characterNameText->setPosition(200,350);
    m_ui.addRootElement(characterNameText);

    m_characterNameInput = std::make_shared<pou::UiTextInput>(&m_ui);
    m_characterNameInput->setText("Poupou");
    m_characterNameInput->setFontSize(24);
    m_characterNameInput->setFont(font);
    m_characterNameInput->setPosition(400,350);
    m_characterNameInput->setSize(296,30);
    m_characterNameInput->setMaxTextLength(24);
    m_ui.addRootElement(m_characterNameInput);

    auto characterNameInput_background = std::make_shared<pou::UiPicture>(&m_ui);
    characterNameInput_background->setPosition(-8,0,-1);
    characterNameInput_background->setSize(m_characterNameInput->getSize().x+16,30);
    characterNameInput_background->setColor({.25,.25,.25,1});
    m_characterNameInput->addChildNode(characterNameInput_background);

    ///
    ///Buttons
    ///

    m_createServerButton = std::make_shared<pou::UiButton>(&m_ui);
    m_createServerButton->setPosition(200,400);
    m_createServerButton->setSize(200,50);
    m_createServerButton->setColor(pou::UiButtonState_Rest, {.5,.5,.5,1});
    m_createServerButton->setColor(pou::UiButtonState_Hover, {.75,.75,.75,1});
    m_createServerButton->setColor(pou::UiButtonState_Pressed, {.25,.25,.25,1});
    m_createServerButton->setColor(pou::UiButtonState_Released, {1,0,0,1});
    m_createServerButton->setLabel("Create Server",24,{1,1,1,1},font);
    m_ui.addRootElement(m_createServerButton);

    m_connectToServerButton = std::make_shared<pou::UiButton>(&m_ui);
    m_connectToServerButton->setPosition(504,400);
    m_connectToServerButton->setSize(200,50);
    m_connectToServerButton->setColor(pou::UiButtonState_Rest, {.5,.5,.5,1});
    m_connectToServerButton->setColor(pou::UiButtonState_Hover, {.75,.75,.75,1});
    m_connectToServerButton->setColor(pou::UiButtonState_Pressed, {.25,.25,.25,1});
    m_connectToServerButton->setColor(pou::UiButtonState_Released, {1,0,0,1});
    m_connectToServerButton->setLabel("Connect To Server",24,{1,1,1,1},font);
    m_ui.addRootElement(m_connectToServerButton);

    ///
    ///Choose your character
    ///
    auto chooseCharacterText = std::make_shared<pou::UiText>(&m_ui);
    chooseCharacterText->setText("Choose your character:");
    chooseCharacterText->setFontSize(24);
    chooseCharacterText->setFont(font);
    chooseCharacterText->setPosition(200,500);
    m_ui.addRootElement(chooseCharacterText);

    m_charSelectButtons = std::make_shared<pou::UiToggleButtonsGroup>(&m_ui);
    m_ui.addRootElement(m_charSelectButtons);

    for(int i = 0 ; i < 3 ; ++i)
    {
        auto charSelectButton = std::make_shared<pou::UiButton>(&m_ui);
        charSelectButton->setPosition(200+100*i,550);
        charSelectButton->setSize(80,80);
        charSelectButton->setColor(pou::UiButtonState_Rest, {.5,.5,.5,1});
        charSelectButton->setColor(pou::UiButtonState_Hover, {.75,.75,.75,1});
        charSelectButton->setColor(pou::UiButtonState_Pressed, {.25,.25,.25,1});
        charSelectButton->setColor(pou::UiButtonState_Released, {1,0,0,1});
        m_charSelectButtons->addButton(charSelectButton, i);
        //m_ui.addRootElement(m_charSelectButton[i]);

        auto charSelectButtonPicture = std::make_shared<pou::UiPicture>(&m_ui);
        charSelectButtonPicture->setSize(charSelectButton->getSize());
        charSelectButtonPicture->setTexture(pou::TexturesHandler::loadAssetFromFile("../data/ui/char"+std::to_string(i+1)+".png"));
        charSelectButtonPicture->setPosition(0,0,1);
        charSelectButton->addChildNode(charSelectButtonPicture);

        if(i == 0)
            charSelectButton->setToggled(true);
    }
}

void MainMenuState::entered()
{
    if(m_firstEntering)
        this->init();
}

void MainMenuState::leaving()
{
    //GameData::serverAddress = pou::NetAddress(m_serverAddressInput->getText(),
    //                                          m_serverPortInput->getText());
}

void MainMenuState::revealed()
{

}

void MainMenuState::obscuring()
{

}


void MainMenuState::handleEvents(const EventsManager *eventsManager)
{
    m_ui.handleEvents(eventsManager);

    if(eventsManager->keyReleased(GLFW_KEY_ESCAPE))
        m_manager->stop();

    if(eventsManager->isAskingToClose())
        m_manager->stop();
}


void MainMenuState::update(const pou::Time &elapsedTime)
{
    m_ui.update(elapsedTime);

    if(m_createServerButton->isClicked())
        this->createServerAction();
    else if(m_connectToServerButton->isClicked())
        this->connectionToServerAction();
}

void MainMenuState::draw(pou::RenderWindow *renderWindow)
{
    if(renderWindow->getRenderer(pou::Renderer_Ui) != nullptr)
    {
        pou::UiRenderer *renderer = dynamic_cast<pou::UiRenderer*>(renderWindow->getRenderer(pou::Renderer_Ui));
        m_ui.render(renderer);
    }
}

///
///Protected
///

void MainMenuState::createServerAction()
{
    auto serverState = PlayerServerTestingState::instance();
    auto serverAddress = pou::NetAddress(m_serverAddressInput->getText(),
                                         m_serverPortInput->getText());

    auto playerSave = std::make_shared<PlayerSave>();
    playerSave->setPlayerName(m_characterNameInput->getText());
    playerSave->setPlayerType(m_charSelectButtons->getValue());

    serverState->setConnectionData(serverAddress.getPort(), playerSave);

    m_manager->switchState(serverState);
}

void MainMenuState::connectionToServerAction()
{
    auto clientState = ClientTestingState::instance();
    auto serverAddress = pou::NetAddress(m_serverAddressInput->getText(),
                                         m_serverPortInput->getText());

    auto playerSave = std::make_shared<PlayerSave>();
    playerSave->setPlayerName(m_characterNameInput->getText());
    playerSave->setPlayerType(m_charSelectButtons->getValue());

    clientState->setConnectionData(serverAddress, playerSave);

    m_manager->switchState(clientState);
}

void MainMenuState::notify(pou::NotificationSender*, int notificationType,
                    void* data)
{

}

