#ifndef SYNCELEMENTS_H
#define SYNCELEMENTS_H

#include "Types.h"

class Character;
class Player;
class WorldNode;
class WorldSprite;

struct NodeSync
{
    int parentNodeId;
    ///std::shared_ptr<WorldNode> node;
    WorldNode *node;
};

struct SpriteEntitySync
{
    int spriteSheetId;
    int spriteId; //Inside spritesheet
    int nodeId;

    ///std::shared_ptr<WorldSprite> spriteEntity;
    WorldSprite *spriteEntity;
};

struct CharacterSync
{
    int characterModelId;
    int nodeId;

    ///std::shared_ptr<Character> character;
    Character *character;
};

struct PlayerSync
{
    int characterId;

    int gearModelsId[NBR_GEAR_TYPES];
    std::vector<int> inventoryItemModelsId;

    std::shared_ptr<Player> player;
};

#endif // SYNCELEMENTS_H
