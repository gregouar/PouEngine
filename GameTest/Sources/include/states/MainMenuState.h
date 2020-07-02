#ifndef MAINMENUSTATE_H
#define MAINMENUSTATE_H

#include "PouEngine/core/GameState.h"
#include "PouEngine/core/Singleton.h"

#include "PouEngine/ui/UserInterface.h"
#include "PouEngine/ui/UiPicture.h"
#include "PouEngine/ui/UiButton.h"
#include "PouEngine/ui/UiTextInput.h"

class MainMenuState : public pou::GameState, public Singleton<MainMenuState>
{
    friend class Singleton<MainMenuState>;

    public:
        virtual void entered();
        virtual void leaving();
        virtual void revealed();
        virtual void obscuring();

        virtual void handleEvents(const EventsManager *eventsManager);
        virtual void update(const pou::Time &elapsedTime);
        virtual void draw(pou::RenderWindow *renderWindow);

    protected:
        MainMenuState();
        virtual ~MainMenuState();

        void init();

    protected:

    private:
        bool m_firstEntering;

        pou::UserInterface m_ui;
        std::shared_ptr<pou::UiButton> m_createServerButton;
        std::shared_ptr<pou::UiButton> m_connectToServerButton;

        std::shared_ptr<pou::UiTextInput> m_serverAddressInput;
        std::shared_ptr<pou::UiTextInput> m_serverPortInput;
};

#endif // MAINMENUSTATE_H
