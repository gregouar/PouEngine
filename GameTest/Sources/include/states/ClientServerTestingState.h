#ifndef CLIENTSERVERTESTINGSTATE_H
#define CLIENTSERVERTESTINGSTATE_H

#include "PouEngine/core/GameState.h"
#include "PouEngine/core/Singleton.h"

#include "PouEngine/renderers/SceneRenderer.h"

#include "PouEngine/ui/UserInterface.h"
#include "PouEngine/ui/UiPicture.h"

#include "net/GameServer.h"
#include "net/GameClient.h"
#include "ui/GameUi.h"

class ClientServerTestingState : public pou::GameState, public Singleton<ClientServerTestingState>
{
     friend class Singleton<ClientServerTestingState>;

    public:
        virtual void entered();
        virtual void leaving();
        virtual void revealed();
        virtual void obscuring();

        virtual void handleEvents(const EventsManager *eventsManager);
        virtual void update(const pou::Time &elapsedTime);
        virtual void draw(pou::RenderWindow *renderWindow);

    protected:
        ClientServerTestingState();
        virtual ~ClientServerTestingState();

        void init();

    private:
        bool m_firstEntering;

        GameServer m_gameServer;
        GameClient m_gameClient;

        GameUi m_gameUi;
};

#endif // CLIENTSERVERTESTINGSTATE_H
