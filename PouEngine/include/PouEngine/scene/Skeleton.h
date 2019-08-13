#ifndef SKELETON_H
#define SKELETON_H

#include "PouEngine/assets/SkeletonModelAsset.h"
#include "PouEngine/scene/SceneNode.h"

namespace pou
{

class Skeleton
{
    public:
        Skeleton(SceneNode *rootNode, SkeletonModelAsset *model);
        virtual ~Skeleton();

        bool attachLimb(const std::string &boneName, SceneObject *object);

    protected:
        void copyFromModel(SkeletonModelAsset *model);

    protected:
        SceneNode *m_rootNode;
        std::map<std::string, SceneNode*> m_nodesByName;

    private:
        bool m_createdRootNode;
};

}

#endif // SKELETON_H
