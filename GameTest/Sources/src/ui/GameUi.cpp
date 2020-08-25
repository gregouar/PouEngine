#include "ui/GameUi.h"

#include "PouEngine/Types.h"
#include "PouEngine/assets/AssetHandler.h"
#include "PouEngine/assets/TextureAsset.h"
#include "PouEngine/assets/FontAsset.h"
#include "PouEngine/core/Config.h"

#include "logic/GameMessageTypes.h"


GameUi::GameUi() :
    m_player(nullptr)
{
    pou::MessageBus::addListener(this, GameMessageType_World_NewPlayer);
}

GameUi::~GameUi()
{
}


bool GameUi::init()
{
    /// CHANGE pou::Config::getInt BY SOMETHING ELSE, NEED TO GET ACCESS TO THE RENDERWINDOW ACTUALLY
    auto windowSize = glm::vec2(pou::Config::getInt("window","width"),
                                pou::Config::getInt("window","height"));

    ///Player Hud //////////
    m_playerHud = std::make_shared<pou::UiElement>();
    this->addUiElement(m_playerHud);

    m_lifeBar =std::make_unique<pou::UiProgressBar>();
    m_lifeBar->transform()->setPosition(17,30);
    m_lifeBar->setSize({288-34,32});
    m_lifeBar->setTextureRect({17,0},{288-34,32},false);
    m_lifeBar->setTexture(pou::TexturesHandler::loadAssetFromFile("../data/ui/Life_bar.png"));
    m_playerHud->addChildElement(m_lifeBar);

    auto lifeBarBackground = std::make_shared<pou::UiPicture>();
    lifeBarBackground->transform()->setPosition(-17,0,-1);
    lifeBarBackground->setSize(288,32);
    lifeBarBackground->setTexture(pou::TexturesHandler::loadAssetFromFile("../data/ui/Life_bar_blank.png"));
    m_lifeBar->addChildElement(lifeBarBackground);

    m_font = pou::FontsHandler::loadAssetFromFile("../data/UASQUARE.TTF");
    m_font_pixel = pou::FontsHandler::loadAssetFromFile("../data/mago1.TTF");

    m_charNameText = std::make_shared<pou::UiText>();
    m_charNameText->transform()->setPosition(0,0,0);
    m_charNameText->setFont(m_font);
    m_charNameText->setFontSize(24);
    m_charNameText->setSize(288, 0);
    m_charNameText->setTextAlign(pou::TextAlignType_Center);
    m_playerHud->addChildElement(m_charNameText);

    ///You Died ///////////
    m_youDiedUi = std::make_shared<pou::UiElement>();
    this->addUiElement(m_youDiedUi);
    m_youDiedUi->hide();

    auto youDiedBackground = std::make_shared<pou::UiPicture>();
    youDiedBackground->setColor(glm::vec4(0.0,0.0,0.0,.5));
    youDiedBackground->setSize(windowSize.x, windowSize.y);
    m_youDiedUi->addChildElement(youDiedBackground);

    auto youDiedText = std::make_shared<pou::UiText>();
    youDiedText->transform()->setPosition(windowSize.x/2,
                                          windowSize.y/2 - 108,
                                          1);
    youDiedText->setFont(m_font);
    youDiedText->setFontSize(72);
    youDiedText->setTextAlign(pou::TextAlignType_Center);
    youDiedText->setText("You Died");
    youDiedText->setColor(glm::vec4(1.0,0.0,0.0,1.0));
    m_youDiedUi->addChildElement(youDiedText);

    m_respawnButton = std::make_shared<pou::UiButton>();
    m_respawnButton->transform()->setPosition(windowSize.x/2,
                                            windowSize.y/2 + 72,
                                            1);
    m_respawnButton->setSizeAndCenter(200,50);
    m_respawnButton->setColor(pou::UiButtonState_Rest, {.5,.5,.5,1});
    m_respawnButton->setColor(pou::UiButtonState_Hover, {.75,.75,.75,1});
    m_respawnButton->setColor(pou::UiButtonState_Pressed, {.25,.25,.25,1});
    m_respawnButton->setColor(pou::UiButtonState_Released, {1,0,0,1});
    m_respawnButton->setLabel("Respawn",24,{1,1,1,1},m_font);
    m_youDiedUi->addChildElement(m_respawnButton);

    ///Infos /////////////////

    m_RTTText = std::make_shared<pou::UiText>();
    m_RTTText->transform()->setPosition(windowSize.x,0,0);
    m_RTTText->setFont(m_font);
    m_RTTText->setFontSize(16);
    m_RTTText->setTextAlign(pou::TextAlignType_Right);
    this->addUiElement(m_RTTText);

    return (true);
}

