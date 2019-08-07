#include <iostream>

#include "PouEngine/utils/Logger.h"
#include "PouEngine/core/VApp.h"
#include "states/TestingState.h"

int main()
{
    Logger::instance();

    try {
        pou::VAppCreateInfos createInfos;
        createInfos.name = "ProjectW";

        pou::VApp app(createInfos);
        app.run(TestingState::instance());
    } catch (const std::exception& e) {
        Logger::fatalError(e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
