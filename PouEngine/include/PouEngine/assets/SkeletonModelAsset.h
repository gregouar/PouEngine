#ifndef SkeletonModelAsset_H
#define SkeletonModelAsset_H

#include "PouEngine/assets/Asset.h"
#include "PouEngine/scene/SceneNode.h"

#include "tinyxml/tinyxml.h"

#include <list>
#include <memory>

namespace pou
{

class SkeletalAnimationFrameModel;
class SkeletonModelAsset;

struct FrameTag
{
    FrameTag() : value(0) {};

    ///std::string value;
    HashedString value;
};

class SkeletalAnimationCommandModel
{
    public:
        SkeletalAnimationCommandModel(SkeletalAnimationFrameModel *frameModel, SkelAnimCmdType type = Unknown_Command,
                                      HashedString nodeName = 0);
                                      //int nodeId = -1);
                                      //const std::string &node = std::string());
        virtual ~SkeletalAnimationCommandModel();

        bool loadFromXml(TiXmlElement *element);

        SkelAnimCmdType getType() const;
        std::pair<const glm::vec4&,const glm::vec4&> getAmount() const;
        float getRate() const;
        float getFrameTime() const;
        //const std::string &getNode() const;
        //int getNodeId() const;
        HashedString getNodeName() const;

    private:
        SkeletalAnimationFrameModel *m_frameModel;
        SkelAnimCmdType m_type;

        glm::vec4  m_amount, m_enabledDirection;
        float   m_rate;

        //std::string m_node; //Need to switch from string to pointer or something better
        //int m_nodeId;
        HashedString m_nodeName;
};

class SkeletalAnimationFrameModel
{
    friend class SkeletalAnimationModel;

    public:
        SkeletalAnimationFrameModel(SkeletonModelAsset *skeletonModel);
        virtual ~SkeletalAnimationFrameModel();

        bool loadFromXml(TiXmlElement *element);

        const SkeletonModelAsset *getSkeletonModel() const;
        const std::list<SkeletalAnimationCommandModel> *getCommands() const;
        const std::list<HashedString> *getSounds() const;
        ///const std::list<std::pair<int,int> > *getStates() const; //return list of NodeId,StateId
        const std::list<std::pair<HashedString,HashedString> > *getStates() const; //return list of NodeId,StateId
        float getFrameTime() const;
        float getSpeedFactor() const;

        //bool hasTag(const std::string &tag);
        bool hasTag(HashedString tag);
       // std::pair <std::unordered_multimap<HashedString, FrameTag>::iterator, std::unordered_multimap<HashedString, FrameTag>::iterator>
         //   getTagValues(const std::string &tag);
        std::pair <std::unordered_multimap<HashedString, FrameTag>::iterator, std::unordered_multimap<HashedString, FrameTag>::iterator>
            getTagValues(HashedString tag);

    protected:
        void setNextFrame(SkeletalAnimationFrameModel *nextFrame);

    private:
        SkeletonModelAsset *m_skeletonModel;

        SkeletalAnimationFrameModel *m_nextFrame;
        std::list<SkeletalAnimationCommandModel> m_commands;
        std::unordered_multimap<HashedString, FrameTag> m_tags;
        std::list<HashedString> m_sounds;
        ///std::list<std::pair<int, int> > m_states;
        std::list<std::pair<HashedString, HashedString> > m_states;

        float m_speedFactor;
        float m_frameTime;
};

class SkeletalAnimationModel
{
    public:
        SkeletalAnimationModel(SkeletonModelAsset *skeletonModel,  HashedString name);
                               //int id);
        virtual ~SkeletalAnimationModel();

        bool loadFromXml(TiXmlElement *element);

        std::pair<SkeletalAnimationFrameModel*, bool> nextFrame(SkeletalAnimationFrameModel* curFrame);

        void setName(HashedString name);

        HashedString getName() const;
        //int getId() const;
        bool isLooping();

    private:
        SkeletonModelAsset *m_skeletonModel;

        bool m_isLooping;
        HashedString m_name;
        //int m_id;
        std::list<SkeletalAnimationFrameModel> m_frames;
};


///SINCE I'm now using HashedString, I should probably remove these ID stuff...
class SkeletonModelAsset : public Asset
{
    public:
        SkeletonModelAsset();
        SkeletonModelAsset(const AssetTypeId);
        virtual ~SkeletonModelAsset();

        bool loadFromFile(const std::string &filePath);

        const SceneNode *getRootNode() const;
        std::unordered_map<HashedString, SceneNode*> getNodesByName();
        ///const std::unordered_map<int, SceneNode*> *getNodesById();
        ///const std::list<std::pair<int,int> > *getInitialStates() const;
        const std::list<std::pair<HashedString,HashedString> > *getInitialStates() const;

        ///SkeletalAnimationModel* findAnimation(int id);
        SkeletalAnimationModel* findAnimation(HashedString name);

        /**int generateNodeId(const std::string &nodeName);
        int generateAnimationId(const std::string &animationName);
        int generateSoundId(const std::string &soundName);
        int generateStateId(const std::string &stateName);

        int getNodeId(const std::string &nodeName) const;
        int getAnimationId(const std::string &animationName) const;
        int getSoundId(const std::string &soundName) const;
        int getStateId(const std::string &stateName) const;**/

    protected:
        bool loadFromXML(TiXmlHandle *);
        void loadNode(SceneNode* rootNode, TiXmlElement *element);
        void loadAnimation(TiXmlElement *element);

        ///int generateId(const std::string &name, std::unordered_map<HashedString, int> &namesMap);
        ///int getId(const std::string &name, const std::unordered_map<HashedString, int> &namesMap) const;

    private:
        SceneNode m_rootNode;
        std::unordered_map<HashedString, SceneNode*> m_nodesByName;
        ///std::unordered_map<int, SceneNode*> m_nodesById;

        ///std::list<std::pair<int,int> > m_initialStates;
        std::list<std::pair<HashedString,HashedString> > m_initialStates;

        /**std::unordered_map<HashedString, int> m_nodeIdByName;
        std::unordered_map<HashedString, int> m_animationIdByName;
        std::unordered_map<HashedString, int> m_soundIdByName;
        std::unordered_map<HashedString, int> m_stateIdByName;**/

        std::unordered_multimap<HashedString, HashedString> m_joints;

        std::unordered_map<HashedString, std::unique_ptr<SkeletalAnimationModel> > m_animations;
        //std::unordered_map<int, SkeletalAnimationModel*> m_animationById;
};

}

#endif // SkeletonModelAsset_H
