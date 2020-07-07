#ifndef GAMEMESSAGETYPES_H_INCLUDED
#define GAMEMESSAGETYPES_H_INCLUDED


#include "PouEngine/core/MessageBus.h"

class GameWorld;
class WorldNode;
class WorldSprite;
class WorldMesh;
class Character;
class Player;

enum GameMessageType
{
    GameMessageType_First = pou::NotificationType_Custom,
    GameMessageType_World_NewPlayer,
    GameMessageType_World_NodeUpdated,
    GameMessageType_World_SpriteUpdated,
    GameMessageType_World_MeshUpdated,
    GameMessageType_World_CharacterUpdated,
    GameMessageType_World_CharacterDamaged,
    GameMessageType_Game_ChangeWorld,
    GameMessageType_Net_Connected,
    GameMessageType_Net_Disconnected,
    GameMessageType_Last,
};

struct GameMessage_World_NewPlayer
{
    Player   *player;
};

struct GameMessage_World_NodeUpdated
{
    WorldNode   *node;
};

struct GameMessage_World_SpriteUpdated
{
    WorldSprite *sprite;
};

struct GameMessage_World_MeshUpdated
{
    WorldMesh *mesh;
};

struct GameMessage_World_CharacterUpdated
{
    Character   *character;
};

struct GameMessage_World_CharacterDamaged
{
    Character   *character;
    int         damages;
    glm::vec2   direction;
    //bool        fatal;
};

struct GameMessage_Game_ChangeWorld
{
    int clientId;
    int playerId;
    GameWorld *world;
};


#endif // GAMEMESSAGETYPES_H_INCLUDED
