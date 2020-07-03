#include "character/PlayerSave.h"

PlayerSave::PlayerSave()
{
    m_playerName.useUpdateBit(false);
}

PlayerSave::~PlayerSave()
{
    //dtor
}

void PlayerSave::createFromPlayer(Player *player)
{
    m_playerName = player->getPlayerName();
}

void PlayerSave::loadToPlayer(Player *player)
{
    player->setPlayerName(m_playerName.getValue());
}

void PlayerSave::serialize(pou::Stream *stream)
{
    m_playerName.serialize(stream, 0);
    m_playerType.serialize(stream, 0);
}

void PlayerSave::setPlayerName(const std::string &playerName) ///Only for testing purposes
{
    m_playerName = playerName;
}

void PlayerSave::setPlayerType(int playerType) ///Only for testing purposes
{
    m_playerType = playerType;
}

int PlayerSave::getPlayerType() ///Only for testing purposes
{
    return m_playerType.getValue();
}
