#ifndef SYNCELEMENTS_H
#define SYNCELEMENTS_H

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
};

struct CharacterSync
{
    int characterModelId;
    int nodeId;

    Character *character;
};

struct PlayerSync
{
    int characterId;

    PlayableCharacter *player;
};

#endif // SYNCELEMENTS_H
