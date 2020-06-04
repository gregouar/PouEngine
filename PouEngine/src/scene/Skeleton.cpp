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
    SceneNode(-1),
    m_syncedAnimationId(-1,0),
    m_syncedFrameNbr(-1,0)
{
    m_nextAnimation     = nullptr;
    m_curAnimation      = nullptr;
    m_curAnimationFrame = nullptr;
    m_forceNewAnimation = false;

    m_wantedFrameNbr = -1;
    //m_isNewFrame        = false;

    this->copyFromModel(model);
}

Skeleton::~Skeleton()
{
    /*if(m_createdRootNode)
        delete m_rootNode;*/
}

bool Skeleton::attachLimb(const std::string &boneName, const std::string &stateName, SceneObject *object)
{
    auto bone = m_nodesByName.find(boneName);
    if(bone == m_nodesByName.end())
    {
        Logger::warning("Cannot attach limb to bone "+boneName);
        return (false);
    }

    int nodeId      = m_model->getNodeId(boneName);
    int stateId     = m_model->getStateId(stateName);
    int oldStateId  = this->getNodeState(nodeId);

    if(stateName == std::string() ||
       stateId == oldStateId)
        bone->second->attachObject(object);

    m_limbsPerNodeState.insert({{nodeId, stateId}, object});

    return (true);
}

bool Skeleton::detachLimb(const std::string &boneName, const std::string &stateName, SceneObject *object)
{
    auto bone = m_nodesByName.find(boneName);
    if(bone == m_nodesByName.end())
    {
        Logger::warning("Cannot detach limb from bone "+boneName);
        return (false);
    }

    if(object == nullptr)
        return (true);

    if(stateName == std::string())
        bone->second->detachObject(object);
    else
    {
        int nodeId = m_model->getNodeId(boneName);
        int stateId = this->getNodeState(nodeId);
        int detachStateId = m_model->getStateId(stateName);

        if(stateId == detachStateId)
            bone->second->detachObject(object);
    }

    auto limbsPerNodeState = m_limbsPerNodeState.equal_range({m_model->getNodeId(boneName),
                                                             m_model->getStateId(stateName)});
    for(auto it = limbsPerNodeState.first ; it != limbsPerNodeState.second ; ++it)
        if(it->second == object)
        {
            m_limbsPerNodeState.erase(it);
            break;
        }

    return (true);
}

/*void Skeleton::attachLimbsOfState(const std::string &boneName, const std::string &stateName)
{
    int nodeId  = m_model->getNodeId(boneName);
    int stateId = m_model->getStateId(stateName);
    auto node = m_nodesById.find(nodeId);

    if(node == m_nodesById.end())
        return;

    auto limbsPerNodeState = m_limbsPerNodeState.equal_range({nodeId, stateId});
    for(auto limbIt = limbsPerNodeState.first ; limbIt != limbsPerNodeState.second ; ++limbIt)
        node->second->attachObject(limbIt->second);

    m_nodesLastState[nodeId] = stateId;
}


void Skeleton::detachLimbsOfDifferentState(const std::string &boneName, const std::string &stateName)
{
    int nodeId  = m_model->getNodeId(boneName);
    int stateId = m_model->getStateId(stateName);
    auto node = m_nodesById.find(nodeId);

    if(node == m_nodesById.end())
        return;

    int oldStateId = -1;
    auto oldState = m_nodesLastState.find(nodeId);
    if(oldState != m_nodesLastState.end())
        oldStateId = oldState->second;

    if(oldStateId == stateId)
        return;

    auto limbsPerNodeState = m_limbsPerNodeState.equal_range({nodeId, oldStateId});
    for(auto limbIt = limbsPerNodeState.first ; limbIt != limbsPerNodeState.second ; ++limbIt)
        node->second->detachObject(limbIt->second);
}*/

void Skeleton::attachLimbsOfState(int nodeId, int stateId)
{
    auto node = m_nodesById.find(nodeId);

    if(node == m_nodesById.end())
        return;

    int oldStateId = this->getNodeState(nodeId);
    /*-1;
    auto oldState = m_nodesLastState.find(nodeId);
    if(oldState != m_nodesLastState.end())
        oldStateId = oldState->second;*/

    if(oldStateId == stateId)
        return;

    auto limbsPerNodeState = m_limbsPerNodeState.equal_range({nodeId, stateId});
    for(auto limbIt = limbsPerNodeState.first ; limbIt != limbsPerNodeState.second ; ++limbIt)
        node->second->attachObject(limbIt->second);

    m_nodesLastState[nodeId] = stateId;
}


