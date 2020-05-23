#include "world/GameWorld.h"

#include "PouEngine/Types.h"
#include "PouEngine/assets/AssetHandler.h"
#include "PouEngine/assets/SpriteSheetAsset.h"

GameWorld::GameWorld(bool renderable) :
    m_scene(nullptr),
    m_isRenderable(renderable)
{
    //ctor
}

GameWorld::~GameWorld()
{
    this->destroy();
}


void GameWorld::update(const pou::Time elapsed_time)
{

}

void GameWorld::generate()
{
    this->destroy();

    m_scene = new pou::Scene();

    pou::SpriteSheetAsset *grassSheet = pou::SpriteSheetsHandler::loadAssetFromFile("../data/grasslands/grassXML.txt");

    for(auto x = -10 ; x < 10 ; x++)
    for(auto y = -10 ; y < 10 ; y++)
    {
        pou::SceneNode *grassNode = m_scene->getRootNode()->createChildNode(x*64,y*64);

        int rd = x+y;//glm::linearRand(0,96);
        int modulo = 4;
        if(abs(rd % modulo) == 0)
            grassNode->attachObject(m_scene->createSpriteEntity(grassSheet->getSpriteModel("grass1_2")));
        else if(abs(rd % modulo) == 1)
            grassNode->attachObject(m_scene->createSpriteEntity(grassSheet->getSpriteModel("grass1_1")));
        else if(abs(rd % modulo) == 2)
            grassNode->attachObject(m_scene->createSpriteEntity(grassSheet->getSpriteModel("grass1_3")));
        else if(abs(rd % modulo) == 3)
            grassNode->attachObject(m_scene->createSpriteEntity(grassSheet->getSpriteModel("grass1_4")));

         grassNode->setScale(glm::vec3(
                                glm::linearRand(0,10) > 5 ? 1 : -1,
                                1,//glm::linearRand(0,10) > 5 ? 1 : -1,
                                1));
    }
}

void GameWorld::destroy()
{
    if(m_scene)
        delete m_scene;
    m_scene = nullptr;
}

