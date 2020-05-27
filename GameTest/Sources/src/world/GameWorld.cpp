#include "world/GameWorld.h"

#include "PouEngine/Types.h"
#include "PouEngine/assets/AssetHandler.h"
#include "PouEngine/assets/SpriteSheetAsset.h"
#include "PouEngine/renderers/SceneRenderer.h"

const int GameWorld::MAX_NBR_PLAYERS = 4;

const int    GameWorld::NODEID_BITS             =16;
const int    GameWorld::SPRITESHEETID_BITS      =10;
const int    GameWorld::SPRITEENTITYID_BITS     =16;
const int    GameWorld::CHARACTERMODELSID_BITS  =12;
const int    GameWorld::CHARACTERSID_BITS       =16;

GameWorld::GameWorld(bool renderable, bool isServer) :
    m_scene(nullptr),
    m_isRenderable(renderable),
    m_isServer(isServer),
    m_curLocalTime(0),
    m_lastSyncTime(-1),
    m_camera(nullptr)
{
    m_syncNodes.setMax(pow(2,GameWorld::NODEID_BITS));
    m_syncSpriteSheets.setMax(pow(2,GameWorld::SPRITESHEETID_BITS));
    m_syncSpriteEntities.setMax(pow(2,GameWorld::SPRITEENTITYID_BITS));
    m_syncCharacterModels.setMax(pow(2,GameWorld::CHARACTERMODELSID_BITS));
    m_syncCharacters.setMax(pow(2,GameWorld::CHARACTERSID_BITS));

    m_syncPlayers.setMax(GameWorld::MAX_NBR_PLAYERS);
    m_syncPlayers.enableIdReuse();
}

GameWorld::~GameWorld()
{
    this->destroy();
}


