#ifndef GAMEMESSAGETYPES_H_INCLUDED
#define GAMEMESSAGETYPES_H_INCLUDED


#include "PouEngine/core/MessageBus.h"

enum GameMessageType
{
    GameMessageType_First = pou::NotificationType_Custom,
    GameMessageType_CharacterDamaged,
    GameMessageType_Last,
};

struct GameMessage_CharacterDamaged
{
    Character   *character;
    int         damages;
    glm::vec2   direction;
    //bool        fatal;
};


#endif // GAMEMESSAGETYPES_H_INCLUDED
