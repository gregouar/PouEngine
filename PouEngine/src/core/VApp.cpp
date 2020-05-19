#include "PouEngine/core/VApp.h"

#include "PouEngine/utils/Parser.h"
#include "PouEngine/utils/Logger.h"
#include "PouEngine/utils/Clock.h"
#include "PouEngine/core/Config.h"

#include "PouEngine/assets/AssetHandler.h"
#include "PouEngine/assets/TextureAsset.h"
#include "PouEngine/vulkanImpl/VBuffersAllocator.h"

#include "PouEngine/renderers/SceneRenderer.h"
#include "PouEngine/renderers/UiRenderer.h"

#include "PouEngine/audio/AudioEngine.h"
#include "PouEngine/audio/FMODAudioImpl.h"

#include "PouEngine/net/NetEngine.h"
#include "PouEngine/net/UdpNetImpl.h"

#include "PouEngine/utils/Profiler.h"

namespace pou
{

const char *VApp::APP_VERSION = "0.0.1";

const char *VApp::DEFAULT_APP_NAME = "VALAGEngine";
const char *VApp::DEFAULT_CONFIG_FILE = "config.ini";
const char *VApp::DEFAULT_SCREENSHOTPATH = "../screenshots/";
const char *VApp::DEFAULT_SHADERPATH = "../shaders/";

const char *VApp::DEFAULT_WINDOW_FULLSCREEN = "false";
const char *VApp::DEFAULT_WINDOW_WIDTH = "1024";
const char *VApp::DEFAULT_WINDOW_HEIGHT = "768";
const char *VApp::DEFAULT_VSYNC = "false";
const char *VApp::DEFAULT_ANISOTROPIC = "16";


const bool VApp::ENABLE_PROFILER = false;
const bool VApp::ENABLE_RANDOMNESS = false;

/** I should replace that by config double/triple buffering at some point **/
const size_t VApp::MAX_FRAMES_IN_FLIGHT = 2;

VApp::VApp(const VAppCreateInfos &infos) :
    m_createInfos(infos),
    m_sceenshotNbr(1)
{

}

VApp::~VApp()
{
    this->cleanup();
}


void VApp::run(GameState *startingState)
{
    m_running = true;

 //   Profiler::resetLoop(ENABLE_PROFILER);

    if(!this->init())
        throw std::runtime_error("Could not initialize application");

    m_statesManager.attachApp(this);
    m_statesManager.switchState(startingState);

    Logger::write("Starting application");

    this->loop();
}

void VApp::stop()
{
    Logger::write("Stopping application");

    m_running = false;
}

void VApp::printscreen()
{

}


bool VApp::init()
{
    Config::instance()->load(DEFAULT_CONFIG_FILE);

    if(VApp::ENABLE_RANDOMNESS)
        std::srand(std::time(nullptr));

    Logger::write("Initializing application");

    glfwInit();

    Profiler::pushClock("Create window");
    if(!this->createWindow())
        throw std::runtime_error("Cannot create window");
    Profiler::popClock();

    Profiler::pushClock("Init vulkan instance");
    VInstance::instance()->init(m_renderWindow.getSurface()); //Throw error
    Profiler::popClock();

    if(!m_renderWindow.init())
        throw std::runtime_error("Cannot initialize window");

    VTexturesManager::instance()->init(m_renderWindow.getFramesCount(),
                                       m_renderWindow.getSwapchainSize());

    m_eventsManager.init(m_renderWindow.getWindowPtr());

    Profiler::pushClock("Create renderers");
    if(!this->createRenderers())
        throw std::runtime_error("Cannot create renderers");
    Profiler::popClock();


    Profiler::pushClock("Init audio engine");
    if(!AudioEngine::instance()->init(std::make_unique<FMODAudioImpl> ()))
        throw std::runtime_error("Cannot initialize audio engine");


    Profiler::pushClock("Init net engine");
    if(!NetEngine::init(std::make_unique<UdpNetImpl> ()))
        throw std::runtime_error("Cannot initialize net engine");

    AudioEngine::set3DSettings(1.0f,100.0f,1.0f);
    Profiler::popClock();

    return (true);
}

bool VApp::createWindow()
{
    int w = Config::getInt("window","width",DEFAULT_WINDOW_WIDTH);
    int h = Config::getInt("window","height",DEFAULT_WINDOW_HEIGHT);
    bool fullscreen = Config::getBool("window","fullscreen",DEFAULT_WINDOW_FULLSCREEN);
    size_t framesCount = MAX_FRAMES_IN_FLIGHT;

    return m_renderWindow.create(w,h,m_createInfos.name,fullscreen, framesCount);
}

bool VApp::createRenderers()
{
    for(auto renderer : m_renderers)
        delete renderer;
    m_renderers.clear();


    //m_renderers.push_back(new SceneRenderer(&m_renderWindow, Renderer_Scene, Renderer_Unique));

    m_renderers.push_back(new SceneRenderer(&m_renderWindow, Renderer_Scene, Renderer_First));
    m_renderers.push_back(new UiRenderer(&m_renderWindow, Renderer_Ui, Renderer_Last));
    //m_renderWindow.attachRenderer(m_renderers.back());

    return (true);
}

void VApp::loop()
{
    Clock clock;

    clock.restart();
    while(m_running)
    {
        Time elapsedTime = clock.restart();
        Profiler::resetLoop(ENABLE_PROFILER);

        if(m_eventsManager.resizedWindow())
        {
            m_renderWindow.resize();
            while(m_eventsManager.getFramebufferSize() != glm::vec2(m_renderWindow.getSwapchainExtent().width,
                                                                    m_renderWindow.getSwapchainExtent().height))
                m_eventsManager.waitForEvents();
            clock.restart();
        }

        Profiler::pushClock("Acquire next image");
        m_renderWindow.acquireNextImage(); //Also update renderers
        Profiler::popClock();


        VTexturesManager::instance()->update(m_renderWindow.getFrameIndex(),
                                             m_renderWindow.getImageIndex());

        AudioEngine::instance()->update();

        m_eventsManager.update();

        if(m_statesManager.peekState() == nullptr)
            this->stop();
        else {
            m_statesManager.handleEvents(&m_eventsManager);

            ///I should add a tick control
            Profiler::pushClock("States update");
            m_statesManager.update(elapsedTime);
            Profiler::popClock();

            Profiler::pushClock("States draw");
            m_statesManager.draw(&m_renderWindow);
            Profiler::popClock();
        }

        Profiler::pushClock("Display");
        m_renderWindow.display();
        Profiler::popClock();

        if(m_eventsManager.keyPressed(GLFW_KEY_P))
            m_renderWindow.takeScreenshot("../screenshots/screen.jpg");
    }



    VInstance::waitDeviceIdle();
}

void VApp::cleanup()
{
    for(auto renderer : m_renderers)
        delete renderer;
    m_renderers.clear();

    TexturesHandler::instance()->cleanAll();
    VBuffersAllocator::instance()->cleanAll();

    m_renderWindow.destroy();

    glfwTerminate();
}


}
