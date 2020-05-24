#ifndef SYNCENTITIES_H
#define SYNCENTITIES_H

struct NodeSync
{
    int parentNodeId;
    pou::SceneNode *node;
};

struct SpriteEntitySync
{
    int      spriteSheetId;
    int      spriteId; //Inside spritesheet
    int      nodeId;
};

#endif // SYNCENTITIES_H
