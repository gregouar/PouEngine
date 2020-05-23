#include "ui/GameUi.h"

#include "PouEngine/Types.h"
#include "PouEngine/assets/AssetHandler.h"
#include "PouEngine/assets/TextureAsset.h"

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

    return (true);
}

/*void GameUi::cleanup()
{
    //m_mainInterface.release();

}*/

/*void GameUi::update(pou::Time elapsed_time)
{
    this->update(elapsed_time);
}*/

void GameUi::updateCharacterLife(float cur, float max)
{
    m_lifeBar->setMinMaxValue(0,max);
    m_lifeBar->setValue(cur);
}

