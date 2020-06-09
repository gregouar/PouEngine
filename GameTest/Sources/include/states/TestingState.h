#ifndef TESTINGSTATE_H
#define TESTINGSTATE_H

#include "PouEngine/core/GameState.h"
#include "PouEngine/utils/Singleton.h"
#include "PouEngine/renderers/SceneRenderer.h"
#include "PouEngine/scene/Scene.h"
#include "PouEngine/scene/LightEntity.h"

//#include "net/GameServer.h"
//#include "net/GameClient.h"
#include "ui/GameUi.h"

#include "character/Character.h"
#include "character/Player.h"

class TestingState : public pou::GameState, public Singleton<TestingState>
{
     friend class Singleton<TestingState>;

    public:
        virtual void entered();
        virtual void leaving();
        virtual void revealed();
        virtual void obscuring();

        virtual void handleEvents(const EventsManager *eventsManager);
        virtual void update(const pou::Time &elapsedTime);
        virtual void draw(pou::RenderWindow *renderWindow);


    protected:
        TestingState();
        virtual ~TestingState();

        void init();

    private:
        bool m_firstEntering;

        pou::Scene *m_scene;
        pou::Time m_totalTime;

        int m_nbrFps;

        bool m_activeCroc, m_activeDuck;

        //Maybe this does not really make sense and it should be SpriteAsset
        //pou::SpriteModel *m_charModel;
        pou::SceneNode     *m_testNode,
                            *m_cursorLightNode;
        pou::SpriteEntity *m_testChar;

        Player *m_character, *m_character2;

        Character  *m_croco, *m_duck;
        std::list<Character *> m_duckSwarm;
        std::list<Character *> m_trees;

        pou::LightEntity    *m_sunLight;
        float m_sunAngle, m_sunAngleVelocity;
        pou::Color m_dayColor, m_nightColor, m_sunsetColor;

        pou::SoundAsset     *m_soundTest;
        pou::SoundBankAsset *m_soundBankTest;

        pou::SoundTypeId    m_soundEventTest;

        pou::CameraObject   *m_camera, *m_listeningCamera;

        GameUi m_gameUi;

        /*pou::UserInterface  *m_mainInterface;
        pou::UiPicture      *m_uiPictureTest;
        pou::UiProgressBar   *m_lifeBar;*/

       // GameServer m_gameServer;
       // GameClient m_gameClient;
};

#endif // TESTINGSTATE_H

