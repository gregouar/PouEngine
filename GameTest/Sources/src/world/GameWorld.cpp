#include "world/GameWorld.h"

#include "PouEngine/Types.h"
#include "PouEngine/assets/AssetHandler.h"
#include "PouEngine/assets/SpriteSheetAsset.h"
#include "PouEngine/renderers/SceneRenderer.h"

#include "net/GameServer.h"
#include "net/GameClient.h"

const int GameWorld::MAX_NBR_PLAYERS = 4;

const int    GameWorld::NODEID_BITS             =16;
const int    GameWorld::SPRITESHEETID_BITS      =10;
const int    GameWorld::SPRITEENTITYID_BITS     =16;
const int    GameWorld::CHARACTERMODELSID_BITS  =12;
const int    GameWorld::CHARACTERSID_BITS       =16;
const int    GameWorld::ITEMMODELSID_BITS  =12;

GameWorld::GameWorld(bool renderable, bool isServer) :
    m_scene(nullptr),
    m_isRenderable(renderable),
    m_isServer(isServer),
    m_curLocalTime(0),
    m_lastSyncTime(-1),
    m_camera(nullptr)
   /// m_wantedRewind(-1)
{
    m_syncNodes.setMax(pow(2,GameWorld::NODEID_BITS));
    m_syncSpriteSheets.setMax(pow(2,GameWorld::SPRITESHEETID_BITS));
    m_syncSpriteEntities.setMax(pow(2,GameWorld::SPRITEENTITYID_BITS));
    m_syncCharacterModels.setMax(pow(2,GameWorld::CHARACTERMODELSID_BITS));
    m_syncCharacters.setMax(pow(2,GameWorld::CHARACTERSID_BITS));
    m_syncItemModels.setMax(pow(2,GameWorld::ITEMMODELSID_BITS));

    m_syncPlayers.setMax(GameWorld::MAX_NBR_PLAYERS);
    m_syncPlayers.enableIdReuse();
}

GameWorld::~GameWorld()
{
    this->destroy();
}


