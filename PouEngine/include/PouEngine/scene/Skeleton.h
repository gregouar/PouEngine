#ifndef SKELETON_H
#define SKELETON_H

#include "PouEngine/assets/SkeletonModelAsset.h"
#include "PouEngine/scene/SceneNode.h"

namespace pou
{

struct SkeletalNodeState
{
    SkeletalNodeState();

    //void update(const Time &elapsedTime, uint32_t localTime = -1);
    ///void rewind(uint32_t time);

    /**SyncAttribute<glm::vec4>   posisiton;
    SyncAttribute<glm::vec4>   rotation;
    SyncAttribute<glm::vec4>   scale;
    SyncAttribute<glm::vec4>   color;**/

    glm::vec4 posisiton;
    glm::vec4 rotation;
    glm::vec4 scale;
    glm::vec4 color;
};

class SkeletalAnimationCommand
{
    public:
        SkeletalAnimationCommand(const SkeletalAnimationCommandModel *model, SceneNode *node, SkeletalNodeState *nodeState,
                                 float startingFrameTime = 0);
        virtual ~SkeletalAnimationCommand();

        virtual bool update(const Time &elapsedTime, uint32_t localTime);
        ///void rewind(uint32_t time);

    protected:
        void computeAmount();

    protected:
        const SkeletalAnimationCommandModel *m_model;
        SceneNode *m_node;
        SkeletalNodeState *m_nodeState;

        /**SyncAttribute<glm::vec4>  m_value;
        SyncAttribute<float>      m_curFrameTime;**/

        glm::vec4  m_value;
        float      m_curFrameTime;

        float     m_startingFrameTime;
        glm::vec4 m_amount;
        glm::vec4 m_enabledDirection;
};


class Skeleton : public SceneNode
{
    public:
        //Skeleton(SceneNode *rootNode, SkeletonModelAsset *model);
        Skeleton(SkeletonModelAsset *model);
        virtual ~Skeleton();

        bool attachLimb(HashedString boneName, HashedString stateName, std::shared_ptr<SceneObject> object);
        bool detachLimb(HashedString boneName, HashedString stateName, SceneObject *object);

        /**void attachLimbsOfState(int nodeId, int stateId);
        void detachLimbsOfDifferentState(int nodeId, int stateId);**/
        void attachLimbsOfState(HashedString nodeName, HashedString stateName);
        void detachLimbsOfDifferentState(HashedString nodeName, HashedString stateName);
        /*void attachLimbsOfState(const std::string &boneName, const std::string &stateName);
        void detachLimbsOfDifferentState(const std::string &boneName, const std::string &stateName);*/
        //bool detachAllLimbs(const std::string &boneName);

        bool attachSound(std::shared_ptr<SoundObject> object, HashedString soundName);
        bool detachSound(SoundObject *object, HashedString soundName);

        bool startAnimation(HashedString animationName, bool forceStart = false);
        ///bool startAnimation(int animationId, bool forceStart = false);
        //could add pause animation etc

        bool isInAnimation();
        bool isNewFrame();
        bool hasTag(HashedString tag);
        std::pair <std::unordered_multimap<HashedString, FrameTag>::iterator, std::unordered_multimap<HashedString, FrameTag>::iterator>
            getTagValues(HashedString tag);
        HashedString getCurrentAnimationName();

        SceneNode* findNode(HashedString name);
        ///SceneNode* findNode(int id);

        ///int getNodeState(int nodeId);
        HashedString getNodeState(HashedString nodeName);

        virtual void update(const Time &elapsedTime = Time(0), uint32_t localTime = -1);
        ///virtual void rewind(uint32_t time);

    protected:
        void copyFromModel(SkeletonModelAsset *model);

        void nextAnimation();

        void loadAnimationCommands(SkeletalAnimationFrameModel *frame, float curFrameTime = 0);

    protected:
        //SceneNode *m_rootNode;
        std::unordered_map<HashedString, SceneNode*> m_nodesByName;
        ///std::unordered_map<int, SceneNode*> m_nodesById;
        std::unordered_map<SceneNode*, SkeletalNodeState> m_nodeStates; // Could pack this into m_nodesByName for better perfs

        ///std::multimap<std::pair<int,int>, std::shared_ptr<SceneObject> > m_limbsPerNodeState;
        std::multimap<std::pair<HashedString,HashedString>, std::shared_ptr<SceneObject> > m_limbsPerNodeState;
        ///std::unordered_multimap<std::pair<HashedString,HashedString>, std::shared_ptr<SceneObject>, IntPairHash> m_limbsPerNodeState;
        std::unordered_map<HashedString, HashedString> m_nodesLastState;

    private:
        SkeletonModelAsset *m_model;

        SkeletalAnimationModel *m_nextAnimation, *m_curAnimation;
        SkeletalAnimationFrameModel *m_curAnimationFrame;
        bool m_forceNewAnimation;
        //bool m_isNewFrame;

        std::list<SkeletalAnimationCommand> m_animationCommands;
        //bool m_createdRootNode;

        /**SyncAttribute<float> m_curFrameTime;
        SyncAttribute<int> m_syncedAnimationId;
        SyncAttribute<int> m_syncedFrameNbr;**/
        float m_curFrameTime;
        int m_frameNbr;
        int m_wantedFrameNbr;
};

}

#endif // SKELETON_H