void Skeleton::detachLimbsOfDifferentState(int nodeId, int stateId)
{
    auto node = m_nodesById.find(nodeId);

    if(node == m_nodesById.end())
        return;

    /*int oldStateId = -1;
    auto oldState = m_nodesLastState.find(nodeId);
    if(oldState != m_nodesLastState.end())
        oldStateId = oldState->second;*/

    int oldStateId = this->getNodeState(nodeId);

    if(oldStateId == -1 || oldStateId == stateId)
        return;

    auto limbsPerNodeState = m_limbsPerNodeState.equal_range({nodeId, oldStateId});
    for(auto limbIt = limbsPerNodeState.first ; limbIt != limbsPerNodeState.second ; ++limbIt)
        node->second->detachObject(limbIt->second);
}



/*bool Skeleton::detachAllLimbs(const std::string &boneName)
{
    auto bone = m_nodesByName.find(boneName);
    if(bone == m_nodesByName.end())
    {
        Logger::warning("Cannot detach limbs from bone "+boneName);
        return (false);
    }

    bone->second->detachAllObjects();

    /// Add detach limbsPerNodeState

    return (true);
}*/

bool Skeleton::attachSound(SoundObject *soundObject, const std::string &soundName)
{
    if(m_model == nullptr)
        return (false);

    SceneNode::attachSound(soundObject, m_model->getSoundId(soundName));
    return (true);
}

bool Skeleton::detachSound(SoundObject *object, const std::string &soundName)
{
    if(m_model == nullptr)
        return (false);

    SceneNode::detachSound(object,m_model->getSoundId(soundName));
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
        m_curFrameTime.setValue(0);
        this->nextAnimation();
    }

    return (true);
}

bool Skeleton::startAnimation(int animationId, bool forceStart)
{
    if(m_curAnimation != nullptr && m_curAnimation->getId() == animationId)
        return (false);

    m_nextAnimation = m_model->findAnimation(animationId);

    if(m_nextAnimation == nullptr)
        return (false);

    if(forceStart)
    {
        m_forceNewAnimation = true;
        m_curFrameTime.setValue(0);
        this->nextAnimation();
    }

    return (true);
}

bool Skeleton::isInAnimation()
{
    return (m_curAnimation != nullptr);
}

/*bool Skeleton::isNewFrame()
{
    return m_isNewFrame;
}*/

bool Skeleton::hasTag(const std::string &tag)
{
    if(m_curAnimationFrame == nullptr)
        return (false);

    return m_curAnimationFrame->hasTag(tag);
}

std::pair <std::multimap<std::string, FrameTag>::iterator, std::multimap<std::string, FrameTag>::iterator>
    Skeleton::getTagValues(const std::string &tag)
{
    if(m_curAnimationFrame == nullptr)
        return std::pair <std::multimap<std::string, FrameTag>::iterator, std::multimap<std::string, FrameTag>::iterator>();

    return m_curAnimationFrame->getTagValues(tag);
}


const SceneNode* Skeleton::findNode(const std::string &name) const
{
    auto found = m_nodesByName.find(name);
    if(found == m_nodesByName.end())
        return (nullptr);
    return found->second;
}

const std::string &Skeleton::getCurrentAnimationName()
{
    if(m_curAnimation == nullptr)
        return (pou::emptyString);
    return m_curAnimation->getName();
}

