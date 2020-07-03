#ifndef INGAMESTATE_H
#define INGAMESTATE_H

#include "PouEngine/core/GameState.h"
#include "PouEngine/core/Singleton.h"

#include "PouEngine/renderers/SceneRenderer.h"

#include "ui/GameUi.h"
#include "world/GameWorld.h"

class InGameState : public pou::GameState, public Singleton<InGameState>, pou::NotificationListener
{
    friend class Singleton<InGameState>;

    public:
        virtual void entered();
        virtual void leaving();
        virtual void revealed();
        virtual void obscuring();

        virtual void handleEvents(const EventsManager *eventsManager);
        virtual void update(const pou::Time &elapsedTime);
        virtual void draw(pou::RenderWindow *renderWindow);

        void setClientId(int clientId);
        void changeWorld(GameWorld *world, int playerId);

    protected:
        InGameState();
        virtual ~InGameState();

        void init();

        virtual void notify(pou::NotificationSender*, int notificationType,
                            void* data);



    private:
        bool m_firstEntering;

        //GameServer  m_gameServer;
        //size_t      m_localClientNbr;

        size_t      m_clientId;
        size_t      m_playerId;
        GameWorld  *m_world;

        GameUi m_gameUi;
};


#endif // INGAMESTATE_H
