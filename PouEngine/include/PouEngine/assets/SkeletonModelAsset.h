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

class SkeletalAnimationCommandModel
{
    public:
        SkeletalAnimationCommandModel(SkeletalAnimationFrameModel *frameModel, SkelAnimCmdType type = Unknown_Command,
                                      const std::string &node = std::string());
        virtual ~SkeletalAnimationCommandModel();

        bool loadFromXml(TiXmlElement *element);

        SkelAnimCmdType getType() const;
        std::pair<const glm::vec4&,const glm::vec4&> getAmount() const;
        float getRate() const;
        float getFrameTime() const;
        const std::string &getNode() const;

    private:
        SkeletalAnimationFrameModel *m_frameModel;
        SkelAnimCmdType m_type;

        glm::vec4  m_amount, m_enabledDirection;
        float   m_rate;

        std::string m_node; //Need to switch from string to pointer
};

class SkeletalAnimationFrameModel
{
    friend class SkeletalAnimationModel;

    public:
        SkeletalAnimationFrameModel();
        virtual ~SkeletalAnimationFrameModel();

        bool loadFromXml(TiXmlElement *element, const std::map<std::string, SimpleNode*> *mapOfNodes = nullptr);

        const std::list<SkeletalAnimationCommandModel> *getCommands();
        float getFrameTime() const;
        float getSpeedFactor() const;

        bool hasTag(const std::string &tag);

    protected:
        void setNextFrame(SkeletalAnimationFrameModel *nextFrame);

    private:
        SkeletalAnimationFrameModel *m_nextFrame;
        std::list<SkeletalAnimationCommandModel> m_commands;
        std::set<std::string> m_tags;

        float m_speedFactor;
        float m_frameTime;
};

class SkeletalAnimationModel
{
    public:
        SkeletalAnimationModel();
        virtual ~SkeletalAnimationModel();

        bool loadFromXml(TiXmlElement *element, const std::map<std::string, SimpleNode*> *mapOfNodes = nullptr);

        SkeletalAnimationFrameModel* nextFrame(SkeletalAnimationFrameModel* curFrame);

        void setName(const std::string &name);

        const std::string &getName() const;
        bool isLooping();

    private:
        bool m_isLooping;
        std::string m_name;
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

        SkeletalAnimationModel* findAnimation(const std::string &name);


    protected:
        bool loadFromXML(TiXmlHandle *);
        void loadNode(SimpleNode* rootNode, TiXmlElement *element);
        void loadAnimation(TiXmlElement *element);

    private:
        SimpleNode m_rootNode;
        std::map<std::string, SimpleNode*> m_nodesByName;
        std::multimap<std::string, std::string> m_joints;
        std::map<std::string, std::unique_ptr<SkeletalAnimationModel> > m_animations;
};

}

#endif // SkeletonModelAsset_H
