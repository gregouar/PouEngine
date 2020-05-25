#ifndef SERVERTESTINGSTATE_H
#define SERVERTESTINGSTATE_H

#include "PouEngine/core/GameState.h"
#include "PouEngine/utils/Singleton.h"

#include "PouEngine/scene/Scene.h"

#include "net/GameServer.h"

class ServerTestingState : public pou::GameState, public Singleton<ServerTestingState>
{
     friend class Singleton<ServerTestingState>;

    public:
        void entered();
        void leaving();
        void revealed();
        void obscuring();

        void handleEvents(const EventsManager *eventsManager);
        void update(const pou::Time &elapsedTime);
        void draw(pou::RenderWindow *renderWindow);

    protected:
        ServerTestingState();
        virtual ~ServerTestingState();

        void init();

    private:
        bool m_firstEntering;

        GameServer m_gameServer;
};

#endif // SERVERTESTINGSTATE_H
