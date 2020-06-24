#include "PouEngine/system/SimpleNode.h"

#include "PouEngine/tools/Logger.h"

#include <iostream>

namespace pou
{

const glm::vec3 SimpleNode::NODE_MAX_POS         = glm::vec3(50000.0, 50000.0, 1000.0);
const float     SimpleNode::NODE_MAX_SCALE       = 100.0f;
const uint8_t   SimpleNode::NODE_SCALE_DECIMALS  = 2;


SimpleNode::SimpleNode(/**const NodeTypeId id**/) :
    m_globalPosition(glm::vec3(0)),
    m_position(glm::vec3(0), 0),
    m_eulerRotations(glm::vec3(0),0),
    m_scale(glm::vec3(1.0),0),
    m_rigidity(1.0),
    m_curFlexibleLength(0.0),
    m_curFlexibleRotation(0.0),
    m_modelMatrix(1.0),
    m_invModelMatrix(1.0),
    m_curLocalTime(0),
    m_lastSyncTime(-1),
    m_lastUpdateTime(-1),
    m_lastParentUpdateTime(-1),
    m_disableRotationSync(false),
    m_disableSync(false)
{
    m_parent = nullptr;
    ///m_id = id;
    ///m_curNewId = 0;
    m_needToUpdateModelMat = true;

    ///m_eulerRotations.setModuloRange(-glm::vec3(glm::pi<float>()), glm::vec3(glm::pi<float>()));

}

SimpleNode::~SimpleNode()
{
    this->destroy();
    /**if(m_parent != nullptr)
        m_parent->removeChildNode(this);
    this->removeAndDestroyAllChilds();**/
}

void SimpleNode::destroy()
{
    if(m_parent)
        m_parent->removeChildNode(this);
    this->removeAllChilds();
    m_parent = nullptr;
}

void SimpleNode::addChildNode(std::shared_ptr<SimpleNode> childNode)
{
    if(this->containsChildNode(childNode))
        return;

    m_childs.push_back(childNode);
    childNode->setParent(this);
}

bool SimpleNode::removeChildNode(std::shared_ptr<SimpleNode> childNode)
{
    return this->removeChildNode(childNode.get());
}

bool SimpleNode::removeChildNode(SimpleNode *childNode)
{
    for(auto childIt = m_childs.begin() ; childIt != m_childs.end() ; ++childIt)
        if(childIt->get() == childNode)
        {
            (*childIt)->setParent(nullptr);
            m_childs.erase(childIt);
            return (true);
        }
    return (false);
}

void SimpleNode::removeAllChilds()
{
    for(auto &child : m_childs)
     //   if(child)
            child->setParent(nullptr);

    m_childs.clear();
}

void SimpleNode::removeFromParent()
{
    if(m_parent)
        m_parent->removeChildNode(this);
}

std::shared_ptr<SimpleNode> SimpleNode::createChildNode(float x, float y, float z)
{
    auto newNode = this->createChildNode();
    if(newNode)
        newNode->setPosition(x,y,z);
    return newNode;
}


std::shared_ptr<SimpleNode> SimpleNode::createChildNode(float x, float y)
{
    return this->createChildNode(x,y,0);
}

std::shared_ptr<SimpleNode> SimpleNode::createChildNode(glm::vec2 p)
{
    return this->createChildNode(p.x, p.y);
}

std::shared_ptr<SimpleNode> SimpleNode::createChildNode(glm::vec3 p)
{
    return this->createChildNode(p.x, p.y, p.z);
}

std::shared_ptr<SimpleNode> SimpleNode::createChildNode()
{
    std::shared_ptr<SimpleNode> newNode = this->nodeAllocator();
    this->addChildNode(newNode);

    return newNode;
}

bool SimpleNode::containsChildNode(std::shared_ptr<SimpleNode> childNode)
{
    for(auto &child : m_childs)
        if(child == childNode)
            return (true);
    return (false);
}

/**void SimpleNode::addChildNode(SimpleNode* st)
{
    NodeTypeId id = this->generateId();
    this->addChildNode(id, node);
    if(node != nullptr)
        node->setId(id);
}

void SimpleNode::addChildNode(const NodeTypeId id, SimpleNode* node)
{
    auto childsIt = m_childs.find(id);
    if(childsIt != m_childs.end())
   // if(this->getChild(id) == nullptr)
    {
        std::ostringstream warn_report;
        warn_report << "Adding child node of same id as another one (Id="<<id<<")";
        Logger::warning(warn_report);
    }

    if(m_curNewId <= (int)id)
        m_curNewId = id+1;

    m_childs.insert_or_assign(childsIt,id,node);
    node->setParent(this);
}

void SimpleNode::addCreatedChildNode(SimpleNode* node)
{
    NodeTypeId id = this->generateId();
    this->addChildNode(id, node);
    if(node != nullptr)
    {
        node->setId(id);
        m_createdChildsList.insert(id);
    }
}

void SimpleNode::moveChildNode(SimpleNode* node, SimpleNode* target)
{
    if(node != nullptr && node->getParent() == this)
        this->moveChildNode(node->getId(), target);
}

void SimpleNode::moveChildNode(const NodeTypeId id, SimpleNode* target)
{
    auto node = this->getChild(id);
    auto createdChildsIt = m_createdChildsList.find(id);

    if(createdChildsIt != m_createdChildsList.end())
    {
        m_createdChildsList.erase(createdChildsIt);
        target->addCreatedChildNode(node);
    } else
        target->addChildNode(node);
}

SimpleNode* SimpleNode::removeChildNode(const NodeTypeId id)
{
    SimpleNode* node = nullptr;

    auto childsIt = m_childs.find(id);

    if(childsIt == m_childs.end())
    {
        std::ostringstream error_report;
        error_report << "Cannot remove child node (Id="<<id<<")";
        Logger::error(error_report);

        return (nullptr);
    }


    auto createdChildsIt = m_createdChildsList.find(id);
    if(createdChildsIt != m_createdChildsList.end())
        m_createdChildsList.erase(createdChildsIt);

    node = childsIt->second;
    node->setParent(nullptr);

    m_childs.erase(childsIt);

    return node;
}

SimpleNode* SimpleNode::removeChildNode(SimpleNode* node)
{
    if(node != nullptr && node->getParent() == this)
        return this->removeChildNode(node->getId());
    return (nullptr);
}

SimpleNode* SimpleNode::createChildNode()
{
    return this->createChildNode(this->generateId());
}

SimpleNode* SimpleNode::createChildNode(float x, float y, float z)
{
    SimpleNode* newNode = this->createChildNode();
    if(newNode != nullptr)
        newNode->setPosition(x,y,z);
    return newNode;
}


SimpleNode* SimpleNode::createChildNode(float x, float y)
{
    return this->createChildNode(x,y,0);
}

SimpleNode* SimpleNode::createChildNode(glm::vec2 p)
{
    return this->createChildNode(p.x, p.y);
}

SimpleNode* SimpleNode::createChildNode(glm::vec3 p)
{
    return this->createChildNode(p.x, p.y, p.z);
}

SimpleNode* SimpleNode::createChildNode(const NodeTypeId id)
{
    if(static_cast<unsigned int>(m_curNewId) <= id)
        m_curNewId = id+1;

    if(this->getChild(id) != nullptr)
    {
        std::ostringstream error_report;
        error_report << "Cannot create new child node with the same Id as an existing child node (Id="<<id<<")";
        Logger::error(error_report);

        return (nullptr); //childsIt->second;
    }

    SimpleNode* newNode = this->nodeAllocator(id);
    m_createdChildsList.insert(id);

    this->addChildNode(id, newNode);

    return newNode;
}

bool SimpleNode::destroyChildNode(SimpleNode* node)
{
    if(node != nullptr && node->getParent() == this)
        return this->destroyChildNode(node->getId());
    return (false);
}

bool SimpleNode::destroyChildNode(const NodeTypeId id)
{
    auto createdChildsIt = m_createdChildsList.find(id);

    if(createdChildsIt == m_createdChildsList.end())
        Logger::warning("Destroying non-created child");
    else
        m_createdChildsList.erase(createdChildsIt);


    //SimpleNode* child = this->getChild(id);
    auto childsIt = m_childs.find(id);

    if(childsIt == m_childs.end())
    //if(child == nullptr)
    {
        std::ostringstream error_report;
        error_report << "Cannot destroy child (Id="<<id<<")";
        Logger::error(error_report);

        return (false);
    }

    if(childsIt->second != nullptr)
    {
        childsIt->second->setParent(nullptr);
        delete childsIt->second;
    }

    m_childs.erase(childsIt);

    return (true);
}

void SimpleNode::removeAndDestroyAllChilds(bool destroyNonCreatedChilds)
{
    if(!destroyNonCreatedChilds)
        while(!m_createdChildsList.empty())
            this->destroyChildNode(*m_createdChildsList.begin());

    if(destroyNonCreatedChilds)
    {
        for(auto childsIt : m_childs)
        {
            if(childsIt.second != nullptr)
            {
                childsIt.second->removeAndDestroyAllChilds(destroyNonCreatedChilds);
                delete childsIt.second;
            }
        }
    }

    m_childs.clear();
    m_createdChildsList.clear();
}**/

void SimpleNode::copyFrom(const SimpleNode* srcNode)
{
    if(srcNode == nullptr)
        return;

    this->setPosition(srcNode->getPosition());
    this->setRotation(srcNode->getEulerRotation());
    this->setScale(srcNode->getScale());
    this->setName(srcNode->getName());
    this->setRigidity(srcNode->getRigidity());

    for(auto child : srcNode->m_childs)
    {
        auto newNode = this->createChildNode();
        if(newNode != nullptr)
            newNode->copyFrom(child.get());
    }
}

void SimpleNode::syncFromNode(SimpleNode* srcNode)
{
    //if(m_lastSyncTime > srcNode->getLastUpdateTime() && m_lastSyncTime != (uint32_t)(-1))
      //  return (false);

    if(m_disableSync)
        return;

    m_position.syncFrom(srcNode->m_position);
    if(!m_disableRotationSync)
        m_eulerRotations.syncFrom(srcNode->m_eulerRotations);
    m_scale.syncFrom(srcNode->m_scale);

    m_lastSyncTime = srcNode->m_curLocalTime;

    //return (true);
}

/*void SimpleNode::setLocalTime(uint32_t localTime)
{
    //if(localTime < 0)
        //localTime = -1;
    m_curLocalTime = localTime;
    this->update();
}*/

void SimpleNode::setReconciliationDelay(uint32_t serverDelay, uint32_t clientDelay)
{
    m_position.setReconciliationDelay(serverDelay, clientDelay);
    m_eulerRotations.setReconciliationDelay(serverDelay, clientDelay);
    m_scale.setReconciliationDelay(serverDelay, clientDelay);
}


void SimpleNode::setMaxRewind(int maxRewind)
{
    m_position.setMaxRewindAmount(maxRewind);
    m_eulerRotations.setMaxRewindAmount(maxRewind);
    m_scale.setMaxRewindAmount(maxRewind);
}

void SimpleNode::disableRotationSync(bool disable)
{
    m_disableRotationSync = disable;
}

void SimpleNode::disableSync(bool disable)
{
    m_disableSync = disable;
}

void SimpleNode::move(float x, float y)
{
    this->move(x,y,0);
}

void SimpleNode::move(float x, float y, float z)
{
    this->move(glm::vec3(x,y,z));
}

void SimpleNode::move(glm::vec2 p)
{
    this->move(glm::vec3(p.x,p.y,0));
}

void SimpleNode::move(glm::vec3 p)
{
    /*for(auto child : m_childs)
        child.second->move((child.second->getRigidity()-1.0f)*p);*/

    for(auto child : m_childs)
        child->computeFlexibleMove(p);

    this->setPosition(this->getPosition()+p);
}

void SimpleNode::setPosition(float x, float y)
{
    this->setPosition(glm::vec2(x,y));
}

void SimpleNode::setPosition(float x, float y, float z)
{
    this->setPosition(glm::vec3(x, y, z));
}

void SimpleNode::setPosition(glm::vec2 xyPos)
{
    this->setPosition(glm::vec3(xyPos.x, xyPos.y,this->getPosition().z));
}

void SimpleNode::setPosition(glm::vec3 pos)
{
    m_position.setValue(pos);
    //m_lastPositionUpdateTime = m_curLocalTime;
    this->setLastUpdateTime(m_curLocalTime);

    ///this->updateGlobalPosition();
    this->askForUpdateModelMatrix();
}


void SimpleNode::setGlobalPosition(float x, float y)
{
    this->setGlobalPosition(glm::vec2(x,y));
}

void SimpleNode::setGlobalPosition(float x, float y, float z)
{
    this->setGlobalPosition(glm::vec3(x, y, z));
}

void SimpleNode::setGlobalPosition(glm::vec2 xyPos)
{
    this->setGlobalPosition(glm::vec3(xyPos.x, xyPos.y,this->getGlobalPosition().z));
}

void SimpleNode::setGlobalPosition(glm::vec3 pos)
{
    if(m_parent != nullptr)
    {
        glm::vec4 newPos = m_parent->getInvModelMatrix() * glm::vec4(pos,1.0);
        this->setPosition({newPos.x, newPos.y, newPos.z});
    }
    else
        this->setPosition(pos);

    //this->updateGlobalPosition();
    this->askForUpdateModelMatrix();
}

void SimpleNode::scale(float scale)
{
    this->scale({scale, scale, scale});
}

void SimpleNode::scale(glm::vec3 scale)
{
    this->setScale(m_scale.getValue()*scale);
}

void SimpleNode::linearScale(float x, float y, float z)
{
    this->linearScale(glm::vec3(x,y,z));
}

void SimpleNode::linearScale(glm::vec3 scale)
{
    this->setScale(m_scale.getValue()+scale);
}

void SimpleNode::setScale(float scale)
{
    this->setScale({scale, scale, scale});
}

void SimpleNode::setScale(glm::vec3 scale)
{
    m_scale.setValue(scale);
    //m_lastScaleUpdateTime = m_curLocalTime;
    this->setLastUpdateTime(m_curLocalTime);
    this->askForUpdateModelMatrix();
}

void SimpleNode::rotate(float value, glm::vec3 axis, bool inRadians)
{
    this->rotate(value*axis, inRadians);
}

void SimpleNode::rotate(glm::vec3 values, bool inRadians)
{
    if(!inRadians)
        values = glm::pi<float>()/180.0f*values;

    /*for(auto child : m_childs)
        child.second->computeFlexibleRotate(values.z);*/

    this->setRotation(m_eulerRotations.getValue()+values);
}


void SimpleNode::setRotation(glm::vec3 rotation, bool inRadians)
{
    if(!inRadians)
    {
        rotation.x = rotation.x*glm::pi<float>()/180.0;
        rotation.y = rotation.y*glm::pi<float>()/180.0;
        rotation.z = rotation.z*glm::pi<float>()/180.0;
    }

    auto eulerRotations = glm::mod(rotation+glm::pi<float>()*glm::vec3(1.0),glm::pi<float>()*2)-glm::pi<float>()*glm::vec3(1.0);
    m_eulerRotations.setValue(eulerRotations);
    //m_lastRotationUpdateTime = m_curLocalTime;
    this->setLastUpdateTime(m_curLocalTime);

    //this->updateModelMatrix();
    this->askForUpdateModelMatrix();
}

void SimpleNode::setName(const std::string &name)
{
    m_name = name;
}

void SimpleNode::setRigidity(float rigidity)
{
    m_rigidity = rigidity;
}

glm::vec3 SimpleNode::getPosition() const
{
    return m_position.getValue();
}

glm::vec2 SimpleNode::getXYPosition() const
{
    return glm::vec2(m_position.getValue());
}

glm::vec3 SimpleNode::getGlobalPosition() const
{
    return m_globalPosition;
}

glm::vec2 SimpleNode::getGlobalXYPosition() const
{
    return glm::vec2(m_globalPosition.x, m_globalPosition.y);
}


glm::vec3 SimpleNode::getScale() const
{
    return m_scale.getValue();
}

glm::vec3 SimpleNode::getEulerRotation() const
{
    return m_eulerRotations.getValue();
}

const glm::mat4 &SimpleNode::getModelMatrix() const
{
    return m_modelMatrix;
}

const glm::mat4 &SimpleNode::getInvModelMatrix() const
{
    return m_invModelMatrix;
}

/**NodeTypeId SimpleNode::getId() const
{
    return m_id;
}**/

const std::string &SimpleNode::getName() const
{
    return m_name;
}

float SimpleNode::getRigidity() const
{
    return m_rigidity;
}

SimpleNode* SimpleNode::getParent()
{
    return m_parent;
}

/**SimpleNode* SimpleNode::getChild(const NodeTypeId id)
{
    auto childsIt = m_childs.find(id);
    if(childsIt == m_childs.end())
        return (nullptr);
    return childsIt->second;
}**/

SimpleNode* SimpleNode::getChildByName(const std::string &name, bool recursiveSearch)
{
    for(auto child : m_childs)
    {
        if(child->getName() == name)
            return child.get();

        if(recursiveSearch)
        {
            auto res = child->getChildByName(name,true);
            if(res != nullptr)
                return res;
        }
    }
    return (nullptr);
}


void SimpleNode::getNodesByName(std::map<std::string, SimpleNode*> &namesAndResMap)
{
    if(!m_name.empty())
    {
        auto res = namesAndResMap.find(m_name);
        if(res != namesAndResMap.end())
            namesAndResMap.insert_or_assign(res, m_name, this);
    }

    for(auto child : m_childs)
        child->getNodesByName(namesAndResMap);
}


void SimpleNode::setLastUpdateTime(uint32_t time, bool force)
{
    if(force || m_lastUpdateTime == (uint32_t)(-1) || m_lastUpdateTime < time)
        m_lastUpdateTime = time;
}

uint32_t SimpleNode::getLastUpdateTime()
{
    return m_lastUpdateTime;
}

uint32_t SimpleNode::getLastParentUpdateTime()
{
    return m_lastParentUpdateTime;
}

uint32_t SimpleNode::getLocalTime()
{
    return m_curLocalTime;
}

/*std::list<SimpleNode*> SimpleNode::getAllChilds()
{
    return m_childs.
}*/


/**void SimpleNode::setId(const NodeTypeId id)
{
    m_id = id;
}**/

void SimpleNode::setParent(SimpleNode *p)
{
    if(m_parent != p)
    {
        this->stopListeningTo(m_parent);

        auto oldParent = m_parent;

        m_parent = p;
        m_lastParentUpdateTime = m_curLocalTime;
        this->setLastUpdateTime(m_curLocalTime);
        if(m_parent != nullptr)
        {
            ///this->setScene(m_parent->getScene());
            this->startListeningTo(m_parent);
        }

        ///this->updateGlobalPosition();
        this->askForUpdateModelMatrix();

        if(oldParent != nullptr && p != nullptr)
            oldParent->removeChildNode(this);
    }
}

std::shared_ptr<SimpleNode> SimpleNode::nodeAllocator(/**NodeTypeId id**/)
{
    return std::make_shared<SimpleNode>(/**id**/);
}

/**NodeTypeId SimpleNode::generateId()
{
    return m_curNewId++;
}**/


void SimpleNode::update(const Time &elapsedTime, uint32_t localTime)
{
    if(localTime != (uint32_t)(-1))
        m_curLocalTime = localTime;

    bool syncUpdate = false;

    syncUpdate |= m_position.update(elapsedTime, m_curLocalTime);
    syncUpdate |= m_eulerRotations.update(elapsedTime, m_curLocalTime);
    syncUpdate |= m_scale.update(elapsedTime, m_curLocalTime);

    if(syncUpdate)
        this->setLastUpdateTime(m_curLocalTime);

    syncUpdate |= syncUpdate;

    //std::cout<<"Update SimpleNode: "<<this<<std::endl;
    if(m_needToUpdateModelMat)
        this->updateModelMatrix();


    for(auto node : m_childs)
        node->update(elapsedTime,localTime);
}

/**void SimpleNode::rewind(uint32_t time)
{
    m_position.rewind(time);
    m_eulerRotations.rewind(time);
    m_scale.rewind(time);

    for(auto node : m_childs)
        node.second->rewind(time);
}**/

void SimpleNode::updateGlobalPosition()
{
    if(m_parent != nullptr)
    {
        if(m_rigidity != 1.0)
        {
            m_globalPosition = m_parent->getGlobalPosition() + m_position.getValue();
        }
        else
        {
            glm::vec4 pos    = m_parent->getModelMatrix() * glm::vec4(m_position.getValue(),1.0);
            m_globalPosition = {pos.x,pos.y,pos.z};
        }
    }
    else
        m_globalPosition = m_position.getValue();

    //this->updateModelMatrix();
    ///this->askForUpdateModelMatrix();
}

void SimpleNode::askForUpdateModelMatrix()
{
    m_needToUpdateModelMat = true;

}

void SimpleNode::updateModelMatrix()
{
    glm::vec3 oldGlobalPosition = m_globalPosition;

    this->updateGlobalPosition();

    if(m_rigidity != 1.0 && m_parent != nullptr)
        m_modelMatrix = glm::translate(glm::mat4(1.0), m_parent->getGlobalPosition());
    else
        m_modelMatrix = glm::mat4(1.0);


    m_modelMatrix = glm::translate(m_modelMatrix, m_position.getValue());
    m_modelMatrix = glm::rotate(m_modelMatrix, m_eulerRotations.getValue().x, glm::vec3(1.0,0.0,0.0));
    m_modelMatrix = glm::rotate(m_modelMatrix, m_eulerRotations.getValue().y, glm::vec3(0.0,1.0,0.0));
    m_modelMatrix = glm::rotate(m_modelMatrix, m_eulerRotations.getValue().z, glm::vec3(0.0,0.0,1.0));
    m_modelMatrix = glm::scale(m_modelMatrix, m_scale.getValue());

    if(m_rigidity == 1.0 && m_parent != nullptr)
    {
        m_modelMatrix       = m_parent->getModelMatrix() * m_modelMatrix;
      //  m_invModelMatrix    = m_parent->getInvModelMatrix();
    }// else
        m_invModelMatrix = glm::mat4(1.0);

    m_invModelMatrix = glm::scale(m_invModelMatrix, 1.0f/m_scale.getValue());
    m_invModelMatrix = glm::rotate(m_invModelMatrix, -m_eulerRotations.getValue().z, glm::vec3(0.0,0.0,1.0));
    m_invModelMatrix = glm::rotate(m_invModelMatrix, -m_eulerRotations.getValue().y, glm::vec3(0.0,1.0,0.0));
    m_invModelMatrix = glm::rotate(m_invModelMatrix, -m_eulerRotations.getValue().x, glm::vec3(1.0,0.0,0.0));
    m_invModelMatrix = glm::translate(m_invModelMatrix, -m_position.getValue());

    if(m_rigidity != 1.0 && m_parent != nullptr)
        m_invModelMatrix = glm::translate(m_invModelMatrix, -m_parent->getGlobalPosition());

    if(m_rigidity == 1.0 && m_parent != nullptr)
        m_invModelMatrix = m_invModelMatrix * m_parent->getInvModelMatrix();

    m_needToUpdateModelMat = false;
    this->sendNotification(NotificationType_NodeMoved,&oldGlobalPosition);
}


/*{
if(m_rigidity != 1.0 && m_parent != nullptr)
        m_modelMatrix = glm::translate(glm::mat4(1.0), m_parent->getGlobalPosition());
    else
        m_modelMatrix = glm::mat4(1.0);


    m_modelMatrix = glm::translate(m_modelMatrix, m_position);
    m_modelMatrix = glm::rotate(m_modelMatrix, m_eulerRotations.x, glm::vec3(1.0,0.0,0.0));
    m_modelMatrix = glm::rotate(m_modelMatrix, m_eulerRotations.y, glm::vec3(0.0,1.0,0.0));
    m_modelMatrix = glm::rotate(m_modelMatrix, m_eulerRotations.z, glm::vec3(0.0,0.0,1.0));
    m_modelMatrix = glm::scale(m_modelMatrix, m_scale);

    if(m_rigidity == 1.0 && m_parent != nullptr)
    {
        m_modelMatrix       = m_parent->getModelMatrix() * m_modelMatrix;
        m_invModelMatrix    = m_parent->getInvModelMatrix();
    } else
        m_invModelMatrix = glm::mat4(1.0);

    m_invModelMatrix = glm::scale(m_invModelMatrix, 1.0f/m_scale);
    m_invModelMatrix = glm::rotate(m_invModelMatrix, -m_eulerRotations.z, glm::vec3(0.0,0.0,1.0));
    m_invModelMatrix = glm::rotate(m_invModelMatrix, -m_eulerRotations.y, glm::vec3(0.0,1.0,0.0));
    m_invModelMatrix = glm::rotate(m_invModelMatrix, -m_eulerRotations.x, glm::vec3(1.0,0.0,0.0));
    m_invModelMatrix = glm::translate(m_invModelMatrix, -m_position);

    if(m_rigidity != 1.0 && m_parent != nullptr)
        m_invModelMatrix = glm::translate(m_invModelMatrix, -m_parent->getGlobalPosition());

    m_needToUpdateModelMat = false;
    this->sendNotification(NotificationType_NodeMoved);
}*/


void SimpleNode::computeFlexibleMove(glm::vec3 m)
{
    ///Need to add partial rigidity
    if(m_rigidity != 1.0)
    {
        glm::vec2 p(m_position.getValue());
        float l = glm::length(p);
        glm::vec2 np = l*glm::normalize(p-glm::vec2(m.x,m.y));
        //float dot = glm::dot(p,np);

        float wantedRotation = glm::atan(np.y,np.x)-glm::pi<float>()*0.5;
        this->rotate(wantedRotation-m_curFlexibleRotation,{0,0,1});
        m_curFlexibleRotation = wantedRotation;

        glm::vec3 mp = m+glm::vec3(np.x-p.x,np.y-p.y,0);
        for(auto child : m_childs)
            child->computeFlexibleMove(mp);

        this->setPosition(np);

    } else
        for(auto child : m_childs)
            child->computeFlexibleMove(m);
}

/*void SimpleNode::computeFlexibleRotate(float rot)
{
    if(m_rigidity != 1.0)
    {
        float c = glm::cos(rot);
        float s = glm::sin(rot);

        glm::vec2 p(m_position.x,m_position.y);
        glm::vec2 np(p.x*c - p.y*s, p.x*s + p.y*c);
        auto v = np-p;

        this->computeFlexibleMove({v.x,v.y,0.0f});
    } else
        for(auto child : m_childs)
            child.second->computeFlexibleRotate(rot);
}*/

void SimpleNode::notify(NotificationSender* sender, int notificationType,
                        void* data)
{
    if(sender == m_parent)
    {
        if(notificationType == NotificationType_NodeMoved)
        {
            ///this->updateGlobalPosition();
            this->askForUpdateModelMatrix();
        }
        if(notificationType == NotificationType_SenderDestroyed)
        {
            m_parent = nullptr;
            ///this->updateGlobalPosition();
            this->askForUpdateModelMatrix();
        }
    }
}

void SimpleNode::serializeNode(Stream *stream, uint32_t clientTime)
{
    bool hasPos = false;
    if(!stream->isReading() && uint32less(clientTime,m_position.getLastUpdateTime()))
        hasPos = true;
    stream->serializeBool(hasPos);
    if(hasPos)
    {
        glm::vec3 pos = m_position.getValue();
        stream->serializeFloat(pos.x, -SimpleNode::NODE_MAX_POS.x, SimpleNode::NODE_MAX_POS.x, 0);
        stream->serializeFloat(pos.y, -SimpleNode::NODE_MAX_POS.y, SimpleNode::NODE_MAX_POS.y, 0);
        stream->serializeFloat(pos.z, -SimpleNode::NODE_MAX_POS.z, SimpleNode::NODE_MAX_POS.z, 2);

        if(stream->isReading())
            this->setPosition(pos);
    }

    bool hasRot = false;
    if(!stream->isReading() && uint32less(clientTime,m_eulerRotations.getLastUpdateTime()) && !m_disableRotationSync)
        hasRot = true;
    stream->serializeBool(hasRot);
    if(hasRot)
    {
        glm::vec3 rot = m_eulerRotations.getValue();
        stream->serializeFloat(rot.x, -3.14, 3.14, 2);
        stream->serializeFloat(rot.y, -3.14, 3.14, 2);
        stream->serializeFloat(rot.z, -3.14, 3.14, 2);
        //stream->serializeFloat(rot.z, -glm::pi<float>(), glm::pi<float>(), 2);
        //stream->serializeFloat(rot.z);

        if(stream->isReading())
            this->setRotation(rot);
    }

    bool hasScale = false;
    if(!stream->isReading() && uint32less(clientTime,m_scale.getLastUpdateTime()))
        hasScale = true;
    stream->serializeBool(hasScale);
    if(hasScale)
    {
        glm::vec3 scale = m_scale.getValue();
        stream->serializeFloat(scale.x, -SimpleNode::NODE_MAX_SCALE, SimpleNode::NODE_MAX_SCALE, SimpleNode::NODE_SCALE_DECIMALS);
        stream->serializeFloat(scale.y, -SimpleNode::NODE_MAX_SCALE, SimpleNode::NODE_MAX_SCALE, SimpleNode::NODE_SCALE_DECIMALS);
        stream->serializeFloat(scale.z, -SimpleNode::NODE_MAX_SCALE, SimpleNode::NODE_MAX_SCALE, SimpleNode::NODE_SCALE_DECIMALS);

        if(stream->isReading())
            this->setScale(scale);
    }
}

}
