#ifndef SkeletonModelAsset_H
#define SkeletonModelAsset_H

#include "PouEngine/assets/Asset.h"
#include "PouEngine/utils/SimpleNode.h"

#include "tinyxml/tinyxml.h"

#include <list>
#include <memory>

namespace pou
{

class SkeletalAnimationFrameModel;
class SkeletonModelAsset;

struct FrameTag
{
    std::string value;
};

class SkeletalAnimationCommandModel
{
    public:
        SkeletalAnimationCommandModel(SkeletalAnimationFrameModel *frameModel, SkelAnimCmdType type = Unknown_Command,
                                      int nodeId = -1);
                                      //const std::string &node = std::string());
        virtual ~SkeletalAnimationCommandModel();

        bool loadFromXml(TiXmlElement *element);

        SkelAnimCmdType getType() const;
        std::pair<const glm::vec4&,const glm::vec4&> getAmount() const;
        float getRate() const;
        float getFrameTime() const;
        //const std::string &getNode() const;
        int getNodeId() const;

    private:
        SkeletalAnimationFrameModel *m_frameModel;
        SkelAnimCmdType m_type;

        glm::vec4  m_amount, m_enabledDirection;
        float   m_rate;

        //std::string m_node; //Need to switch from string to pointer or something better
        int m_nodeId;
};

class SkeletalAnimationFrameModel
{
    friend class SkeletalAnimationModel;

    public:
        SkeletalAnimationFrameModel(SkeletonModelAsset *skeletonModel);
        virtual ~SkeletalAnimationFrameModel();

        bool loadFromXml(TiXmlElement *element/*, const std::map<int, SimpleNode*> *mapOfNodes = nullptr*/);

        const SkeletonModelAsset *getSkeletonModel() const;
        const std::list<SkeletalAnimationCommandModel> *getCommands() const;
        const std::list<int> *getSounds() const;
        const std::list<std::pair<int,int> > *getStates() const; //return list of NodeId,StateId
        float getFrameTime() const;
        float getSpeedFactor() const;

        bool hasTag(const std::string &tag);
        std::pair <std::multimap<std::string, FrameTag>::iterator, std::multimap<std::string, FrameTag>::iterator>
            getTagValues(const std::string &tag);

    protected:
        void setNextFrame(SkeletalAnimationFrameModel *nextFrame);

    private:
        SkeletonModelAsset *m_skeletonModel;

        SkeletalAnimationFrameModel *m_nextFrame;
        std::list<SkeletalAnimationCommandModel> m_commands;
        std::multimap<std::string, FrameTag> m_tags;
        std::list<int> m_sounds;
        std::list<std::pair<int, int> > m_states;

        float m_speedFactor;
        float m_frameTime;
};

class SkeletalAnimationModel
{
    public:
        SkeletalAnimationModel(SkeletonModelAsset *skeletonModel, int id);
        virtual ~SkeletalAnimationModel();

        bool loadFromXml(TiXmlElement *element/*, const std::map<int, SimpleNode*> *mapOfNodes = nullptr*/);

        std::pair<SkeletalAnimationFrameModel*, bool> nextFrame(SkeletalAnimationFrameModel* curFrame);

        void setName(const std::string &name);

        const std::string &getName() const;
        int getId() const;
        bool isLooping();

    private:
        SkeletonModelAsset *m_skeletonModel;

        bool m_isLooping;
        std::string m_name;
        int m_id;
        std::list<SkeletalAnimationFrameModel> m_frames;
};

class SkeletonModelAsset : public Asset
{
    public:
        SkeletonModelAsset();
        SkeletonModelAsset(const AssetTypeId);
        virtual ~SkeletonModelAsset();

        bool loadFromFile(const std::string &filePath);

        const SimpleNode *getRootNode() const;
        std::map<std::string, SimpleNode*> getNodesByName();
        const std::map<int, SimpleNode*> *getNodesById();
        const std::list<std::pair<int,int> > *getInitialStates() const;

        SkeletalAnimationModel* findAnimation(int id);
        SkeletalAnimationModel* findAnimation(const std::string &name);

        int generateNodeId(const std::string &nodeName);
        int generateAnimationId(const std::string &animationName);
        int generateSoundId(const std::string &soundName);
        int generateStateId(const std::string &stateName);

        int getNodeId(const std::string &nodeName) const;
        int getAnimationId(const std::string &animationName) const;
        int getSoundId(const std::string &soundName) const;
        int getStateId(const std::string &stateName) const;

    protected:
        bool loadFromXML(TiXmlHandle *);
        void loadNode(SimpleNode* rootNode, TiXmlElement *element);
        void loadAnimation(TiXmlElement *element);

        int generateId(const std::string &name, std::map<std::string, int> &namesMap);
        int getId(const std::string &name, const std::map<std::string, int> &namesMap) const;

    private:
        SimpleNode m_rootNode;
        std::map<std::string, SimpleNode*> m_nodesByName;
        std::map<int, SimpleNode*> m_nodesById;

        std::list<std::pair<int,int> > m_initialStates;

        std::map<std::string, int> m_nodeIdByName;
        std::map<std::string, int> m_animationIdByName;
        std::map<std::string, int> m_soundIdByName;
        std::map<std::string, int> m_stateIdByName;

        std::multimap<std::string, std::string> m_joints;

        std::map<std::string, std::unique_ptr<SkeletalAnimationModel> > m_animations;
        std::map<int, SkeletalAnimationModel*> m_animationById;
};

}

#endif // SkeletonModelAsset_H