void GameWorld::update(const pou::Time elapsed_time)
{
    //if(m_isServer)
        m_curLocalTime += elapsed_time.count();

    if(!m_scene)
        return;

    while(!m_addedPlayersList.empty())
    {
        this->initPlayer(m_addedPlayersList.back());
        m_addedPlayersList.pop_back();
    }

    while(!m_removedPlayersList.empty())
    {
        this->removePlayer(m_removedPlayersList.back());
        m_removedPlayersList.pop_back();
    }

    this->updateSunLight(elapsed_time);
    m_scene->update(elapsed_time, m_curLocalTime);
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

size_t GameWorld::askToAddPlayer()
{
    auto player = new PlayableCharacter();

    auto player_id = this->syncElement(player);
    if(player_id == 0)
    {
        delete player;
        return (0);
    }
    m_addedPlayersList.push_back(player_id);
    return player_id;
}

bool GameWorld::askToRemovePlayer(size_t player_id)
{
    auto player = m_syncPlayers.findElement(player_id);
    if(!player)
        return (false);
    m_removedPlayersList.push_back(player_id);
    return (true);
}

bool GameWorld::isPlayerCreated(size_t player_id)
{
    auto player = m_syncPlayers.findElement(player_id);
    if(!player)
        return (false);
    return (player->getLastUpdateTime() != -1);
}

void GameWorld::playerWalk(int player_id, glm::vec2 direction, float localTime)
{
    auto player = m_syncPlayers.findElement(player_id);

    if(player == nullptr)
        return;

    player->askToWalk(direction);
}


float GameWorld::getLocalTime()
{
    return m_curLocalTime;
}

float GameWorld::getLastSyncTime()
{
    return m_lastSyncTime;
}

/// Protected


void GameWorld::createScene()
{
    this->destroy();
    m_scene = new pou::Scene();

    if(m_isRenderable)
    {
        m_sunLight = m_scene->createLightEntity(pou::LightType_Directional);
        m_sunLight->enableShadowCasting();
        m_sunLight->setShadowMapExtent({1024,1024});
        m_sunLight->setShadowBlurRadius(10);
    }
}

void GameWorld::createPlayerCamera(size_t player_id)
{
    if(m_isRenderable && !m_camera)
    {
        auto player = m_syncPlayers.findElement(player_id);

        m_camera = m_scene->createCamera();
        auto listeningCamera = m_scene->createCamera();
        listeningCamera -> setListening(true);
        auto *cameraNode = player->createChildNode();
        cameraNode->attachObject(m_camera);
        cameraNode = cameraNode->createChildNode(0,0,250);
        cameraNode->attachObject(listeningCamera);
    }
}

size_t GameWorld::syncElement(pou::SceneNode *node)
{
    return m_syncNodes.allocateId(node);
}

size_t GameWorld::syncElement(pou::SpriteSheetAsset *spriteSheet)
{
    auto id = m_syncSpriteSheets.allocateId(spriteSheet);
    m_syncTimeSpriteSheets.insert({m_curLocalTime, id});
    return id;
}

size_t GameWorld::syncElement(pou::SpriteEntity *spriteEntity)
{
    return m_syncSpriteEntities.allocateId(spriteEntity);
}

size_t GameWorld::syncElement(CharacterModelAsset *characterModel)
{
    auto id = m_syncCharacterModels.allocateId(characterModel);
    m_syncTimeCharacterModels.insert({m_curLocalTime,id});
    return id;
}

size_t GameWorld::syncElement(Character *character)
{
    this->syncElement((pou::SceneNode*)character);
    return m_syncCharacters.allocateId(character);
}

size_t GameWorld::syncElement(PlayableCharacter *player)
{
    this->syncElement((Character*)player);
    return m_syncPlayers.allocateId(player);
}


void GameWorld::desyncElement(pou::SceneNode *node, bool noDesyncInsert)
{
    if(!noDesyncInsert)
    {
        auto id = m_syncNodes.findId(node);
        m_desyncNodes.insert({m_curLocalTime,id});
    }
    m_syncNodes.freeElement(node);
}

void GameWorld::desyncElement(Character *character, bool noDesyncInsert)
{
    this->desyncElement((pou::SceneNode*)character, true);

    if(!noDesyncInsert)
    {
        auto id = m_syncCharacters.findId(character);
        m_desyncCharacters.insert({m_curLocalTime,id});
    }
    m_syncCharacters.freeElement(character);
}

void GameWorld::desyncElement(PlayableCharacter *player, bool noDesyncInsert)
{
    this->desyncElement((Character*)player, true);

    if(!noDesyncInsert)
    {
        auto id = m_syncPlayers.findId(player);
        m_desyncPlayers.insert({m_curLocalTime,id});
    }
    m_syncPlayers.freeElement(player);
}

void GameWorld::updateSunLight(const pou::Time elapsed_time)
{
    m_dayTime = m_dayTime + elapsed_time.count();
    if(m_dayTime >= 360)
        m_dayTime -= 360;

    if(!m_isRenderable)
        return;

    m_sunLight->setDirection({glm::cos(m_dayTime*glm::pi<float>()/180.0f)*.5,
                              glm::sin(m_dayTime*glm::pi<float>()/180.0f)*.5,-1.0});

    pou::Color dayColor = {1.0,1.0,1.0,1.0},
               nightColor = {.2,.2,1.0,1.0},
               sunsetColor = {1.0,.6,0.0,1.0};

    float dayIntensity = 3.0,
          sunsetIntensity = 2.0,
          nightIntensity = 0.5;

    pou::Color sunColor;
    float sunIntensity;

    if(m_dayTime >= 0 && m_dayTime < 30)
    {
        sunColor = glm::mix(sunsetColor, nightColor, m_dayTime/30.0f);
        sunIntensity = glm::mix(sunsetIntensity, nightIntensity, m_dayTime/30.0f);
    }
    else if(m_dayTime >= 30 && m_dayTime <= 150)
    {
        sunColor = nightColor;
        sunIntensity = nightIntensity;
    }
    else if(m_dayTime > 150 && m_dayTime <= 180)
    {
        sunColor =  glm::mix(nightColor, sunsetColor, (m_dayTime-150)/30.0f);
        sunIntensity =  glm::mix(nightIntensity, sunsetIntensity, (m_dayTime-150)/30.0f);
    }
    else if(m_dayTime > 180 && m_dayTime < 210)
    {
        sunColor =  glm::mix(sunsetColor, dayColor, (m_dayTime-180)/30.0f);
        sunIntensity =  glm::mix(sunsetIntensity, dayIntensity, (m_dayTime-180)/30.0f);
    }
    else if(m_dayTime >= 210 && m_dayTime <= 330)
    {
        sunColor = dayColor;
        sunIntensity = dayIntensity;
    }
    else
    {
        sunColor = glm::mix(dayColor,sunsetColor, (m_dayTime-330)/30.0f);
        sunIntensity = glm::mix(dayIntensity,sunsetIntensity, (m_dayTime-330)/30.0f);
    }
    m_scene->setAmbientLight(sunColor * glm::vec4(1.0,1.0,1.0,.75));
    m_sunLight->setDiffuseColor(sunColor);
    m_sunLight->setIntensity(sunIntensity);
}


