#ifndef GAMEUI_H
#define GAMEUI_H

#include "PouEngine/ui/UserInterface.h"
#include "PouEngine/ui/UiPicture.h"
#include "PouEngine/ui/UiText.h"

#include "character/Player.h"

#include <memory>

class GameUi : public pou::UserInterface, public pou::NotificationListener
{
    public:
        GameUi();
        virtual ~GameUi();

        bool init();

        virtual void update(const pou::Time &elapsedTime) override;

        void setPlayer(Player *player);

        //void updateCharacterLife(float cur, float max);

    protected:
       // void cleanup();

        void updatePlayerLife();

        virtual void notify(pou::NotificationSender*, int notificationType,
                            void* data);


    private:
        Player *m_player;

        //std::unique_ptr<pou::UserInterface>     m_mainInterface;
        std::shared_ptr<pou::UiPicture>     m_uiPictureTest;
        std::shared_ptr<pou::UiProgressBar> m_lifeBar;

        std::shared_ptr<pou::UiText>        m_testText;
};

#endif // GAMEUI_H
