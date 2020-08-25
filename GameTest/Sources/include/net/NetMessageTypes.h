#ifndef NETMESSAGETYPES_H
#define NETMESSAGETYPES_H

#include "PouEngine/net/NetMessagesFactory.h"

#include "PouEngine/scene/SceneNode.h"
#include "net/SyncElements.h"
#include "character/Player.h"
#include "character/PlayerSave.h"


enum NetMessageType
{
    NetMessageType_ConnectionStatus,
    NetMessageType_Test,
    NetMessageType_WorldInit,
    NetMessageType_WorldSync,
    NetMessageType_PlayerSync,
    NetMessageType_PlayerEvent,
    //NetMessageType_AskForWorldSync,
    //NetMessageType_PlayerAction,
    NBR_RELIABLEMESSAGETYPES,
};

enum PlayerEventType
{
    PlayerEventType_CharacterDamaged,
    NBR_PLAYEREVENTTYPES,
};

void initializeNetMessages();

struct NetMessage_Test : public pou::NetMessage
{
    NetMessage_Test() : NetMessage(){}
    NetMessage_Test(int t) : NetMessage(t){}

    virtual std::shared_ptr<pou::NetMessage> msgAllocator()
    {
        return std::make_shared<NetMessage_Test>();
    }

    int test_value;

   // int dummy;

    virtual void serializeImpl(pou::Stream *stream)
    {
        //for(int i = 0 ; i < 0 ; ++i)
          //  stream->serializeBits(dummy, 8);

        stream->serializeBits(test_value, 16);

       // for(int i = 0 ; i < 0 ; ++i)
         //   stream->serializeBits(dummy, 8);
    }
};


struct NetMessage_WorldSync : public pou::NetMessage
{
    NetMessage_WorldSync();
    NetMessage_WorldSync(int t);

    virtual std::shared_ptr<pou::NetMessage> msgAllocator()
    {
        return std::make_shared<NetMessage_WorldSync>();
    }

    uint32_t   lastSyncTime;
    uint32_t   clientTime; //This is the last known client time
    uint32_t   localTime; //This is the server time
    //int     dayTime;

    //int player_id;

    std::list<std::shared_ptr<NodeSyncer> >     nodesBuffer;
    /**std::list<std::shared_ptr<WorldSprite> >    spritesBuffer;
    std::list<std::shared_ptr<WorldMesh> >      meshesBuffer;**/
    std::list<std::shared_ptr<Character> >      charactersBuffer;
    /**std::list<std::shared_ptr<PrefabInstance> > prefabsBuffer;**/

    //int nbr_nodes;
    //std::vector< std::pair<int, NodeSync> > nodes; //NodeId, Node
    std::vector< NodeSyncer* > nodeSyncers;
    /**std::vector< std::pair<int, std::string > > spriteSheets; //Id, Path
    std::vector< std::pair<int, SpriteEntitySync> > spriteEntities;
    std::vector< std::pair<int, std::string > > meshModels; //Id, Path
    std::vector< std::pair<int, MeshEntitySync> > meshEntities;**/
    std::vector< std::pair<int, std::string > > characterModels; //Id, Path
    std::vector< std::pair<int, CharacterSync > > characters;
    std::vector< std::pair<int, std::string > > itemModels; //Id, Path
    std::vector< std::pair<int, PlayerSync > > players;
    /**std::vector< std::pair<int, std::string > > prefabModels; //Id, Path
    std::vector< std::pair<int, PrefabSync> > prefabs;**/

    std::vector<int> desyncNodes;
    std::vector<int> desyncCharacters;
    std::vector<int> desyncPlayers;

    virtual void serializeImpl(pou::Stream *stream);