void GameWorld::update(const pou::Time elapsed_time/*, bool isRewinding*/)
{

    /**this->processPlayerActions(elapsed_time);
    //if(m_isServer)
        m_curLocalTime += elapsed_time.count();**/

    //m_curLocalTime += elapsed_time.count();
   /** if(!isRewinding)
    {
        m_syncTime = m_curLocalTime;  //We want to update the updateTime of syncedAtt with the localTime before rewinding !
        if(m_wantedRewind != (uint32_t)(-1))
        {
            //std::cout<<"Rewind to:"<<m_wantedRewind<<std::endl;
            uint32_t wantedRewind = m_wantedRewind;
            m_wantedRewind = (uint32_t)(-1);
            this->rewind(wantedRewind);
        }
    }**/

    this->processPlayerActions();

    m_curLocalTime++;

    //std::cout<<"LocalTime:"<<m_curLocalTime<<std::endl;

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
    m_scene->update(elapsed_time, m_curLocalTime /**m_syncTime**/);


    uint32_t cleanTime = m_curLocalTime - GameClient::MAX_PLAYER_REWIND;///GameServer::MAX_REWIND_AMOUNT;
    auto cleanPlayerActionsIt = m_playerActions.lower_bound(cleanTime);
    if(cleanPlayerActionsIt != m_playerActions.begin())
        (--cleanPlayerActionsIt);

    if(cleanPlayerActionsIt != m_playerActions.end())
        m_playerActions.erase(m_playerActions.begin(), cleanPlayerActionsIt);

   // while(!m_playerActions.empty()
    //&& m_playerActions.begin()->first < ))
      //  m_playerActions.erase(m_playerActions.begin());
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

/**void GameWorld::rewind(uint32_t time, bool simulate)
{
    if(!m_scene)
        return;

    std::cout<<"Rewind from"<<time<<std::endl;

    if(time < m_curLocalTime - pou::NetEngine::getMaxRewindAmount())
        time = m_curLocalTime - pou::NetEngine::getMaxRewindAmount();

    uint32_t curTime = m_curLocalTime;
    m_scene->rewind(time);
    m_curLocalTime = time;

    if(simulate)
        while(m_curLocalTime < curTime)
            this->update(GameServer::TICKDELAY, true);
    std::cout<<"End Rewind at "<<m_curLocalTime<<std::endl;
}**/

size_t GameWorld::askToAddPlayer(bool isLocalPlayer)
{
    auto player = new Player(isLocalPlayer);
    auto player_id = this->syncElement(player);

    if(player_id == 0)
    {
        delete player;
        return (0);
    }

    if(isLocalPlayer)
    {
        this->createPlayerCamera(player_id);

        ///Move this to world generation something something
        auto music = pou::AudioEngine::createEvent("event:/Music");
        pou::AudioEngine::playEvent(music);
    } else {
        player->disableDamageDealing();
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
    return (player->getLastPlayerUpdateTime() != (uint32_t)(-1));
}

void GameWorld::addPlayerAction(int player_id, const PlayerAction &playerAction, uint32_t clientTime)
{
    if(clientTime == (uint32_t)(-1))
        clientTime = m_curLocalTime;

    /*auto boundaries = m_playerActions.equal_range(clientTime);
    for(auto it = boundaries.first ; it != boundaries.second ; ++it)
        if((int)it->second.first == player_id && it->second.second.actionType == playerAction.actionType)
            it = m_playerActions.erase(it);*/

    PlayerAction action = playerAction;

    if(action.direction != glm::vec2(0))
        action.direction = normalize(action.direction);

    m_playerActions.insert({clientTime, {player_id,action}});

    /**if(pou::NetEngine::getMaxRewindAmount() == 0)
        return;

    if(clientTime < m_curLocalTime)
    if(m_wantedRewind > clientTime - 1 || m_wantedRewind == (uint32_t)(-1))
        m_wantedRewind = clientTime - 1;**/
}

void GameWorld::removeAllPlayerActions(int player_id, uint32_t time)
{
    if(time == (uint32_t)(-1))
        time = m_curLocalTime;

    auto actionIt = m_playerActions.lower_bound(time);
    while(actionIt != m_playerActions.end())
    {
        if((int)actionIt->second.first == player_id)
            actionIt = m_playerActions.erase(actionIt);
        if(actionIt != m_playerActions.end())
            ++actionIt;
    }
}

/*void GameWorld::playerWalk(int player_id, glm::vec2 direction, float localTime)
{
    auto player = m_syncPlayers.findElement(player_id);

    if(player == nullptr)
        return;


    ///Replace by add walking action with time or something ?
    player->askToWalk(direction);

    if(localTime != -1)
        this->rewind(localTime);
}*/

glm::vec2 GameWorld::convertScreenToWorldCoord(glm::vec2 p)
{
    if(!m_scene || !m_camera)
        return glm::vec2(0);
    return m_scene->convertScreenToWorldCoord(p, m_camera);
}


uint32_t GameWorld::getLocalTime()
{
    return m_curLocalTime;
}

uint32_t GameWorld::getLastSyncTime()
{
    return m_lastSyncTime;
}

Player *GameWorld::getPlayer(int player_id)
{
    return m_syncPlayers.findElement(player_id);
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

        m_scene->getRootNode()->attachObject(m_sunLight);
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
    int id = m_syncCharacters.allocateId(character);
    character->setSyncId(id);
    return id;
}

size_t GameWorld::syncElement(ItemModelAsset *itemModel)
{
    auto id = m_syncItemModels.allocateId(itemModel);
    m_syncTimeItemModels.insert({m_curLocalTime,id});
    return id;
}

size_t GameWorld::syncElement(Player *player)
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

void GameWorld::desyncElement(Player *player, bool noDesyncInsert)
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
    ///m_dayTime = m_dayTime + elapsed_time.count();
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

void GameWorld::processPlayerActions()
{
    auto boundaries = m_playerActions.equal_range(m_curLocalTime);

    for(auto it = boundaries.first ; it != boundaries.second ; ++it)
    {
        auto player_id = it->second.first;
        auto player = m_syncPlayers.findElement(player_id);
        if(player == nullptr)
        {
           // ++it;
            continue;
        }

        auto& playerAction = it->second.second;
        player->processAction(playerAction);
    }
}

