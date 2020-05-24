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
const int    GameWorld::CHARACTERMODELSID_BITS  =12;
const int    GameWorld::CHARACTERSID_BITS       =16;

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

size_t GameWorld::syncElement(CharacterModelAsset *characterModel)
{
    return m_syncCharacterModels.allocateId(characterModel);
}

size_t GameWorld::syncElement(Character *character)
{
    this->syncElement((pou::SceneNode*)character);
    return m_syncCharacters.allocateId(character);
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

