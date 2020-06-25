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

        bool attachLimb(const std::string &boneName, const std::string &stateName, std::shared_ptr<SceneObject> object);
        bool detachLimb(const std::string &boneName, const std::string &stateName, SceneObject *object);

        void attachLimbsOfState(int nodeId, int stateId);
        void detachLimbsOfDifferentState(int nodeId, int stateId);
        /*void attachLimbsOfState(const std::string &boneName, const std::string &stateName);
        void detachLimbsOfDifferentState(const std::string &boneName, const std::string &stateName);*/
        //bool detachAllLimbs(const std::string &boneName);

        bool attachSound(std::shared_ptr<SoundObject> object, const std::string &soundName);
        bool detachSound(SoundObject *object, const std::string &soundName);

        bool startAnimation(const std::string &animationName, bool forceStart = false);
        bool startAnimation(int animationId, bool forceStart = false);
        //could add pause animation etc

        bool isInAnimation();
        bool isNewFrame();
        bool hasTag(const std::string &tag);
        std::pair <std::multimap<std::string, FrameTag>::iterator, std::multimap<std::string, FrameTag>::iterator>
            getTagValues(const std::string &tag);
        const std::string &getCurrentAnimationName();

        const SceneNode* findNode(const std::string &name) const;
        const SceneNode* findNode(int id) const;

        int getNodeState(int nodeId);

        virtual void update(const Time &elapsedTime, uint32_t localTime = -1);
        ///virtual void rewind(uint32_t time);

    protected:
        void copyFromModel(SkeletonModelAsset *model);

        void nextAnimation();

        void loadAnimationCommands(SkeletalAnimationFrameModel *frame, float curFrameTime = 0);

    protected:
        //SceneNode *m_rootNode;
        std::map<std::string, SceneNode*> m_nodesByName;
        std::map<int, SceneNode*> m_nodesById;
        std::map<SceneNode*, SkeletalNodeState> m_nodeStates; // Could pack this into m_nodesById for better perfs

        std::multimap<std::pair<int,int>, std::shared_ptr<SceneObject> > m_limbsPerNodeState;
        std::map<int, int> m_nodesLastState;

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
