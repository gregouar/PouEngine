#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "PouEngine/Types.h"
#include "PouEngine/core/StatesManager.h"
#include "PouEngine/core/EventsManager.h"

namespace pou
{

class GameState
{
    public:
        GameState();
        virtual ~GameState();

        virtual void entered() = 0;
        virtual void leaving() = 0;
        virtual void revealed() = 0;
        virtual void obscuring() = 0;

        virtual void handleEvents(const EventsManager *eventsManager) = 0;
        virtual void update(const Time &elapsedTime) = 0;
        virtual void draw(RenderWindow *renderWindow) = 0;

        void setManager(StatesManager *);

        void pause();
        void resume();

    protected:
        StatesManager *m_manager;

    private:
        bool m_running;
};

}

#endif // GAMESTATE_H


