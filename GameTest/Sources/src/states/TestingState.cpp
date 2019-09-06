#include "states/TestingState.h"

#include "PouEngine/core/StatesManager.h"
#include "PouEngine/utils/Clock.h"

#include "PouEngine/scene/SceneNode.h"
#include "PouEngine/assets/AssetHandler.h"
#include "PouEngine/assets/SpriteSheetAsset.h"
#include "PouEngine/assets/TextureAsset.h"

#include "PouEngine/Types.h"

#include <glm/gtc/random.hpp>

TestingState::TestingState() :
    m_firstEntering(true),
    m_scene(nullptr)
{
    m_nbrFps = 0;

    m_character = nullptr;
    m_character2 = nullptr;
    m_croco = nullptr;
}

TestingState::~TestingState()
{
    this->leaving();
}

void TestingState::init()
{
    m_firstEntering = false;

    m_camVelocity = glm::vec2(0,0);

    m_scene = new pou::Scene();

    pou::AssetLoadType loadType = pou::LoadType_InThread;

    pou::AssetTypeId tex[10];
    tex[0] = pou::TexturesHandler::loadAssetFromFile("../data/sand_color.png",loadType)->getId();
    tex[1] = pou::TexturesHandler::loadAssetFromFile("../data/sand_height.png",loadType)->getId();
    tex[2] = pou::TexturesHandler::loadAssetFromFile("../data/sand_normal.png",loadType)->getId();

    tex[3] = pou::TexturesHandler::loadAssetFromFile("../data/abbey_albedo.png",loadType)->getId();
    tex[4] = pou::TexturesHandler::loadAssetFromFile("../data/abbey_height.png",loadType)->getId();
    tex[5] = pou::TexturesHandler::loadAssetFromFile("../data/abbey_normal.png",loadType)->getId();

    tex[6] = pou::TexturesHandler::loadAssetFromFile("../data/tree_albedo.png",loadType)->getId();
    tex[7] = pou::TexturesHandler::loadAssetFromFile("../data/tree_height.png",loadType)->getId();
    tex[8] = pou::TexturesHandler::loadAssetFromFile("../data/tree_height.png",loadType)->getId();
    tex[9] = pou::TexturesHandler::loadAssetFromFile("../data/tree_rmt.png",loadType)->getId();


    pou::TextureAsset *charTexture2 = pou::TexturesHandler::loadAssetFromFile("../data/char1/char1.png",loadType);
    pou::TextureAsset *charTexture = pou::TexturesHandler::loadAssetFromFile("../data/char1/char1_scale.png",loadType);

    m_charModel = new pou::SpriteModel();
    m_charModel->setTexture(charTexture);
    m_charModel->setSize({128.0,128.0});
    m_charModel->setCenter({64,64});
    m_charModel->setTextureRect({0,0},{1,1});

    m_testNode = m_scene->getRootNode()->createChildNode({0,0});
    m_testChar =  m_scene->createSpriteEntity(m_charModel);
    m_testChar->setRotation(30.0);
    m_testChar->setOrdering(pou::ORDERED_BY_Z);
    //m_testNode->attachObject(m_testChar);

    m_character = new Character();
    m_character->loadModel("../data/char1/char1XML.txt");
    m_character->setPosition(0,0,1);
    m_scene->getRootNode()->addChildNode(m_character);

    m_character2 = new Character();
    m_character2->loadModel("../data/char1/char1XML.txt");
    m_character2->setPosition(-100,-100,1);
    m_scene->getRootNode()->addChildNode(m_character2);

    m_croco = new Character();
    m_croco->loadModel("../data/croco/crocoXML.txt");
    m_croco->setPosition(220,70,1);
    m_croco->setWalkingSpeed(100.0f);
    m_croco->setRotationRadius(100.0f);
    m_scene->getRootNode()->addChildNode(m_croco);



    pou::SpriteSheetAsset *grassSheet = pou::SpriteSheetsHandler::loadAssetFromFile("../data/grassXML.txt");

    for(auto x = -5 ; x < 5 ; x++)
    for(auto y = -5 ; y < 5 ; y++)
    {
         pou::SceneNode *grassNode = m_scene->getRootNode()->createChildNode(x*64,y*64);
         grassNode->attachObject(m_scene->createSpriteEntity(grassSheet->getSpriteModel("grass1")));
    }

    m_camera = m_scene->createCamera();
    //m_camera->setViewport({.2,.3},{.5,.4});
    m_cameraNode = m_scene->getRootNode()->createChildNode(0,0);
    ///m_cameraNode->attachObject(m_camera);
    m_character->attachObject(m_camera);

    //m_scene->setCurrentCamera(m_camera);


    m_scene->setAmbientLight({1.0,1.0,1.0,1.0});
    //m_sunLight = m_scene->createLightEntity(pou::LightType_Directional);
    //m_scene->getRootNode()->attachObject(m_sunLight);

    ///Day
   // m_sunLight->setDiffuseColor({1.0,1.0,1.0,1.0});
   // m_sunLight->setIntensity(20.0);

    ///Night
    //m_sunLight.setDiffuseColor({0.7,0.7,1.0,1.0});
    //m_sunLight.setIntensity(0.2);

    //sunLight->setType(pou::LightType_Directional);
    //m_sunLight.setDirection({-1.0,0.0,-1.0});
    /*m_sunLight->setDirection({.2 ,-1.0,-1.0});
    m_sunLight->setShadowMapExtent({1920,1080});
    m_sunLight->enableShadowCasting();*/

    /*pou::LightEntity* sunLight = m_scene->createLightEntity(pou::LightType_Directional);
    m_scene->getRootNode()->attachObject(sunLight);
    sunLight->setDirection({-1.0 ,1.0,-1.0});
    sunLight->setDiffuseColor({1.0,0.0,0.0,1.0});
    sunLight->setIntensity(15.0);
    sunLight->enableShadowCasting();*/

   /* pou::LightEntity *cursorLight = m_scene->createLightEntity();
    m_cursorLightNode = m_scene->getRootNode()->createChildNode(0,0,60);
    m_cursorLightNode->attachObject(cursorLight);
    cursorLight->setDiffuseColor({1.0,1.0,1.0,1.0});
    cursorLight->setIntensity(20.0);
    cursorLight->setRadius(400.0);
    cursorLight->setType(pou::LightType_Omni);*/

    /*for(size_t i = 3 ; i < 3 ; ++i)
    {
        pou::LightEntity *secLight = m_scene->createLightEntity();
        secLight->setDiffuseColor({glm::linearRand(0,100)/100.0,
                                    glm::linearRand(0,100)/100.0,
                                    glm::linearRand(0,100)/100.0});

        secLight->setIntensity(glm::linearRand(2.0,20.0));
        secLight->setRadius(glm::linearRand(100.0,600.0));
        m_cursorLightNode->createChildNode({glm::linearRand(-600,600),
                                            glm::linearRand(-600,600),
                                            glm::linearRand(-50,300)})->attachObject(secLight);
    }*/


  /*  for(size_t i = 0 ; i < 8 ; ++i)
    {
        //vec3(.4,0,.8)
        glm::vec3 v;
        v.x = glm::linearRand(-100,100)/100.0;
        v.y = glm::linearRand(-100,100)/100.0;
        v.z = glm::linearRand(0,100)/100.0;
        v = glm::normalize(v);
        v *= glm::linearRand(10,100)/100.0;

        std::ostringstream buf;
        buf<<"vec3("<<v.x<<","<<v.y<<","<<v.z<<"),";
        Logger::write(buf);

        v.x *= -1;
        v.y *= -1;

        std::ostringstream buf2;
        buf2<<"vec3("<<v.x<<","<<v.y<<","<<v.z<<"),";
        Logger::write(buf2);
    }*/
}

