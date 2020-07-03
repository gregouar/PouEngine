#ifndef PLAYERSAVE_H
#define PLAYERSAVE_H

#include "PouEngine/system/Stream.h"

#include "character/Player.h"

class PlayerSave
{
    public:
        PlayerSave();
        virtual ~PlayerSave();

        void createFromPlayer(Player *player);
        void loadToPlayer(Player *player);

        void serialize(pou::Stream *stream);

        void setPlayerName(const std::string &playerName); ///Only for testing purposes
        void setPlayerType(int playerType); ///Only for testing purposes

        int getPlayerType(); ///Only for testing purposes

    protected:

    private:
        pou::StringSyncElement m_playerName;

        pou::IntSyncElement m_playerType; ///Only for testing purposes
};

#endif // PLAYERSAVE_H