void Skeleton::update(const Time &elapsedTime, uint32_t localTime)
{
    SceneNode::update(elapsedTime, localTime);

    m_syncedAnimationId.update(elapsedTime, localTime);
    m_syncedFrameNbr.update(elapsedTime, localTime);
    m_curFrameTime.update(elapsedTime, localTime);

    //m_isNewFrame = false;
    if(m_wantedFrameNbr == -1)
        return;

    if(m_curAnimation == nullptr && m_nextAnimation != nullptr)
        this->nextAnimation();
    if(!m_curAnimation)
        return;


    if(!m_forceNewAnimation && m_wantedFrameNbr == m_syncedFrameNbr.getValue())
    {
        float speedFactor = 1.0;
        if(m_curAnimationFrame != nullptr)
            speedFactor *= m_curAnimationFrame->getSpeedFactor();
        m_curFrameTime.setValue(m_curFrameTime.getValue() + elapsedTime.count() * speedFactor);

        bool nextFrame = true;

        for(auto cmd = m_animationCommands.begin() ; cmd != m_animationCommands.end() ; ++cmd)
        {
            if(!cmd->update(elapsedTime * speedFactor, localTime))
                nextFrame = false;
        }

        if(nextFrame)
        {
            m_wantedFrameNbr = m_syncedFrameNbr.getValue() + 1;
            m_curFrameTime.setValue(0);
        }
    }

    if(m_wantedFrameNbr != m_syncedFrameNbr.getValue())
    {
        auto wantedFrameNbr = m_wantedFrameNbr;
        if(wantedFrameNbr < m_syncedFrameNbr.getValue())
        {
            //m_syncedFrameNbr.setValue(0);
            m_nextAnimation = m_curAnimation;
            this->nextAnimation();
        }
        m_wantedFrameNbr = wantedFrameNbr;

        while(m_wantedFrameNbr != m_syncedFrameNbr.getValue())
        {
            m_syncedFrameNbr.setValue(m_syncedFrameNbr.getValue()+1);
            auto [nextFrame, isLooping] = m_curAnimation->nextFrame(m_curAnimationFrame);
            m_curAnimationFrame = nextFrame;
            if(isLooping)
            {
                m_syncedFrameNbr.setValue(0);
                m_wantedFrameNbr = 0;
            }
        }

        if(m_curAnimationFrame == nullptr)
            this->nextAnimation();
        else
            this->loadAnimationCommands(m_curAnimationFrame, m_curFrameTime.getValue());
    }

    /*if(nextFrame)
    {
        if(m_curAnimation != nullptr)
        {
            if(m_forceNewAnimation)
                m_curAnimationFrame = nullptr;

            m_curAnimationFrame = m_curAnimation->nextFrame(m_curAnimationFrame);

            if(m_curAnimationFrame == nullptr)
            {
                this->nextAnimation();
            } else {
                m_syncedFrameNbr.setValue(m_syncedFrameNbr.getValue()+1);
                this->loadAnimationCommands(m_curAnimationFrame);
                m_curFrameTime.setValue(0);
                m_isNewFrame = true;
            }

        } else
            this->nextAnimation();
    }*/

    m_forceNewAnimation = false;
}

void Skeleton::rewind(uint32_t time)
{
    SceneNode::rewind(time);

    for(auto& nodeState : m_nodeStates)
        nodeState.second.rewind(time);

    auto oldAnimationId = m_syncedAnimationId.getValue();
    auto oldFrameNbr = m_syncedFrameNbr.getValue();

    m_syncedAnimationId.rewind(time);
    m_syncedFrameNbr.rewind(time);
    m_curFrameTime.rewind(time);

    auto wantedFrameNbr = m_syncedFrameNbr.getValue();
    auto oldFrameTime = m_curFrameTime.getValue();

    if(oldAnimationId != m_syncedAnimationId.getValue())
    {
        //m_wantedFrameNbr = m_syncedFrameNbr.getValue();
        this->startAnimation(m_syncedAnimationId.getValue(), true);
        //m_curAnimation = m_model->findAnimation(m_syncedAnimationId.getValue());
        //m_nextAnimation = nullptr;
    }
    else if(oldFrameNbr != m_syncedFrameNbr.getValue())
    {
        m_syncedFrameNbr.setValue(oldFrameNbr);
        //std::cout<<"RewindFrame from:"<<oldFrameNbr<< " to:"<<m_syncedFrameNbr.getValue()<<std::endl;
        //m_nextAnimation = m_curAnimation;
        //m_wantedFrameNbr = m_syncedFrameNbr.getValue();
    }
    else
    {
        for(auto& command : m_animationCommands)
            command.rewind(time);
    }


    m_wantedFrameNbr = wantedFrameNbr;
    m_curFrameTime.setValue(oldFrameTime);
}

