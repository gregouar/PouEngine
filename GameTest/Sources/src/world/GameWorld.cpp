#include "world/GameWorld.h"

#include "PouEngine/Types.h"
#include "PouEngine/assets/AssetHandler.h"
#include "PouEngine/assets/SpriteSheetAsset.h"
#include "PouEngine/renderers/SceneRenderer.h"

const int GameWorld::MAX_NBR_PLAYERS = 4;
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
    m_isRenderable(renderable),
    m_curLocalTime(0)
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
    m_curLocalTime += elapsed_time.count();

    if(!m_scene)
        return;

    this->updateSunLight(elapsed_time);
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

        m_sunLight = m_scene->createLightEntity(pou::LightType_Directional);
        m_sunLight->enableShadowCasting();
        m_sunLight->setShadowMapExtent({1024,1024});
        m_sunLight->setShadowBlurRadius(10);
    }
}

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

size_t GameWorld::syncElement(PlayableCharacter *player)
{
    this->syncElement((Character*)player);
    return m_syncPlayers.allocateId(player);
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


