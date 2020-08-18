#ifndef SYNCELEMENTS_H
#define SYNCELEMENTS_H

#include "Types.h"

class Character;
class Player;
class NodeSyncer;
class WorldSprite;
class WorldMesh;
class PrefabInstance;

/*struct NodeSync
{
    int parentNodeId;
    //pou::SceneNode *node;
    NodeSyncer *nodeSyncer;
};*/

struct SpriteEntitySync
{
    int spriteSheetId;
    int spriteId; //Inside spritesheet
    int nodeId;

    WorldSprite *spriteEntity;
};


struct MeshEntitySync
{
    int meshModelId;
    int nodeId;

    WorldMesh *meshEntity;
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

    int gearModelsId[NBR_GEAR_TYPES];
    std::vector<int> inventoryItemModelsId;

    std::shared_ptr<Player> player;
};

struct PrefabSync
{
    int prefabModelId;
    int nodeId;

    PrefabInstance *prefab;
};

#endif // SYNCELEMENTS_H
