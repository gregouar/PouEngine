#ifndef GAMEMESSAGETYPES_H_INCLUDED
#define GAMEMESSAGETYPES_H_INCLUDED


#include "PouEngine/core/MessageBus.h"

class Character;
class WorldNode;
class WorldSprite;

enum GameMessageType
{
    GameMessageType_First = pou::NotificationType_Custom,
    GameMessageType_NodeUpdated,
    GameMessageType_SpriteUpdated,
    GameMessageType_CharacterUpdated,
    GameMessageType_CharacterDamaged,
    GameMessageType_Last,
};

struct GameMessage_NodeUpdated
{
    WorldNode   *node;
};

struct GameMessage_SpriteUpdated
{
    WorldSprite *sprite;
};

struct GameMessage_CharacterUpdated
{
    Character   *character;
};

struct GameMessage_CharacterDamaged
{
    Character   *character;
    int         damages;
    glm::vec2   direction;
    //bool        fatal;
};


#endif // GAMEMESSAGETYPES_H_INCLUDED
