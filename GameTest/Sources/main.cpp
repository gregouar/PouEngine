#include <string>
#include <iostream>

#include "PouEngine/tools/Logger.h"
#include "PouEngine/core/VApp.h"

#include "net/NetMessageTypes.h"

#include "states/TestingState.h"
#include "states/ServerTestingState.h"
#include "states/ClientTestingState.h"
#include "states/ClientServerTestingState.h"
#include "states/PlayerServerTestingState.h"
#include "states/MainMenuState.h"

int main()
{
    pou::Logger::instance();

    try {
        pou::VAppCreateInfos createInfos;

        bool testingState = false;

        if(testingState)
        {
            createInfos.name = "ProjectW";
            pou::VApp app(createInfos);
            app.run(TestingState::instance());
        } else {

        #ifdef CLIENTONLY
            createInfos.name = "ProjectW - client";
            pou::VApp app(createInfos);
            app.run(ClientTestingState::instance());
        #elif SERVERONLY
            createInfos.name = "ProjectW - dedicated server";
            pou::VApp app(createInfos);
            app.run(ServerTestingState::instance());
        #elif PLAYERSERVER
            createInfos.name = "ProjectW - player server";
            pou::VApp app(createInfos);
            app.run(PlayerServerTestingState::instance());
        #else
            createInfos.name = "ProjectW";
            pou::VApp app(createInfos);
            app.run(MainMenuState::instance());
            //app.run(PlayerServerTestingState::instance());
            //app.run(ClientTestingState::instance());
            //app.run(ClientServerTestingState::instance());
        #endif

        }
    } catch (const std::exception& e) {
        pou::Logger::fatalError(e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
