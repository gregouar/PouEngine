#include <string>
#include <iostream>

#include "PouEngine/utils/Logger.h"
#include "PouEngine/core/VApp.h"

#include "net/NetMessageTypes.h"

#include "states/TestingState.h"
#include "states/ServerTestingState.h"
#include "states/ClientTestingState.h"
#include "states/ClientServerTestingState.h"

int main()
{
    pou::Logger::instance();

    srand (time(NULL));
    initializeNetMessages();

    try {
        pou::VAppCreateInfos createInfos;
        //createInfos.name = "ProjectW";
        //pou::VApp app(createInfos);
        //app.run(TestingState::instance());

        #ifdef CLIENTONLY
            createInfos.name = "ProjectW - client";
            pou::VApp app(createInfos);
            app.run(ClientTestingState::instance());
        #elif SERVERONLY
            createInfos.name = "ProjectW - dedicated server";
            pou::VApp app(createInfos);
            app.run(ServerTestingState::instance());
        #else
            createInfos.name = "ProjectW";
            pou::VApp app(createInfos);
            app.run(ClientServerTestingState::instance());
        #endif
    } catch (const std::exception& e) {
        pou::Logger::fatalError(e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