void TestingState::entered()
{
    m_totalTime = pou::TimeZero();

    if(m_firstEntering)
        this->init();
}

void TestingState::leaving()
{
    delete m_scene;
    m_scene = nullptr;

    delete m_charModel;
    m_charModel = nullptr;

    delete m_character;
    m_character = nullptr;

    delete m_character2;
    m_character2 = nullptr;

    delete m_croco;
    m_croco = nullptr;
}

void TestingState::revealed()
{

}

void TestingState::obscuring()
{

}

void TestingState::handleEvents(const EventsManager *eventsManager)
{
    if(eventsManager->keyReleased(GLFW_KEY_ESCAPE))
        m_manager->stop();

    if(eventsManager->isAskingToClose())
        m_manager->stop();

    if(m_scene == nullptr)
        return;

    glm::vec2 worldMousePos = m_scene->convertScreenToWorldCoord(eventsManager->centeredMousePosition(), m_camera);

    if(eventsManager->mouseButtonIsPressed(GLFW_MOUSE_BUTTON_1))
        m_croco->setDestination(worldMousePos);
    if(eventsManager->mouseButtonIsPressed(GLFW_MOUSE_BUTTON_2))
        m_character2->setDestination(worldMousePos);

    m_camVelocity = {0,0};

    if(eventsManager->keyIsPressed(GLFW_KEY_DOWN))
        m_camVelocity.y = 200.0;
    if(eventsManager->keyIsPressed(GLFW_KEY_UP))
        m_camVelocity.y = -200.0;
    if(eventsManager->keyIsPressed(GLFW_KEY_LEFT))
        m_camVelocity.x = -200.0;
    if(eventsManager->keyIsPressed(GLFW_KEY_RIGHT))
        m_camVelocity.x = 200.0;


    glm::vec2 charDirection = {0,0};

    if(eventsManager->keyIsPressed(GLFW_KEY_S))
        charDirection.y = 1;
    if(eventsManager->keyIsPressed(GLFW_KEY_W))
        charDirection.y = -1;
    if(eventsManager->keyIsPressed(GLFW_KEY_A))
        charDirection.x = -1;
    if(eventsManager->keyIsPressed(GLFW_KEY_D))
        charDirection.x = 1;

    m_character->walk(charDirection);

    if(eventsManager->keyIsPressed(GLFW_KEY_SPACE))
        m_croco->walk({0,-1});
    else
        m_croco->walk({0,0});

}

void TestingState::update(const pou::Time &elapsedTime)
{
    m_totalTime += elapsedTime;
    m_nbrFps++;

    glm::vec2 camMove = m_scene->convertScreenToWorldCoord(m_camVelocity);
    camMove.x *= elapsedTime.count();
    camMove.y *= elapsedTime.count();
    m_cameraNode->move(camMove);

    m_testChar->rotate(1.0*elapsedTime.count());

    m_scene->update(elapsedTime);

    if(m_totalTime.count() > 1)
    {
        m_totalTime -= std::chrono::seconds(1);
        std::cout<<"FPS : "<<m_nbrFps<<std::endl;
        m_nbrFps = 0;
    }
}

void TestingState::draw(pou::RenderWindow *renderWindow)
{
    if(renderWindow->getRenderer(pou::Renderer_Scene) != nullptr)
    {
        pou::SceneRenderer *renderer = dynamic_cast<pou::SceneRenderer*>(renderWindow->getRenderer(pou::Renderer_Scene));

        m_scene->render(renderer, m_camera);
    }
}



