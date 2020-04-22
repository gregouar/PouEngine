#ifndef SKELETON_H
#define SKELETON_H

#include "PouEngine/assets/SkeletonModelAsset.h"
#include "PouEngine/scene/SceneNode.h"

namespace pou
{

struct SkeletalNodeState
{
    SkeletalNodeState();

    glm::vec3   posisiton;
    glm::vec3   rotation;
    glm::vec3   scale;
};

class SkeletalAnimationCommand
{
    public:
        SkeletalAnimationCommand(const SkeletalAnimationCommandModel *model, SimpleNode *node, SkeletalNodeState *nodeState);

        virtual bool update(const Time &elapsedTime);

    protected:
        void computeAmount();

    protected:
        const SkeletalAnimationCommandModel *m_model;
        SimpleNode *m_node;
        SkeletalNodeState *m_nodeState;

        glm::vec3 m_value;
        glm::vec3 m_amount;
        glm::vec3 m_enabledDirection;
};


class Skeleton : public SceneNode
{
    public:
        //Skeleton(SceneNode *rootNode, SkeletonModelAsset *model);
        Skeleton(SkeletonModelAsset *model);
        virtual ~Skeleton();

        bool attachLimb(const std::string &boneName, SceneObject *object);

        bool startAnimation(const std::string &animationName, bool forceStart = false); //Probably could add ForceStart
        //could add pause animation etc

        bool isInAnimation();
        bool hasTag(const std::string &tag);

        const SceneNode* findNode(const std::string &name) const;

        virtual void update(const Time &elapsedTime);

    protected:
        void copyFromModel(SkeletonModelAsset *model);

        void loadAnimationCommands(SkeletalAnimationFrameModel *frame);

    protected:
        //SceneNode *m_rootNode;
        std::map<std::string, SceneNode*> m_nodesByName;
        std::map<SceneNode*, SkeletalNodeState> m_nodeStates;

    private:
        SkeletonModelAsset *m_model;

        SkeletalAnimationModel *m_nextAnimation, *m_curAnimation;
        SkeletalAnimationFrameModel *m_curAnimationFrame;
        bool m_forceNewAnimation;

        std::list<SkeletalAnimationCommand> m_animationCommands;
        //bool m_createdRootNode;
};

}

#endif // SKELETON_H
