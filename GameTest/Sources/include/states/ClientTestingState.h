#ifndef CLIENTTESTINGSTATE_H
#define CLIENTTESTINGSTATE_H

#include "PouEngine/core/GameState.h"
#include "PouEngine/core/Singleton.h"

#include "PouEngine/renderers/SceneRenderer.h"
#include "PouEngine/scene/Scene.h"

#include "PouEngine/ui/UserInterface.h"
#include "PouEngine/ui/UiPicture.h"

#include "net/GameClient.h"
#include "ui/GameUi.h"

class ClientTestingState : public pou::GameState, public Singleton<ClientTestingState>
{
     friend class Singleton<ClientTestingState>;

    public:
        virtual void entered();
        virtual void leaving();
        virtual void revealed();
        virtual void obscuring();

        virtual void handleEvents(const EventsManager *eventsManager);
        virtual void update(const pou::Time &elapsedTime);
        virtual void draw(pou::RenderWindow *renderWindow);

        void setConnectionData(const pou::NetAddress &serverAddress, std::shared_ptr<PlayerSave> playerSave);

    protected:
        ClientTestingState();
        virtual ~ClientTestingState();

        void init();

    private:
        bool m_firstEntering;

        std::unique_ptr<GameClient> m_gameClient;

        pou::NetAddress m_serverAddress;
        std::shared_ptr<PlayerSave> m_playerSave;
       // GameUi m_gameUi;
};

#endif // CLIENTTESTINGSTATE_H
