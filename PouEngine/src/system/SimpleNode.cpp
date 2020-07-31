#include "PouEngine/system/SimpleNode.h"

#include "PouEngine/tools/Logger.h"

#include <iostream>

namespace pou
{

SimpleNode::SimpleNode() :
    m_globalPosition(glm::vec3(0)),
    m_lastGlobalPosition(0),
    m_position(glm::vec3(0)),
    m_eulerRotations(glm::vec3(0)),
    m_scale(glm::vec3(1.0)),
    m_rigidity(1.0),
    m_curFlexibleLength(0.0),
    m_curFlexibleRotation(0.0),
    m_modelMatrix(1.0),
    m_invModelMatrix(1.0)
{
    m_parent = nullptr;
    m_needToUpdateModelMat = true;

    m_treeDepth = 0;
}

SimpleNode::~SimpleNode()
{
    ///this->destroy();
}

/*void SimpleNode::destroy()
{
   /// if(m_parent)
       /// m_parent->removeChildNode(this);
    //this->removeAllChilds();
    m_parent    = nullptr;
    m_treeDepth = 0;
}*/

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
   // for(auto &child : m_childs)
     //   if(child)
         //  child->setParent(nullptr);

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

void SimpleNode::copyFrom(const SimpleNode* srcNode, bool dontCopyChilds)
{
    if(srcNode == nullptr)
        return;

    this->setPosition(srcNode->getPosition());
    this->setRotation(srcNode->getEulerRotation());
    this->setScale(srcNode->getScale());
    this->setName(srcNode->getName());
    this->setRigidity(srcNode->getRigidity());

    if(dontCopyChilds)
        return;

    for(auto child : srcNode->m_childs)
    {
        auto newNode = this->createChildNode();
        if(newNode != nullptr)
            newNode->copyFrom(child.get());
    }
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
    if(m_position != pos)
        this->askForUpdateModelMatrix();
    m_position = pos;
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
    this->setScale(this->getScale()*scale);
}

void SimpleNode::linearScale(float x, float y, float z)
{
    this->linearScale(glm::vec3(x,y,z));
}

void SimpleNode::linearScale(glm::vec3 scale)
{
    this->setScale(this->getScale()+scale);
}

void SimpleNode::setScale(float scale)
{
    this->setScale({scale, scale, scale});
}

void SimpleNode::setScale(glm::vec3 scale)
{
    m_scale = scale;
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

    this->setRotation(this->getEulerRotation()+values);
}


void SimpleNode::setRotation(glm::vec3 rotation, bool inRadians)
{
    if(!inRadians)
    {
        rotation.x = rotation.x*glm::pi<float>()/180.0;
        rotation.y = rotation.y*glm::pi<float>()/180.0;
        rotation.z = rotation.z*glm::pi<float>()/180.0;
    }

    auto eulerRotations = glm::mod(rotation+glm::pi<float>()*glm::vec3(1.0),
                                   glm::pi<float>()*2)-glm::pi<float>()*glm::vec3(1.0);
    m_eulerRotations = eulerRotations;

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


glm::vec3 SimpleNode::getVelocity() const
{
    return this->getGlobalPosition() - m_lastGlobalPosition;
}

const glm::vec3 &SimpleNode::getPosition() const
{
    return m_position;
}

glm::vec2 SimpleNode::getXYPosition() const
{
    return glm::vec2(this->getPosition());
}

const glm::vec3 &SimpleNode::getGlobalPosition() const
{
    return m_globalPosition;
}

glm::vec2 SimpleNode::getGlobalXYPosition() const
{
    return glm::vec2(m_globalPosition);
}


const glm::vec3 &SimpleNode::getScale() const
{
    return m_scale;
}

const glm::vec3 &SimpleNode::getEulerRotation() const
{
    return m_eulerRotations;
}

const glm::mat4 &SimpleNode::getModelMatrix() const
{
    return m_modelMatrix;
}

const glm::mat4 &SimpleNode::getInvModelMatrix() const
{
    return m_invModelMatrix;
}

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

size_t SimpleNode::getTreeDepth()
{
    return m_treeDepth;
}

bool SimpleNode::setParent(SimpleNode *p)
{
    if(m_parent != p)
    {
        this->stopListeningTo(m_parent);

        auto oldParent = m_parent;

        m_parent = p;
        if(m_parent != nullptr)
            this->startListeningTo(m_parent);

        if(oldParent != nullptr && p != nullptr)
            oldParent->removeChildNode(this);

        this->askForUpdateModelMatrix();

        return (true);
    }
    return (false);
}

bool SimpleNode::setAsParentTo(SimpleNode *parentNode, SimpleNode *childNode)
{
    return childNode->setParent(parentNode);
}

std::shared_ptr<SimpleNode> SimpleNode::nodeAllocator()
{
    return std::make_shared<SimpleNode>();
}


void SimpleNode::update(const Time &elapsedTime, uint32_t localTime)
{
    m_lastGlobalPosition = this->getGlobalPosition();

    if(m_needToUpdateModelMat)
        this->updateModelMatrix();

    for(auto node : m_childs)
        node->update(elapsedTime,localTime);
}

void SimpleNode::updateGlobalPosition()
{
    if(m_parent != nullptr)
    {
        if(m_rigidity != 1.0)
            m_globalPosition = m_parent->getGlobalPosition() + this->getPosition();
        else
        {
            glm::vec4 pos    = m_parent->getModelMatrix() * glm::vec4(this->getPosition(),1.0);
            m_globalPosition = {pos.x,pos.y,pos.z};
        }
        m_treeDepth = m_parent->getTreeDepth() + 1;
    }
    else
    {
        m_globalPosition = this->getPosition();
        m_treeDepth = 0;
    }
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

    m_modelMatrix = glm::translate(m_modelMatrix, this->getPosition());

    m_modelMatrix = glm::rotate(m_modelMatrix, this->getEulerRotation().x, glm::vec3(1.0,0.0,0.0));
    m_modelMatrix = glm::rotate(m_modelMatrix, this->getEulerRotation().y, glm::vec3(0.0,1.0,0.0));
    m_modelMatrix = glm::rotate(m_modelMatrix, this->getEulerRotation().z, glm::vec3(0.0,0.0,1.0));

    m_modelMatrix = glm::scale(m_modelMatrix, this->getScale());

    if(m_rigidity == 1.0 && m_parent != nullptr)
    {
        m_modelMatrix       = m_parent->getModelMatrix() * m_modelMatrix;
      //  m_invModelMatrix    = m_parent->getInvModelMatrix();
    }// else
        m_invModelMatrix = glm::mat4(1.0);

    /*std::cout<<"After:"<<std::endl;
    for(auto x = 0 ; x < 4 ; ++x)
    {
        for(auto y = 0 ; y < 4 ; ++y)
            std::cout<<m_modelMatrix[x][y]<<" ";
        std::cout<<std::endl;
    }
    std::cout<<std::endl<<std::endl;*/

    m_invModelMatrix = glm::scale(m_invModelMatrix, 1.0f/this->getScale());
    m_invModelMatrix = glm::rotate(m_invModelMatrix, -this->getEulerRotation().z, glm::vec3(0.0,0.0,1.0));
    m_invModelMatrix = glm::rotate(m_invModelMatrix, -this->getEulerRotation().y, glm::vec3(0.0,1.0,0.0));
    m_invModelMatrix = glm::rotate(m_invModelMatrix, -this->getEulerRotation().x, glm::vec3(1.0,0.0,0.0));
    m_invModelMatrix = glm::translate(m_invModelMatrix, -this->getPosition());

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
        glm::vec2 p(this->getPosition());
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

}
