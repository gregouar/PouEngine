#ifndef TESTINGSTATE_H
#define TESTINGSTATE_H

#include "PouEngine/core/GameState.h"
#include "PouEngine/utils/Singleton.h"
#include "PouEngine/renderers/SceneRenderer.h"
#include "PouEngine/scene/Scene.h"
#include "PouEngine/scene/LightEntity.h"

#include "PouEngine/ui/UserInterface.h"
#include "PouEngine/ui/UiPicture.h"

#include "Character.h"
#include "PlayableCharacter.h"

class TestingState : public pou::GameState, public Singleton<TestingState>
{
     friend class Singleton<TestingState>;

    public:
        void entered();
        void leaving();
        void revealed();
        void obscuring();

        void handleEvents(const EventsManager *eventsManager);
        void update(const pou::Time &elapsedTime);
        void draw(pou::RenderWindow *renderWindow);


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
        pou::SceneNode     *m_testNode;
        pou::SpriteEntity *m_testChar;

        PlayableCharacter *m_character;

        Character  *m_character2, *m_croco, *m_duck;
        std::list<Character *> m_duckSwarm;
        std::list<Character *> m_trees;

        pou::SoundAsset     *m_soundTest;
        pou::SoundBankAsset *m_soundBankTest;

        pou::SoundTypeId    m_soundEventTest;

        pou::CameraObject   *m_camera, *m_listeningCamera;
        //pou::SceneNode      *m_cameraNode;
        //glm::vec3            m_camVelocity;
        //glm::vec2            m_charVelocity;

        pou::UserInterface  *m_mainInterface;
        pou::UiPicture      *m_uiPictureTest;
        pou::UiProgressBar   *m_lifeBar;
};

#endif // TESTINGSTATE_H