int Skeleton::getNodeState(int nodeId)
{
    int oldStateId = -1;
    auto oldState = m_nodesLastState.find(nodeId);
    if(oldState != m_nodesLastState.end())
        oldStateId = oldState->second;
    return oldStateId;
}

/// Protected ///

void Skeleton::copyFromModel(SkeletonModelAsset *model)
{
    m_model = model;

    SceneNode::copyFrom(model->getRootNode());

    auto nodeNames = model->getNodesByName();
    SceneNode::getNodesByName(nodeNames);

    m_nodesByName.clear();
    m_nodesById.clear();
    for(auto it : nodeNames)
    {
        auto *node = dynamic_cast<SceneNode*>(it.second);
        m_nodesByName.insert({it.first, node});
        m_nodesById.insert({model->getNodeId(it.first), node});
    }

    auto initStates = model->getInitialStates();
    for(auto state : *initStates)
        this->attachLimbsOfState(state.first, state.second);

   // m_nodesByName = m_model->getNodesByName();
   // m_rootNode->getChildsByNames(m_nodesByName, true);
}

void Skeleton::nextAnimation()
{
    m_curAnimation  = m_nextAnimation;
    m_nextAnimation = nullptr;

    if(m_curAnimation)
    {
        m_curAnimationFrame = nullptr;
        m_syncedAnimationId.setValue(m_curAnimation->getId());
        m_syncedFrameNbr.setValue(-1);
        m_wantedFrameNbr = 0;
    }
    else
        m_syncedAnimationId.setValue(-1);
}

void Skeleton::loadAnimationCommands(SkeletalAnimationFrameModel *frame, float curFrameTime)
{
    m_animationCommands.clear();

    auto commands = frame->getCommands();
    for(auto cmd  = commands->begin() ; cmd != commands->end() ; ++cmd)
    {
        //std::string nodeName = cmd->getNode();
        int nodeId = cmd->getNodeId();

        //auto node = m_nodesByName[nodeName]; ///Need to change this so that I dont use name (but some kind of ID ?)
        auto node = m_nodesById[nodeId];
        if(node != nullptr)
            m_animationCommands.push_back(
                SkeletalAnimationCommand (&(*cmd), node, &m_nodeStates[node], curFrameTime));
        //m_animationCommands.back().computeAmount(m_nodeStates[node]);
    }

    auto sounds = frame->getSounds();
    for(const auto sound : *sounds)
        this->playSound(sound);


    auto states = frame->getStates();
    for(const auto state : *states)
    {
        this->detachLimbsOfDifferentState(state.first, state.second);
        this->attachLimbsOfState(state.first, state.second);
    }
}

/**                             **/
/// SkeletalAnimationCommand    ///
/**                             **/

SkeletalAnimationCommand::SkeletalAnimationCommand(const SkeletalAnimationCommandModel *model, SceneNode *node,
                                                   SkeletalNodeState *nodeState, float startingFrameTime) :
    m_model(model),
    m_node(node),
    m_nodeState(nodeState),
    m_value(glm::vec4(0),0),
    m_curFrameTime(startingFrameTime,0),
    m_startingFrameTime(startingFrameTime),
    m_amount(0)
{
    this->computeAmount();
}


void SkeletalAnimationCommand::computeAmount()
{
    m_amount = m_model->getAmount().first;
    m_enabledDirection = m_model->getAmount().second;

    if(m_model->getType() == Move_To)
        m_amount = m_amount - m_nodeState->posisiton.getValue();
    else if(m_model->getType() == Rotate_To)
        m_amount = m_amount - m_nodeState->rotation.getValue();
    else if(m_model->getType() == Scale_To)
        m_amount = m_amount - m_nodeState->scale.getValue();
    else if(m_model->getType() == Color_To)
        m_amount = m_amount - m_nodeState->color.getValue();

    if(m_amount.x == 0) m_enabledDirection.x = false;
    if(m_amount.y == 0) m_enabledDirection.y = false;
    if(m_amount.z == 0) m_enabledDirection.z = false;
    if(m_amount.w == 0) m_enabledDirection.w = false;
}