void GameUi::update(const pou::Time &elapsed_time)
{
    this->updatePlayerHud();
    this->updatePlayerList();

    UserInterface::update(elapsed_time);
}

void GameUi::setPlayer(Player *player)
{
    m_player = player;

    this->startListeningTo(player, pou::NotificationType_SenderDestroyed);
}

void GameUi::setRTTInfo(float RTT)
{
    if(RTT == 0)
    {
        m_RTTText->hide();
        return;
    }

    m_RTTText->setText("Ping: "+std::to_string((int)(RTT*1000)/2)+" ms");
    m_RTTText->show();
}

bool GameUi::askForRespawn()
{
    return (m_respawnButton->isClicked());
}

///
///Protected
///

void GameUi::updatePlayerHud()
{
    if(!m_player)
    {
        m_playerHud->hide();
        return;
    }
    else
    {
        m_playerHud->show();
        m_charNameText->setText(m_player->getPlayerName());
    }

    auto playerLife = m_player->getAttributes().life;
    auto playerMaxLife = m_player->getModelAttributes().maxLife;

    m_lifeBar->setMinMaxValue(0,playerMaxLife);
    m_lifeBar->setValue(playerLife);

    if(!m_player->isAlive())
        m_youDiedUi->show();
    else
        m_youDiedUi->hide();
}


void GameUi::updatePlayerList()
{
    if(!m_player)
        return;

    auto playerPos = m_player->transform()->getGlobalXYPosition();

    for(auto &p : m_playerList)
    if(p.player != m_player)
    {
        auto otherPlayerPos = p.player->transform()->getGlobalXYPosition();

        auto screenSize = glm::vec2(pou::Config::getInt("window","width"),
                                    pou::Config::getInt("window","height"));

        auto screenpos = otherPlayerPos - playerPos + screenSize*0.5f - glm::vec2(0,64);

        p.playerNameText->setText(p.player->getPlayerName());

        auto textWidth = p.playerNameText->computeTextSize()+p.playerNameText->getFontSize();
        screenpos = glm::clamp(screenpos, glm::vec2(textWidth/2,0), screenSize-glm::vec2(textWidth/2,p.playerNameText->getFontSize()*1.5));

        p.playerNameText->transform()->setPosition(glm::round(screenpos)); ///I should probably replace this by some kind of world->convertCoord
    }
}

void GameUi::addPlayer(Player *player)
{
    /*for(auto p : m_playerList)
        if(p.player == player)
            return;*/

    auto playerNameText = std::make_shared<pou::UiText>(/*this*/);
    playerNameText->setFont(m_font_pixel);
    playerNameText->setFontSize(20);
    playerNameText->setTextAlign(pou::TextAlignType_Center);
    //playerNameText->setText("Player");
    m_playerHud->addChildElement(playerNameText);

    OtherPlayerUi playerUi;
    playerUi.player = player;
    playerUi.playerNameText = playerNameText;
    m_playerList.push_back(playerUi);

    this->startListeningTo(player, pou::NotificationType_SenderDestroyed);
}

void GameUi::removePlayer(Player *player)
{
    if(player == m_player)
        m_player = nullptr;

    for(auto playerIt = m_playerList.begin() ; playerIt != m_playerList.end() ;)
    {
        if(playerIt->player == player)
        {
            playerIt->playerNameText->removeFromParent();
            playerIt = m_playerList.erase(playerIt);
        }
        else
            ++playerIt;
    }
}

void GameUi::clearPlayersList()
{
    while(!m_playerList.empty())
        this->removePlayer(m_playerList.begin()->player);
}


void GameUi::notify(pou::NotificationSender* sender, int notificationType,
                    void* data)
{
    ///pou::UserInterface::notify(sender, notificationType, data);

    if(notificationType == pou::NotificationType_SenderDestroyed)
        this->removePlayer((Player*)sender);

    if(notificationType == GameMessageType_World_NewPlayer)
    {
        auto gameMsg = static_cast<GameMessage_World_NewPlayer*>(data);
        this->addPlayer(gameMsg->player);
    }
}
