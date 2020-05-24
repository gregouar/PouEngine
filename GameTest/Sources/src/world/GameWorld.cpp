#include "world/GameWorld.h"

#include "PouEngine/Types.h"
#include "PouEngine/assets/AssetHandler.h"
#include "PouEngine/assets/SpriteSheetAsset.h"
#include "PouEngine/renderers/SceneRenderer.h"

const glm::vec3 GameWorld::GAMEWORLD_MAX_SIZE   = glm::vec3(50000.0, 50000.0, 1000.0);

const float     GameWorld::NODE_MAX_SCALE       = 100.0f;
const uint8_t   GameWorld::NODE_SCALE_DECIMALS  = 2;



const int    GameWorld::NODEID_BITS             =16;
const int    GameWorld::SPRITESHEETID_BITS      =10;
const int    GameWorld::SPRITEENTITYID_BITS     =16;

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
    if(m_scene)
        m_scene->update(elapsed_time);
}

void GameWorld::render(pou::RenderWindow *renderWindow)
{
    if(!m_isRenderable)
        return;

    if(!m_scene)
        return;

    if(renderWindow->getRenderer(pou::Renderer_Scene) != nullptr)
    {
        auto renderer = dynamic_cast<pou::SceneRenderer*>(renderWindow->getRenderer(pou::Renderer_Scene));
        m_scene->render(renderer, m_camera);
    }
}

