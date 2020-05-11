#ifndef VAPP_H
#define VAPP_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "PouEngine/core/GameState.h"
#include "PouEngine/core/EventsManager.h"
#include "PouEngine/core/StatesManager.h"

#include "PouEngine/renderers/RenderWindow.h"
#include "PouEngine/renderers/AbstractRenderer.h"

namespace pou
{

struct VAppCreateInfos
{
    std::string name;
};

class VApp
{
    public:
        VApp(const VAppCreateInfos &infos);
        virtual ~VApp();


        void run(GameState *startingState);
        void stop();

        void printscreen();

    protected:
        bool    init();

        bool    createWindow();
        bool    createRenderers();

        void    loop();

        void    cleanup();

    private:
        bool        m_running;

        VAppCreateInfos     m_createInfos;

        StatesManager       m_statesManager;
        EventsManager       m_eventsManager;
        RenderWindow        m_renderWindow;

        std::vector<AbstractRenderer*> m_renderers;

        unsigned int m_sceenshotNbr;

    public:
        static const char *APP_VERSION;

        static const char *DEFAULT_APP_NAME;
        static const char *DEFAULT_CONFIG_FILE;
        static const char *DEFAULT_SCREENSHOTPATH;
        static const char *DEFAULT_SHADERPATH;

        static const char *DEFAULT_WINDOW_FULLSCREEN;
        static const char *DEFAULT_WINDOW_WIDTH;
        static const char *DEFAULT_WINDOW_HEIGHT;
        static const char *DEFAULT_VSYNC;
        static const char *DEFAULT_ANISOTROPIC;

        static const bool ENABLE_PROFILER;
        static const bool ENABLE_RANDOMNESS;
        static const size_t MAX_FRAMES_IN_FLIGHT;
};

}

#endif // VAPP_H

