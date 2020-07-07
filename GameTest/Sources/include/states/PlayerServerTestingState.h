#ifndef PLAYERSERVERTESTINGSTATE_H
#define PLAYERSERVERTESTINGSTATE_H


#include "PouEngine/core/GameState.h"
#include "PouEngine/core/Singleton.h"

#include "PouEngine/renderers/SceneRenderer.h"

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

        void setConnectionData(int port, std::shared_ptr<PlayerSave> playerSave);

    protected:
        PlayerServerTestingState();
        virtual ~PlayerServerTestingState();

        void init();


    private:
        bool m_firstEntering;

        std::unique_ptr<GameServer>  m_gameServer;

        int m_port;
        std::shared_ptr<PlayerSave> m_playerSave;
        //size_t      m_localClientNbr;

        //GameUi m_gameUi;
};

#endif // PLAYERSERVERTESTINGSTATE_H
