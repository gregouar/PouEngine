#include "ui/GameUi.h"

#include "PouEngine/Types.h"
#include "PouEngine/assets/AssetHandler.h"
#include "PouEngine/assets/TextureAsset.h"
#include "PouEngine/assets/FontAsset.h"

GameUi::GameUi() :
    m_player(nullptr)
{
}

GameUi::~GameUi()
{
}


bool GameUi::init()
{
    m_lifeBar = this->createProgressBar(true);
    m_lifeBar->setPosition(17,30);
    m_lifeBar->setSize({288-34,32});
    m_lifeBar->setTextureRect({17,0},{288-34,32},false);
    m_lifeBar->setTexture(pou::TexturesHandler::loadAssetFromFile("../data/ui/Life_bar.png"));

    m_uiPictureTest = this->createUiPicture(false);
    m_uiPictureTest->setPosition(-17,0,-1);
    m_uiPictureTest->setSize(288,32);
    m_uiPictureTest->setTexture(pou::TexturesHandler::loadAssetFromFile("../data/ui/Life_bar_blank.png"));
    m_lifeBar->addChildNode(m_uiPictureTest);

    m_testText = std::make_shared<pou::UiText>(this);
    m_testText->setPosition(0,0,0);
    m_testText->setFont(pou::FontsHandler::loadAssetFromFile("../data/UASQUARE.TTF"));
    //m_testText->setText("Poupou is da Pou");
    m_testText->setFontSize(24);
    m_testText->setSize(288, 0);
    m_testText->setTextAlign(pou::TextAlignType_Center);
    this->addRootElement(m_testText);

    return (true);
}

void GameUi::update(const pou::Time &elapsed_time)
{
    this->updatePlayerLife();

    if(m_player)
        m_testText->setText(m_player->getPlayerName());
    else
        m_testText->setText(std::string());

    UserInterface::update(elapsed_time);
}

void GameUi::setPlayer(Player *player)
{
    m_player = player;

    this->startListeningTo(player, pou::NotificationType_SenderDestroyed);
}

void GameUi::updatePlayerLife(/*float cur, float max*/)
{
    if(!m_player)
    {
        m_lifeBar->hide();
        return;
    }
    else
        m_lifeBar->show();

    auto playerLife = m_player->getAttributes().life;
    auto playerMaxLife = m_player->getModelAttributes().maxLife;

    m_lifeBar->setMinMaxValue(0,playerMaxLife);
    m_lifeBar->setValue(playerLife);
}


void GameUi::notify(pou::NotificationSender* sender, int notificationType,
                    void* data)
{
    ///pou::UserInterface::notify(sender, notificationType, data);

    if(notificationType == pou::NotificationType_SenderDestroyed
    && sender == m_player)
        m_player = nullptr;
}
