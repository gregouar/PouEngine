#ifndef PLAYERSERVERTESTINGSTATE_H
#define PLAYERSERVERTESTINGSTATE_H


#include "PouEngine/core/GameState.h"
#include "PouEngine/core/Singleton.h"

#include "PouEngine/renderers/SceneRenderer.h"

#include "PouEngine/ui/UserInterface.h"
#include "PouEngine/ui/UiPicture.h"

#include "net/GameServer.h"
#include "net/GameClient.h"
#include "ui/GameUi.h"

class PlayerServerTestingState : public pou::GameState, public Singleton<PlayerServerTestingState>
{
     friend class Singleton<PlayerServerTestingState>;

    public:
        virtual void entered();
        virtual void leaving();
        virtual void revealed();
        virtual void obscuring();

        virtual void handleEvents(const EventsManager *eventsManager);
        virtual void update(const pou::Time &elapsedTime);
        virtual void draw(pou::RenderWindow *renderWindow);

    protected:
        PlayerServerTestingState();
        virtual ~PlayerServerTestingState();

        void init();


    private:
        bool m_firstEntering;

        GameServer  m_gameServer;
        size_t      m_localClientNbr;

        GameUi m_gameUi;
};

#endif // PLAYERSERVERTESTINGSTATE_H
