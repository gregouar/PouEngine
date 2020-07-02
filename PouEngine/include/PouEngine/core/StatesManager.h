#ifndef STATESMANAGER_H
#define STATESMANAGER_H

#include <vector>

#include "PouEngine/core/EventsManager.h"
#include "PouEngine/renderers/RenderWindow.h"

namespace pou
{

class VApp;
class GameState;

class StatesManager
{
    friend class VApp;

    public:
        StatesManager();
        virtual ~StatesManager();

        void stop();
        void switchState(GameState*);
        void pushState(GameState*);
        GameState* popState(); //Return new current state
        GameState* peekState();

        void handleEvents(const EventsManager *eventsManager);
        void update(const Time &elapsedTime);
        void draw(RenderWindow *renderWindow);
        void handleSwitchs();

        VApp* getApp();

    protected:
        void attachApp(VApp*);

        void switchStateImpl(GameState*);

    private:
        std::vector<GameState*> m_states;
        VApp *m_attachedApp;

        bool m_switchState;
        GameState *m_nextState;
};

}

#endif // STATESMANAGER_H
