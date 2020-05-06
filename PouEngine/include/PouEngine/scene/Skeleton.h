#ifndef SKELETON_H
#define SKELETON_H

#include "PouEngine/assets/SkeletonModelAsset.h"
#include "PouEngine/scene/SceneNode.h"

namespace pou
{

struct SkeletalNodeState
{
    SkeletalNodeState();

    glm::vec4   posisiton;
    glm::vec4   rotation;
    glm::vec4   scale;
    glm::vec4   color;
};

class SkeletalAnimationCommand
{
    public:
        SkeletalAnimationCommand(const SkeletalAnimationCommandModel *model, SceneNode *node, SkeletalNodeState *nodeState);

        virtual bool update(const Time &elapsedTime);

    protected:
        void computeAmount();

    protected:
        const SkeletalAnimationCommandModel *m_model;
        SceneNode *m_node;
        SkeletalNodeState *m_nodeState;

        glm::vec4 m_value;
        float     m_curFrameTime;
        glm::vec4 m_amount;
        glm::vec4 m_enabledDirection;
};


class Skeleton : public SceneNode
{
    public:
        //Skeleton(SceneNode *rootNode, SkeletonModelAsset *model);
        Skeleton(SkeletonModelAsset *model);
        virtual ~Skeleton();

        bool attachLimb(const std::string &boneName, SceneObject *object);
        bool detachLimb(const std::string &boneName, SceneObject *object);
        bool detachAllLimbs(const std::string &boneName);

        bool attachSound(SoundObject *object, const std::string &soundName);

        bool startAnimation(const std::string &animationName, bool forceStart = false); //Probably could add ForceStart
        //could add pause animation etc

        bool isInAnimation();
        bool isNewFrame();
        bool hasTag(const std::string &tag);
        std::pair <std::multimap<std::string, FrameTag>::iterator, std::multimap<std::string, FrameTag>::iterator>
            getTagValues(const std::string &tag);
        const std::string &getCurrentAnimationName();

        const SceneNode* findNode(const std::string &name) const;
        const SceneNode* findNode(int id) const;

        virtual void update(const Time &elapsedTime);

    protected:
        void copyFromModel(SkeletonModelAsset *model);

        void loadAnimationCommands(SkeletalAnimationFrameModel *frame);

    protected:
        //SceneNode *m_rootNode;
        std::map<std::string, SceneNode*> m_nodesByName;
        std::map<int, SceneNode*> m_nodesById;
        std::map<SceneNode*, SkeletalNodeState> m_nodeStates;

    private:
        SkeletonModelAsset *m_model;

        SkeletalAnimationModel *m_nextAnimation, *m_curAnimation;
        SkeletalAnimationFrameModel *m_curAnimationFrame;
        bool m_forceNewAnimation;
        bool m_isNewFrame;

        std::list<SkeletalAnimationCommand> m_animationCommands;
        //bool m_createdRootNode;
};

}

#endif // SKELETON_H