    void serializeNode(pou::Stream *stream, NodeSyncer *(&nodeSyncer));
    /**void serializeSpriteSheet(pou::Stream *stream, std::pair<int, std::string > &spriteSheet);
    void serializeSpriteEntity(pou::Stream *stream, std::pair<int, SpriteEntitySync> &spriteEntity);
    void serializeMeshModel(pou::Stream *stream, std::pair<int, std::string > &meshModel);
    void serializeMeshEntity(pou::Stream *stream, std::pair<int, MeshEntitySync> &meshEntity);**/
    void serializeCharacterModel(pou::Stream *stream, std::pair<int, std::string > &characterModel);
    void serializeCharacter(pou::Stream *stream, std::pair<int, CharacterSync> &character);
    void serializeItemModel(pou::Stream *stream, std::pair<int, std::string > &itemModel);
    void serializePlayer(pou::Stream *stream, std::pair<int, PlayerSync> &player);
    /**void serializePrefabAsset(pou::Stream *stream, std::pair<int, std::string > &prefabAsset);
    void serializePrefabInstance(pou::Stream *stream, std::pair<int, PrefabSync> &prefabInstance);**/
};

struct NetMessage_WorldInit : public NetMessage_WorldSync
{
    NetMessage_WorldInit() : NetMessage_WorldSync(){}
    NetMessage_WorldInit(int t) : NetMessage_WorldSync(t){}

    virtual std::shared_ptr<pou::NetMessage> msgAllocator()
    {
        return std::make_shared<NetMessage_WorldInit>();
    }

    int world_id;
    int player_id;
    int dayTime;

    std::string worldGeneratorModel;
    int worldGeneratorSeed;


    ///int worldGrid_nodeId;

    virtual void serializeImpl(pou::Stream *stream);
};


/**struct NetMessage_AskForWorldSync : public pou::NetMessage
{
    NetMessage_AskForWorldSync() : NetMessage(){}
    NetMessage_AskForWorldSync(int t) : NetMessage(t){}

    virtual std::shared_ptr<pou::NetMessage> msgAllocator()
    {
        return std::make_shared<NetMessage_AskForWorldSync>();
    }

    //int world_id;
    uint32_t lastSyncTime;
    uint32_t localTime;

    std::vector< std::pair<uint32_t, PlayerAction> > lastPlayerActions;

    virtual void serializeImpl(pou::Stream *stream);
};*/

struct NetMessage_PlayerSync : public pou::NetMessage
{
    NetMessage_PlayerSync() : NetMessage(){}
    NetMessage_PlayerSync(int t) : NetMessage(t){}

    virtual std::shared_ptr<pou::NetMessage> msgAllocator()
    {
        return std::make_shared<NetMessage_PlayerSync>();
    }

    //int world_id;
    uint32_t lastSyncTime;
    uint32_t localTime;

    bool useLockStepMode;

    ///For first sync only
    std::shared_ptr<PlayerSave> playerSave;

    ///For non lockstep mode
    std::shared_ptr<NodeSyncer> nodeBuffer;
    std::shared_ptr<Character> characterBuffer;

    NodeSyncer     *nodeSyncer;
    CharacterSync   characterSync;
    PlayerSync      playerSync;

    ///For lockstep mode
    std::vector< std::pair<uint32_t, PlayerAction> > lastPlayerActions;

    virtual void serializeImpl(pou::Stream *stream);
};

struct NetMessage_PlayerEvent : public pou::NetMessage
{

    NetMessage_PlayerEvent() : NetMessage(){}
    NetMessage_PlayerEvent(int t) : NetMessage(t){}

    virtual std::shared_ptr<pou::NetMessage> msgAllocator()
    {
        return std::make_shared<NetMessage_PlayerEvent>();
    }

    uint32_t localTime;
    uint32_t playerId;

    int         eventType;
    int         syncId;
    glm::vec2   direction;
    float       amount;

    virtual void serializeImpl(pou::Stream *stream);
};

/*struct NetMessage_PlayerAction : public pou::NetMessage
{
    NetMessage_PlayerAction() : NetMessage(){}
    NetMessage_PlayerAction(int t) : NetMessage(t){}

    virtual std::shared_ptr<pou::NetMessage> msgAllocator()
    {
        return std::make_shared<NetMessage_PlayerAction>();
    }

    uint32_t clientTime;
    PlayerAction playerAction;

    virtual void serializeImpl(pou::Stream *stream);
};*/



#endif // NETMESSAGETYPES_H

