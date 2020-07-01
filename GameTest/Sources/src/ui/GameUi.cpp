#include "ui/GameUi.h"

#include "PouEngine/Types.h"
#include "PouEngine/assets/AssetHandler.h"
#include "PouEngine/assets/TextureAsset.h"
#include "PouEngine/assets/FontAsset.h"

GameUi::GameUi()
{
    //ctor
}

GameUi::~GameUi()
{
    //this->cleanup;
}


bool GameUi::init()
{
   // m_mainInterface = std::make_unique<m_mainInterface> ();

    m_lifeBar = this->createProgressBar(true);
    m_lifeBar->setPosition(17,0);
    m_lifeBar->setSize({288-34,32});
    m_lifeBar->setTextureRect({17,0},{288-34,32},false);
    m_lifeBar->setTexture(pou::TexturesHandler::loadAssetFromFile("../data/ui/Life_bar.png"));

    m_uiPictureTest = this->createUiPicture(false);
    m_uiPictureTest->setPosition(-17,0,-1);
    m_uiPictureTest->setSize(288,32);
    m_uiPictureTest->setTexture(pou::TexturesHandler::loadAssetFromFile("../data/ui/Life_bar_blank.png"));
    m_lifeBar->addChildNode(m_uiPictureTest);

    m_testText = std::make_shared<pou::UiText>(this);
    m_testText->setPosition(200,200,0);
    m_testText->setFont(pou::FontsHandler::loadAssetFromFile("../data/UASQUARE.TTF"));
    m_testText->setText("Poupou is da Pou");
    m_testText->setFontSize(24);
    m_testText->setSize(640, 110);
    this->addRootElement(m_testText);

    return (true);
}

/*void GameUi::cleanup()
{
    //m_mainInterface.reset();

}*/

/*void GameUi::update(pou::Time elapsed_time)
{
    this->update(elapsed_time);
}*/

void GameUi::updateCharacterLife(float cur, float max)
{
    m_lifeBar->setMinMaxValue(0,max);
    m_lifeBar->setValue(cur);

    //std::string text = "Poupou is da Pou: "+std::to_string((int)cur)+" / "+std::to_string((int)max);
    std::string text = "Lorem Ipsum \n is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy \n text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book. It has survived not only five centuries, but also the leap into electronic typesetting, remaining essentially unchanged. It was popularised in the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, and more recently with desktop publishing software like Aldus PageMaker including versions of Lorem Ipsum.";
    //m_testText->setFontSize((int)cur);
    m_testText->setText(text);
}

