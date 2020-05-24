#ifndef NETMESSAGETYPES_H
#define NETMESSAGETYPES_H

#include "PouEngine/net/NetMessagesFactory.h"

#include "PouEngine/scene/SceneNode.h"
#include "net/SyncElements.h"

enum NetMessageType
{
    NetMessageType_Default,
    //NetMessageType_Slice,
    NetMessageType_Test,
    NetMessageType_WorldInitialization,
    NetMessageType_AskForWorldSync,
    NBR_RELIABLEMESSAGETYPES,
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

struct NetMessage_WorldInitialization : public pou::NetMessage
{
    NetMessage_WorldInitialization() : NetMessage(){}
    NetMessage_WorldInitialization(int t) : NetMessage(t){}

    virtual std::shared_ptr<pou::NetMessage> msgAllocator()
    {
        return std::make_shared<NetMessage_WorldInitialization>();
    }

    int world_id;

    float   localTime;
    int     dayTime;

    int nbr_nodes;
    std::vector<std::pair<int, NodeSync> > nodes; //NodeId, ParentNodeId, Node

    int nbr_spriteSheets;
    std::vector< std::pair<int, std::string > > spriteSheets; //Id, Path

    int nbr_spriteEntities;
    std::vector< std::pair<int,SpriteEntitySync> > spriteEntities;

    int nbr_characterModels;
    std::vector< std::pair<int, std::string > > characterModels; //Id, Path

    int nbr_characters;
    std::vector <std::pair<int, CharacterSync > > characters;

    virtual void serializeImpl(pou::Stream *stream);

    virtual void serializeNode(pou::Stream *stream, std::pair<int, NodeSync> &node);
    virtual void serializeSpriteSheet(pou::Stream *stream, std::pair<int, std::string > &spriteSheet);
    virtual void serializeSpriteEntity(pou::Stream *stream, std::pair<int, SpriteEntitySync> &spriteEntity);
    virtual void serializeCharacterModels(pou::Stream *stream, std::pair<int, std::string > &characterModel);
    virtual void serializeCharacters(pou::Stream *stream, std::pair<int, CharacterSync> &character);
};


struct NetMessage_AskForWorldSync : public pou::NetMessage
{
    NetMessage_AskForWorldSync() : NetMessage(){}
    NetMessage_AskForWorldSync(int t) : NetMessage(t){}

    virtual std::shared_ptr<pou::NetMessage> msgAllocator()
    {
        return std::make_shared<NetMessage_AskForWorldSync>();
    }

    int world_id;

    virtual void serializeImpl(pou::Stream *stream);
};




#endif // NETMESSAGETYPES_H

