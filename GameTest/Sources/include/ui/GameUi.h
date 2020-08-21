#ifndef GAMEUI_H
#define GAMEUI_H

#include "PouEngine/ui/UserInterface.h"
#include "PouEngine/ui/UiPicture.h"
#include "PouEngine/ui/UiText.h"
#include "PouEngine/ui/UiButton.h"

#include "character/Player.h"

#include <memory>

struct OtherPlayerUi
{
    Player *player;
    std::shared_ptr<pou::UiText> playerNameText;
};

class GameUi : public pou::UserInterface, public pou::NotificationListener
{
    public:
        GameUi();
        virtual ~GameUi();

        bool init();

        virtual void update(const pou::Time &elapsedTime) override;

        void setPlayer(Player *player);
        void setRTTInfo(float RTT);

        bool askForRespawn();

    protected:
       // void cleanup();

        void updatePlayerHud();
        void updatePlayerList();
        void updateYouDied();

        void addPlayer(Player *player);
        void removePlayer(Player *player);

        virtual void notify(pou::NotificationSender*, int notificationType,
                            void* data);


    private:
        Player *m_player;

        pou::FontAsset *m_font;
        pou::FontAsset *m_font_pixel;

        std::shared_ptr<pou::UiElement>     m_playerHud;
        std::shared_ptr<pou::UiProgressBar> m_lifeBar;

        std::shared_ptr<pou::UiText>        m_charNameText;
        std::vector< OtherPlayerUi >        m_playerList;

        std::shared_ptr<pou::UiElement>     m_youDiedUi;
        std::shared_ptr<pou::UiButton>      m_respawnButton;

        float m_RTTInfo;
        std::shared_ptr<pou::UiText>        m_RTTText;
};

#endif // GAMEUI_H
