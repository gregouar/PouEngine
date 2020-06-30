#ifndef NETMESSAGETYPES_H
#define NETMESSAGETYPES_H

#include "PouEngine/net/NetMessagesFactory.h"

#include "PouEngine/scene/SceneNode.h"
#include "net/SyncElements.h"
#include "character/Player.h"


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

    std::list<std::shared_ptr<WorldNode> >      nodesBuffer;
    std::list<std::shared_ptr<WorldSprite> >    spritesBuffer;
    std::list<std::shared_ptr<Character> >      charactersBuffer;

    //int nbr_nodes;
    std::vector< std::pair<int, NodeSync> > nodes; //NodeId, Node
    std::vector< std::pair<int, std::string > > spriteSheets; //Id, Path
    std::vector< std::pair<int,SpriteEntitySync> > spriteEntities;
    std::vector< std::pair<int, std::string > > characterModels; //Id, Path
    std::vector< std::pair<int, CharacterSync > > characters;
    std::vector< std::pair<int, std::string > > itemModels; //Id, Path
    std::vector< std::pair<int, PlayerSync > > players;

    std::vector<int> desyncNodes;
    std::vector<int> desyncCharacters;
    std::vector<int> desyncPlayers;

    virtual void serializeImpl(pou::Stream *stream);

    virtual void serializeNode(pou::Stream *stream, std::pair<int, NodeSync> &node);
    virtual void serializeSpriteSheet(pou::Stream *stream, std::pair<int, std::string > &spriteSheet);
    virtual void serializeSpriteEntity(pou::Stream *stream, std::pair<int, SpriteEntitySync> &spriteEntity);
    virtual void serializeCharacterModel(pou::Stream *stream, std::pair<int, std::string > &characterModel);
    virtual void serializeCharacter(pou::Stream *stream, std::pair<int, CharacterSync> &character);
    virtual void serializeItemModel(pou::Stream *stream, std::pair<int, std::string > &itemModel);
    virtual void serializePlayer(pou::Stream *stream, std::pair<int, PlayerSync> &player);
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

    std::shared_ptr<WorldNode> nodeBuffer;
    std::shared_ptr<Character> characterBuffer;

    NodeSync        nodeSync;
    CharacterSync   characterSync;
    PlayerSync      playerSync;


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

    //uint32_t localTime;

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

