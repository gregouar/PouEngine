#ifndef TESTINGSTATE_H
#define TESTINGSTATE_H

#include "PouEngine/core/GameState.h"
#include "PouEngine/utils/Singleton.h"
#include "PouEngine/renderers/SceneRenderer.h"
#include "PouEngine/scene/Scene.h"
#include "PouEngine/scene/LightEntity.h"


#include "Character.h"

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

        //Maybe this does not really make sense and it should be SpriteAsset
        pou::SpriteModel *m_charModel;
        pou::SceneNode      *m_testNode;
        pou::SpriteEntity *m_testChar;

        Character *m_character, *m_croco;

        pou::CameraObject   *m_camera;
        pou::SceneNode      *m_cameraNode;
        glm::vec2            m_camVelocity;
        //glm::vec2            m_charVelocity;
};

#endif // TESTINGSTATE_H