void GameWorld::generate()
{
    this->createScene();

    //pou::SpriteSheetAsset *grassSheet = pou::SpriteSheetsHandler::loadAssetFromFile("../data/grasslands/grassXML.txt");
    auto *grassSheet = pou::SpriteSheetsHandler::loadAssetFromFile("../data/grasslands/grassXML.txt");
    //auto grassSheet = this->syncSpriteSheet("../data/grasslands/grassXML.txt").second;
    this->syncElement(grassSheet);

    for(auto x = -10 ; x < 10 ; x++)
    for(auto y = -10 ; y < 10 ; y++)
    {
       /* auto grassNode = this->createNode(nullptr, true);
        grassNode->setPosition(x*64,y*64);*/

        pou::SceneNode *grassNode = m_scene->getRootNode()->createChildNode(x*64,y*64);
        this->syncElement(grassNode);

        pou::SpriteEntity *spriteEntity;

        int rd = x+y;//glm::linearRand(0,96);
        int modulo = 4;
        if(abs(rd % modulo) == 0)
            spriteEntity = m_scene->createSpriteEntity(grassSheet->getSpriteModel("grass1_2"));
           // grassNode->attachObject(m_scene->createSpriteEntity(grassSheet->getSpriteModel("grass1_2")));
        else if(abs(rd % modulo) == 1)
            spriteEntity = m_scene->createSpriteEntity(grassSheet->getSpriteModel("grass1_1"));
            //grassNode->attachObject(m_scene->createSpriteEntity(grassSheet->getSpriteModel("grass1_1")));
        else if(abs(rd % modulo) == 2)
            spriteEntity = m_scene->createSpriteEntity(grassSheet->getSpriteModel("grass1_3"));
            //grassNode->attachObject(m_scene->createSpriteEntity(grassSheet->getSpriteModel("grass1_3")));
        else if(abs(rd % modulo) == 3)
            spriteEntity = m_scene->createSpriteEntity(grassSheet->getSpriteModel("grass1_4"));
            //grassNode->attachObject(m_scene->createSpriteEntity(grassSheet->getSpriteModel("grass1_4")));

        grassNode->attachObject(spriteEntity);
        this->syncElement(spriteEntity);

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


void GameWorld::createWorldInitializationMsg(std::shared_ptr<NetMessage_WorldInitialization> worldInitMsg)
{
    worldInitMsg->nbr_nodes = m_syncNodes.size();
    worldInitMsg->nodes.resize(worldInitMsg->nbr_nodes);
    size_t i = 0;
    for(auto it = m_syncNodes.begin() ; it != m_syncNodes.end() ; ++it, ++i)
    {
        //auto& [ nodeId, parentNodeId, nodePtr ] = worldInitMsg->nodes[i];

        auto &[ nodeId, nodeSync ] = worldInitMsg->nodes[i];

        nodeId = it->first;

        if(it->second->getParent() != m_scene->getRootNode())
            nodeSync.parentNodeId = m_syncNodes.findId((pou::SceneNode*)it->second->getParent());
        else
            nodeSync.parentNodeId = 0;

        nodeSync.node = it->second;
    }

    worldInitMsg->nbr_spriteSheets = m_syncSpriteSheets.size();
    worldInitMsg->spriteSheets.resize(worldInitMsg->nbr_nodes);
    i = 0;
    for(auto it = m_syncSpriteSheets.begin() ; it != m_syncSpriteSheets.end() ; ++it, ++i)
    {
        auto &[ spriteSheetId, spriteSheetPath ] = worldInitMsg->spriteSheets[i];
        spriteSheetId = it->first;

        spriteSheetPath = it->second->getFilePath();
    }

    worldInitMsg->nbr_spriteEntities = m_syncSpriteEntities.size();
    worldInitMsg->spriteEntities.resize(worldInitMsg->nbr_spriteEntities);
    i = 0;
    for(auto it = m_syncSpriteEntities.begin() ; it != m_syncSpriteEntities.end() ; ++it, ++i)
    {
        auto &[ spriteEntityId, spriteEntitySync ] = worldInitMsg->spriteEntities[i];
        spriteEntityId = it->first;

        auto spriteEntity = it->second;

        auto spriteModel = spriteEntity->getSpriteModel();
        if(spriteModel == nullptr)
            continue;

        auto spriteSheet = spriteModel->getSpriteSheet();
        if(spriteSheet == nullptr)
            continue;

        size_t spriteSheetId = m_syncSpriteSheets.findId(spriteSheet);

        size_t nodeId = 0;

        auto parentNode = spriteEntity->getParentNode();
        if(parentNode != nullptr)
            nodeId = m_syncNodes.findId(parentNode);

        spriteEntitySync.spriteSheetId = spriteSheetId;
        spriteEntitySync.spriteId = spriteModel->getSpriteId();
        spriteEntitySync.nodeId = nodeId;
    }
}

void GameWorld::generate(std::shared_ptr<NetMessage_WorldInitialization> worldInitMsg)
{
    this->createScene();

    for(auto &node : worldInitMsg->nodes)
    {
        auto& [ nodeId, nodeSync ] = node;

        auto nodePtr = m_scene->getRootNode()->createChildNode();
        m_syncNodes.insert(nodeId, nodePtr);

        nodePtr->setPosition(nodeSync.node->getPosition());
        nodePtr->setRotation(nodeSync.node->getEulerRotation());
        nodePtr->setScale(nodeSync.node->getScale());
    }

    for(auto &node : worldInitMsg->nodes)
    {
        auto& [ nodeId, nodeSync ] = node;

        if(nodeSync.parentNodeId != 0)
        {
            auto nodePtr = m_syncNodes.findElement(nodeId);
            auto parentptr = m_syncNodes.findElement(nodeSync.parentNodeId);
            if(parentptr)
                parentptr->addChildNode(nodePtr);
        }
    }

    for(auto &spriteSheetIt : worldInitMsg->spriteSheets)
    {
        auto& [ spriteSheetId, spriteSheetPath ] = spriteSheetIt;
        auto *spriteSheetPtr = pou::SpriteSheetsHandler::loadAssetFromFile(spriteSheetPath);
        m_syncSpriteSheets.insert(spriteSheetId, spriteSheetPtr);
    }


    for(auto &spriteEntityIt : worldInitMsg->spriteEntities)
    {
        auto& [ spriteEntityId, spriteEntitySync ] = spriteEntityIt;

        /*m_syncSpriteEntities.insert(spriteEntityId, spriteEntitySync );*/

        auto *spriteSheetPtr = m_syncSpriteSheets.findElement(spriteEntitySync.spriteSheetId);
        if(spriteSheetPtr == nullptr)
            continue;

        auto *spriteEntityModel = spriteSheetPtr->getSpriteModel(spriteEntitySync.spriteId);
        auto *spriteEntity = m_scene->createSpriteEntity(spriteEntityModel);

        //if(spriteEntitySync.nodeId == 0)
            //m_scene->getRootNode()->attachObject(spriteEntity);
        //else
        if(spriteEntitySync.nodeId != 0)
        {
            auto *node = m_syncNodes.findElement(spriteEntitySync.nodeId);
            if(node == nullptr)
                continue;
            node->attachObject(spriteEntity);
        }

        m_syncSpriteEntities.insert(spriteEntityId, spriteEntity);
    }

}

/// Protected


void GameWorld::createScene()
{
    this->destroy();
    m_scene = new pou::Scene();

    if(m_isRenderable)
    {
        m_camera = m_scene->createCamera();
        auto listeningCamera = m_scene->createCamera();
        listeningCamera -> setListening(true);
        auto *cameraNode = m_scene->getRootNode()->createChildNode(0,0,0);
        cameraNode->attachObject(m_camera);
        cameraNode = cameraNode->createChildNode(0,0,250);
        cameraNode->attachObject(listeningCamera);
    }
}

/*size_t GameWorld::addSyncNode(pou::SceneNode *node)
{
    return m_syncNodes.allocateId(node);
}*/

/*size_t GameWorld::addSyncEntity(pou::SceneEntity *entity)
{
    return m_syncEntities.allocateId(entity);
}*/


/*pou::SceneNode* GameWorld::createNode(pou::SceneNode* parentNode, bool sync)
{
    if(parentNode == nullptr)
        parentNode = m_scene->getRootNode();

    auto node = parentNode->createChildNode();
    if(sync)
        m_syncNodes.allocateId(node);
        //this->addSyncNode(node);

    return node;
}*/

size_t GameWorld::syncElement(pou::SceneNode *node)
{
    return m_syncNodes.allocateId(node);
}

size_t GameWorld::syncElement(pou::SpriteSheetAsset *spriteSheet)
{
    return m_syncSpriteSheets.allocateId(spriteSheet);
}

size_t GameWorld::syncElement(pou::SpriteEntity *spriteEntity)
{
    return m_syncSpriteEntities.allocateId(spriteEntity);
}

/**size_t GameWorld::syncNode(pou::SceneNode *node)
{
    size_t parentNodeId = 0;
    if(node->getParent() != m_scene->getRootNode())
        parentNodeId = m_syncNodes.findId((pou::SceneNode*)node->getParent());

    return this->syncNode(node, parentNodeId);
}

size_t GameWorld::syncNode(pou::SceneNode *node, size_t parentNodeId)
{
    NodeSync nodeSync;
    nodeSync.node = node;
    nodeSync.parentNodeId = parentNodeId;

    return m_syncNodes.allocateId(nodeSync);
}

size_t GameWorld::syncSpriteSheet(pou::SpriteSheetAsset *spriteSheet)
{
    return m_syncSpriteSheets.allocateId(spriteSheet);
}

size_t GameWorld::syncSpriteEntity(pou::SpriteEntity *spriteEntity)
{
    auto spriteModel = spriteEntity->getSpriteModel();
    if(spriteModel == nullptr)
        return 0;

    auto spriteSheet = spriteModel->getSpriteSheet();
    if(spriteSheet == nullptr)
        return 0;

    size_t spriteSheetId = m_syncSpriteSheets.findId(spriteSheet);
    return this->syncSpriteEntity(spriteEntity, spriteSheetId);
}

size_t GameWorld::syncSpriteEntity(pou::SpriteEntity *spriteEntity, size_t spriteSheetId)
{
    size_t nodeId = 0;

    auto parentNode = spriteEntity->getParentNode();
    if(parentNode != nullptr)
        nodeId = m_syncNodes.findId(parentNode);

    return this->syncSpriteEntity(spriteEntity, spriteSheetId, nodeId);
}

size_t GameWorld::syncSpriteEntity(pou::SpriteEntity *spriteEntity, size_t spriteSheetId, size_t nodeId)
{
    auto spriteModel = spriteEntity->getSpriteModel();
    if(spriteModel == nullptr)
        return 0;

    SpriteEntitySync spriteEntitySync;
    spriteEntitySync.nodeId         = nodeId;
    spriteEntitySync.spriteId       = spriteModel->getSpriteId();
    spriteEntitySync.spriteSheetId  = spriteSheetId;

    return m_syncSpriteEntities.allocateId(spriteSheet);
}**/


/*std::pair<size_t, pou::SpriteSheetAsset*> GameWorld::syncSpriteSheet(const std::string &path)
{
    pou::SpriteSheetAsset *spriteSheet = pou::SpriteSheetsHandler::loadAssetFromFile(path);
    size_t id = m_syncSpriteSheets.allocateId(spriteSheet);
    return {id, spriteSheet};
}*/

/*std::pair<size_t, SpriteEntitySync> GameWorld::createSpriteEntitySync(size_t spriteSheetId, const std::string &spriteName)
{
    auto spriteSheet = m_syncSpriteSheets.findElement(spriteName);
    return this->createSpriteEntitySync({spriteSheetId, spriteSheet}, spriteName);
}


std::pair<size_t, SpriteEntitySync> GameWorld::createSpriteEntitySync(
            std::pair<size_t, pou::SpriteSheetAsset*> spriteSheetPair, const std::string &spriteName)
{
    size_t spriteId = spriteSheetPair.second->getSpriteId(spriteName);
    size_t spriteEntityId = this->allocateId;

    SpriteEntitySync spriteEntitySync;
    spriteEntitySync.spriteSheetId = spriteSheetPair.first
    spriteEntitySync.spriteId = spriteId.first
    spriteEntitySync.nodeId = nodeId.first

    return {spriteEntityId, }
}*/

/*pou::SpriteEntity* GameWorld::createSpriteEntity()
{
    return (nullptr);
}*/

