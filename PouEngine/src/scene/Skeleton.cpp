#include "PouEngine/scene/Skeleton.h"

#include "PouEngine/utils/Logger.h"

namespace pou
{


/*Skeleton::Skeleton(SceneNode *rootNode, SkeletonModelAsset *model)
{
    m_rootNode = rootNode;
    m_createdRootNode = false;
    if(m_rootNode == nullptr)
    {
        m_rootNode = new SceneNode(-1,nullptr);
        m_createdRootNode = true;
    }

    this->copyFromModel(model);
}*/


Skeleton::Skeleton(SkeletonModelAsset *model) :
    SceneNode(-1)
{
    m_nextAnimation     = nullptr;
    m_curAnimation      = nullptr;
    m_curAnimationFrame = nullptr;
    m_forceNewAnimation = false;

    this->copyFromModel(model);
}

Skeleton::~Skeleton()
{
    /*if(m_createdRootNode)
        delete m_rootNode;*/
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

bool Skeleton::startAnimation(const std::string &animationName, bool forceStart)
{
    if(m_curAnimation != nullptr && m_curAnimation->getName() == animationName)
        return (false);

    m_nextAnimation = m_model->findAnimation(animationName);

    if(m_nextAnimation == nullptr)
        return (false);

    if(forceStart)
    {
        m_forceNewAnimation = true;
        m_curAnimation = m_nextAnimation;
        m_nextAnimation = nullptr;
    }

    return (true);
}

bool Skeleton::isInAnimation()
{
    return (m_curAnimation != nullptr);
}

bool Skeleton::hasTag(const std::string &tag)
{
    if(m_curAnimationFrame == nullptr)
        return (false);

    return m_curAnimationFrame->hasTag(tag);
}

const SceneNode* Skeleton::findNode(const std::string &name) const
{
    auto found = m_nodesByName.find(name);
    if(found == m_nodesByName.end())
        return (nullptr);
    return found->second;
}

void Skeleton::update(const Time &elapsedTime)
{
    SceneNode::update(elapsedTime);

    bool nextFrame = true;

    if(!m_forceNewAnimation)
        //for(auto &cmd : m_animationCommands)
        for(auto cmd = m_animationCommands.begin() ; cmd != m_animationCommands.end() ; ++cmd)
        {
            float speedFactor = 1.0;

            if(m_curAnimationFrame != nullptr)
                speedFactor *= m_curAnimationFrame->getSpeedFactor();

            if(!cmd->update(elapsedTime * speedFactor))
                nextFrame = false;
            /*else
            {
                auto c = cmd--;
                m_animationCommands.erase(c);
            }*/
        }

    if(nextFrame)
    {
        if(m_curAnimation != nullptr)
        {
            if(m_forceNewAnimation)
                m_curAnimationFrame = nullptr;

            m_curAnimationFrame = m_curAnimation->nextFrame(m_curAnimationFrame);

            if(m_curAnimationFrame == nullptr)
            {
                m_curAnimation  = m_nextAnimation;
                m_nextAnimation = nullptr;
            } else
                this->loadAnimationCommands(m_curAnimationFrame);

        } else {
            m_curAnimation  = m_nextAnimation;
            m_nextAnimation = nullptr;
        }
    }

    m_forceNewAnimation = false;
}


/// Protected ///

void Skeleton::copyFromModel(SkeletonModelAsset *model)
{
    m_model = model;

    SceneNode::copyFrom(model->getRootNode());

    auto nodeNames = model->getNodesByName();
    SceneNode::getNodesByName(nodeNames);

    m_nodesByName.clear();
    for(auto it : nodeNames)
        m_nodesByName.insert({it.first, dynamic_cast<SceneNode*>(it.second)});

   // m_nodesByName = m_model->getNodesByName();
   // m_rootNode->getChildsByNames(m_nodesByName, true);
}

void Skeleton::loadAnimationCommands(SkeletalAnimationFrameModel *frame)
{
    m_animationCommands.clear();

    auto commands = frame->getCommands();

    for(auto cmd  = commands->begin() ; cmd != commands->end() ; ++cmd)
    {
        std::string nodeName = cmd->getNode();
        auto node = m_nodesByName[nodeName]; ///Need to change this so that I dont use name (but some kind of ID ?)
        if(node != nullptr)
            m_animationCommands.push_back(
                SkeletalAnimationCommand (&(*cmd), node, &m_nodeStates[node]));
        //m_animationCommands.back().computeAmount(m_nodeStates[node]);
    }
}

/**                             **/
/// SkeletalAnimationCommand    ///
/**                             **/


SkeletalAnimationCommand::SkeletalAnimationCommand(const SkeletalAnimationCommandModel *model, SimpleNode *node,
                                                   SkeletalNodeState *nodeState) :
    m_model(model),
    m_node(node),
    m_nodeState(nodeState),
    m_value(0),
    m_amount({0,0,0})
{
    this->computeAmount();
}


void SkeletalAnimationCommand::computeAmount()
{
    m_amount = m_model->getAmount().first;
    m_enabledDirection = m_model->getAmount().second;


    if(m_model->getType() == Move_To)
        m_amount = m_amount - m_nodeState->posisiton;
    else if(m_model->getType() == Rotate_To)
        m_amount = m_amount - m_nodeState->rotation;
    else if(m_model->getType() == Scale_To)
        m_amount = m_amount - m_nodeState->scale;
}

bool SkeletalAnimationCommand::update(const Time &elapsedTime)
{
    float a = elapsedTime.count() * m_model->getRate();
    glm::vec3 absAmount = glm::abs(m_amount);

    bool finished = true;

    glm::vec3 sign;

    sign.x = (m_amount.x < 0) ? -1 : 1;
    sign.y = (m_amount.y < 0) ? -1 : 1;
    sign.z = (m_amount.z < 0) ? -1 : 1;
    sign = sign * m_enabledDirection;

    glm::vec3 finalAmount(0.0);

    if(m_value.x + a >= absAmount.x)
    {
        finalAmount.x = sign.x * (absAmount.x - m_value.x);
        m_enabledDirection.x = 0;
    }
    else
    {
        finalAmount.x = sign.x * a;
        finished = false;
    }

    if(m_value.y + a >= absAmount.y)
    {
        finalAmount.y = sign.y * (absAmount.y - m_value.y);
        m_enabledDirection.y = 0;
    }
    else
    {
        finalAmount.y = sign.y * a;
        finished = false;
    }

    if(m_value.z + a >= absAmount.z)
    {
        finalAmount.z = sign.z * (absAmount.z - m_value.z);
        m_enabledDirection.z = 0;
    }
    else
    {
        finalAmount.z = sign.z * a;
        finished = false;
    }

    m_value += a * glm::vec3(1.0);

    if(m_model->getType() == Move_To)
    {
        m_node->move(finalAmount);
        m_nodeState->posisiton += finalAmount;
    }
    else if(m_model->getType() == Rotate_To)
    {
        m_node->rotate(finalAmount, false);
        m_nodeState->rotation += finalAmount;
    }
    else if(m_model->getType() == Scale_To)
    {
        m_node->linearScale(finalAmount);
        m_nodeState->scale += finalAmount;
    }
    else
        return (true);

    return finished;
}

/// ************************  ///

SkeletalNodeState::SkeletalNodeState() :
    posisiton({0,0,0}),
    rotation({0,0,0}),
    scale({0,0,0})
{

}

}
