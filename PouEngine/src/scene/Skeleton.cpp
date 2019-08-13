#include "PouEngine/scene/Skeleton.h"

#include "PouEngine/utils/Logger.h"

namespace pou
{

Skeleton::Skeleton(SceneNode *rootNode, SkeletonModelAsset *model)
{
    m_rootNode = rootNode;
    m_createdRootNode = false;
    if(m_rootNode == nullptr)
    {
        m_rootNode = new SceneNode(-1,nullptr);
        m_createdRootNode = true;
    }

    this->copyFromModel(model);
}

Skeleton::~Skeleton()
{
    if(m_createdRootNode)
        delete m_rootNode;
}

bool Skeleton::attachLimb(const std::string &boneName, SceneObject *object)
{
    auto bone = m_nodesByName.find(boneName);
    if(bone == m_nodesByName.end())
    {
        Logger::warning("Cannot attach limb to bone "+boneName);
        return (false);
    }

    bone->second->attachObject(object);

    return (true);
}

void Skeleton::copyFromModel(SkeletonModelAsset *model)
{
    m_rootNode->copyFrom(model->getRootNode());

    auto nodeNames = model->getNodesByName();
    m_rootNode->getNodesByName(nodeNames);

    m_nodesByName.clear();
    for(auto it : nodeNames)
        m_nodesByName.insert({it.first, dynamic_cast<SceneNode*>(it.second)});

   // m_nodesByName = m_model->getNodesByName();
   // m_rootNode->getChildsByNames(m_nodesByName, true);
}


}
