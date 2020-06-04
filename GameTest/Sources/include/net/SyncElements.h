#ifndef SYNCELEMENTS_H
#define SYNCELEMENTS_H

#include "PouEngine/scene/SpriteEntity.h"
#include "Types.h"

class Character;
class PlayableCharacter;

struct NodeSync
{
    int parentNodeId;
    pou::SceneNode *node;
};

struct SpriteEntitySync
{
    int spriteSheetId;
    int spriteId; //Inside spritesheet
    int nodeId;

    pou::SpriteEntity* spriteEntity;
};

struct CharacterSync
{
    int characterModelId;
    int nodeId;

    Character *character;
};

struct PlayerSync
{
    int itemModelsId[NBR_GEAR_TYPES];
    int characterId;

    PlayableCharacter *player;
};

#endif // SYNCELEMENTS_H