bool SkeletalAnimationCommand::update(const Time &elapsedTime, uint32_t curTime)
{
    m_value.update(elapsedTime, curTime);
    m_curFrameTime.update(elapsedTime, curTime);
    m_nodeState->update(elapsedTime, curTime);

    m_curFrameTime.setValue(m_curFrameTime.getValue() + elapsedTime.count());

    glm::vec4 a = glm::vec4(elapsedTime.count()); // * m_model->getRate();
    glm::vec4 absAmount = glm::abs(m_amount);

    if(m_model->getRate() > 0)
        a = a * m_model->getRate();
    else
    {
        if(m_curFrameTime.getValue() > m_model->getFrameTime())
            a = absAmount - m_value.getValue();
        else
            a = a * absAmount/(m_model->getFrameTime()-m_startingFrameTime);
    }

    bool finished = true;

    glm::vec4 sign;

    sign.x = (m_amount.x < 0) ? -1 : 1;
    sign.y = (m_amount.y < 0) ? -1 : 1;
    sign.z = (m_amount.z < 0) ? -1 : 1;
    sign.w = (m_amount.w < 0) ? -1 : 1;
    sign = sign * m_enabledDirection;

    glm::vec4 finalAmount(0.0);

    if(m_enabledDirection.x)
    {
        if(m_value.getValue().x + a.x >= absAmount.x)
        {
            finalAmount.x = sign.x * (absAmount.x - m_value.getValue().x);
            m_enabledDirection.x = 0;
        }
        else
        {
            finalAmount.x = sign.x * a.x;
            finished = false;
        }
    }

    if(m_enabledDirection.y)
    {
        if(m_value.getValue().y + a.y >= absAmount.y)
        {
            finalAmount.y = sign.y * (absAmount.y - m_value.getValue().y);
            m_enabledDirection.y = 0;
        }
        else
        {
            finalAmount.y = sign.y * a.y;
            finished = false;
        }
    }


    if(m_enabledDirection.z)
    {
        if(m_value.getValue().z + a.z >= absAmount.z)
        {
            finalAmount.z = sign.z * (absAmount.z - m_value.getValue().z);
            m_enabledDirection.z = 0;
        }
        else
        {
            finalAmount.z = sign.z * a.z;
            finished = false;
        }
    }

    if(m_enabledDirection.w)
    {
        if(m_value.getValue().w + a.w >= absAmount.w)
        {
            finalAmount.w = sign.w * (absAmount.w - m_value.getValue().w);
            m_enabledDirection.w = 0;
        }
        else
        {
            finalAmount.w = sign.w * a.w;
            finished = false;
        }
    }

    m_value.setValue(m_value.getValue() + a);

    if(m_model->getType() == Move_To)
    {
        m_node->move(finalAmount.x,finalAmount.y,finalAmount.z);
        m_nodeState->posisiton.setValue(m_nodeState->posisiton.getValue() + finalAmount);
    }
    else if(m_model->getType() == Rotate_To)
    {
        m_node->rotate(glm::vec3(finalAmount.x,finalAmount.y,finalAmount.z), false);
        m_nodeState->rotation.setValue(m_nodeState->rotation.getValue() + finalAmount);
    }
    else if(m_model->getType() == Scale_To)
    {
        m_node->linearScale(finalAmount.x,finalAmount.y,finalAmount.z);
        m_nodeState->scale.setValue(m_nodeState->scale.getValue() + finalAmount);
    }
    else if(m_model->getType() == Color_To)
    {
        m_node->colorize(finalAmount);
        m_nodeState->color.setValue(m_nodeState->color.getValue() + finalAmount);
    }
    else
        return (true);

    return finished;
}


void SkeletalAnimationCommand::rewind(uint32_t time)
{
    m_value.rewind(time);
    m_curFrameTime.rewind(time);
}

/// ************************  ///

SkeletalNodeState::SkeletalNodeState() :
    posisiton(glm::vec4(0),0),
    rotation(glm::vec4(0),0),
    scale(glm::vec4(0),0),
    color(glm::vec4(0),0)
{

}

void SkeletalNodeState::update(const Time &elapsedTime, uint32_t localTime)
{
    posisiton.update(elapsedTime,localTime);
    rotation.update(elapsedTime,localTime);
    scale.update(elapsedTime,localTime);
    color.update(elapsedTime,localTime);
}

void SkeletalNodeState::rewind(uint32_t time)
{
    posisiton.rewind(time);
    rotation.rewind(time);
    scale.rewind(time);
    color.rewind(time);
}



}
