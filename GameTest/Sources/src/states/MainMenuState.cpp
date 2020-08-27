#include "states/MainMenuState.h"


#include "PouEngine/Types.h"
#include "PouEngine/system/Clock.h"

#include "PouEngine/assets/AssetHandler.h"
#include "PouEngine/assets/SoundBankAsset.h"
#include "PouEngine/assets/FontAsset.h"

#include "PouEngine/renderers/UiRenderer.h"

#include "PouEngine/core/Config.h"

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

    auto title = std::make_shared<pou::UiText>();
    m_ui.addUiElement(title);
    title->setText("ProjectW");
    title->setFontSize(64);
    title->setFont(font);
    title->transform()->setPosition(pou::Config::getInt("window","width")/2,100);
    title->setTextAlign(pou::TextAlignType_Center);


    auto menuElement = std::make_shared<pou::UiElement>();
    menuElement->transform()->setPosition(pou::Config::getInt("window","width")/2,100);
    m_ui.addUiElement(menuElement);


    auto serverAddressTxt = std::make_shared<pou::UiText>();
    serverAddressTxt->setText("Server address:");
    serverAddressTxt->setFontSize(24);
    serverAddressTxt->setFont(font);
    //serverAddressTxt->transform()->setPosition(200,300);
    serverAddressTxt->transform()->setPosition(-250,300);
    menuElement->addChildElement(serverAddressTxt);

    m_serverAddressInput = std::make_shared<pou::UiTextInput>();
    m_serverAddressInput->setText("127.0.0.1");
    m_serverAddressInput->setFontSize(24);
    m_serverAddressInput->setFont(font);
    //m_serverAddressInput->transform()->setPosition(400,300);
    m_serverAddressInput->transform()->setPosition(-50,300);
    m_serverAddressInput->setSize(160,30);
    m_serverAddressInput->setMaxTextLength(15);
    menuElement->addChildElement(m_serverAddressInput);

    auto serverAddressInput_background = std::make_shared<pou::UiPicture>();
    serverAddressInput_background->transform()->setPosition(-8,0,-1);
    serverAddressInput_background->setSize(176,30);
    serverAddressInput_background->setColor({.25,.25,.25,1});
    m_serverAddressInput->addChildElement(serverAddressInput_background);

    auto serverAddressTxt_col = std::make_shared<pou::UiText>();
    serverAddressTxt_col->setText(":");
    serverAddressTxt_col->setFontSize(24);
    serverAddressTxt_col->setFont(font);
    //serverAddressTxt_col->transform()->setPosition(576,300);
    serverAddressTxt_col->transform()->setPosition(126,300);
    menuElement->addChildElement(serverAddressTxt_col);

    m_serverPortInput = std::make_shared<pou::UiTextInput>();
    m_serverPortInput->setText("46969");
    m_serverPortInput->setFontSize(24);
    m_serverPortInput->setFont(font);
    //m_serverPortInput->transform()->setPosition(596,300);
    m_serverPortInput->transform()->setPosition(146,300);
    m_serverPortInput->setSize(100,30);
    m_serverPortInput->setMaxTextLength(5);
    menuElement->addChildElement(m_serverPortInput);

    auto serverPortInput_background = std::make_shared<pou::UiPicture>();
    serverPortInput_background->transform()->setPosition(-8,0,-1);
    serverPortInput_background->setSize(116,30);
    serverPortInput_background->setColor({.25,.25,.25,1});
    m_serverPortInput->addChildElement(serverPortInput_background);

    ///
    ///Character name choice
    ///
    auto characterNameText = std::make_shared<pou::UiText>();
    characterNameText->setText("Character name:");
    characterNameText->setFontSize(24);
    characterNameText->setFont(font);
    //characterNameText->transform()->setPosition(200,350);
    characterNameText->transform()->setPosition(-250,350);
    menuElement->addChildElement(characterNameText);

    m_characterNameInput = std::make_shared<pou::UiTextInput>();
    m_characterNameInput->setText("Poupou");
    m_characterNameInput->setFontSize(24);
    m_characterNameInput->setFont(font);
    //m_characterNameInput->transform()->setPosition(400,350);
    m_characterNameInput->transform()->setPosition(-50,350);
    m_characterNameInput->setSize(296,30);
    m_characterNameInput->setMaxTextLength(24);
    menuElement->addChildElement(m_characterNameInput);

    auto characterNameInput_background = std::make_shared<pou::UiPicture>();
    characterNameInput_background->transform()->setPosition(-8,0,-1);
    characterNameInput_background->setSize(m_characterNameInput->getSize().x+16,30);
    characterNameInput_background->setColor({.25,.25,.25,1});
    m_characterNameInput->addChildElement(characterNameInput_background);

    ///
    ///Buttons
    ///
    m_createServerButton = std::make_shared<pou::UiButton>();
    menuElement->addChildElement(m_createServerButton);
    //m_createServerButton->transform()->setPosition(200,400);
    m_createServerButton->transform()->setPosition(-250,400);
    m_createServerButton->setSize(200,50);
    m_createServerButton->setColor(pou::UiButtonState_Rest, {.5,.5,.5,1});
    m_createServerButton->setColor(pou::UiButtonState_Hover, {.75,.75,.75,1});
    m_createServerButton->setColor(pou::UiButtonState_Pressed, {.25,.25,.25,1});
    m_createServerButton->setColor(pou::UiButtonState_Released, {1,0,0,1});
    m_createServerButton->setLabel("Create Server",24,{1,1,1,1},font);

    m_connectToServerButton = std::make_shared<pou::UiButton>();
    menuElement->addChildElement(m_connectToServerButton);
    //m_connectToServerButton->transform()->setPosition(504,400);
    m_connectToServerButton->transform()->setPosition(54,400);
    m_connectToServerButton->setSize(200,50);
    m_connectToServerButton->setColor(pou::UiButtonState_Rest, {.5,.5,.5,1});
    m_connectToServerButton->setColor(pou::UiButtonState_Hover, {.75,.75,.75,1});
    m_connectToServerButton->setColor(pou::UiButtonState_Pressed, {.25,.25,.25,1});
    m_connectToServerButton->setColor(pou::UiButtonState_Released, {1,0,0,1});
    m_connectToServerButton->setLabel("Connect To Server",24,{1,1,1,1},font);

    ///
    ///Choose your character
    ///
    auto chooseCharacterText = std::make_shared<pou::UiText>();
    chooseCharacterText->setText("Choose your character:");
    chooseCharacterText->setFontSize(24);
    chooseCharacterText->setFont(font);
    //chooseCharacterText->transform()->setPosition(200,500);
    chooseCharacterText->transform()->setPosition(-250,500);
    menuElement->addChildElement(chooseCharacterText);

    m_charSelectButtons = std::make_shared<pou::UiToggleButtonsGroup>();
    menuElement->addChildElement(m_charSelectButtons);

    for(int i = 0 ; i < 5 ; ++i)
    {
        auto charSelectButton = std::make_shared<pou::UiButton>();
        charSelectButton->transform()->setPosition(-250+100*i,550);
        charSelectButton->setSize(80,80);
        charSelectButton->setColor(pou::UiButtonState_Rest, {.5,.5,.5,1});
        charSelectButton->setColor(pou::UiButtonState_Hover, {.75,.75,.75,1});
        charSelectButton->setColor(pou::UiButtonState_Pressed, {.25,.25,.25,1});
        charSelectButton->setColor(pou::UiButtonState_Released, {1,0,0,1});
        m_charSelectButtons->addButton(charSelectButton, i);
        //m_ui.addRootElement(m_charSelectButton[i]);

        auto charSelectButtonPicture = std::make_shared<pou::UiPicture>();
        charSelectButtonPicture->setSize(charSelectButton->getSize());
        charSelectButtonPicture->setTexture(pou::TexturesHandler::loadAssetFromFile("../data/ui/char"+std::to_string(i+1)+".png"));
        charSelectButtonPicture->transform()->setPosition(0,0,1);
        charSelectButton->addChildElement(charSelectButtonPicture);

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

    ///I should add event system heh or simply use the notification system
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

    clientState->setConnectionData(serverAddress, playerSave, false);

    m_manager->switchState(clientState);
}

void MainMenuState::notify(pou::NotificationSender*, int notificationType,
                    void* data)
{

}

