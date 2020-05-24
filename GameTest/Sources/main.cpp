#include <string>
#include <iostream>

#include "PouEngine/utils/Logger.h"
#include "PouEngine/core/VApp.h"

#include "net/NetMessageTypes.h"

#include "states/TestingState.h"
#include "states/ClientServerTestingState.h"

int main()
{
    pou::Logger::instance();

    srand (time(NULL));
    initializeNetMessages();

    try {
        pou::VAppCreateInfos createInfos;
        createInfos.name = "ProjectW";

        pou::VApp app(createInfos);
        //app.run(TestingState::instance());
        app.run(ClientServerTestingState::instance());
    } catch (const std::exception& e) {
        pou::Logger::fatalError(e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
