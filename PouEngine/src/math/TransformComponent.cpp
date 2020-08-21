#include "PouEngine/math/TransformComponent.h"

#include <iostream>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

namespace pou
{

TransformComponent::TransformComponent() :
    m_parent(nullptr),
    m_treeDepth(0),
    m_lastGlobalPosition(0),
    m_globalPosition(glm::vec3(0)),
    m_position(glm::vec3(0)),
    m_eulerRotations(glm::vec3(0)),
    m_scale(glm::vec3(1.0)),
    m_rigidity(1.0),
    m_curFlexibleLength(0.0),
    m_curFlexibleRotation(0.0),
    m_modelMatrix(1.0),
    m_invModelMatrix(1.0)
{
    //ctor
}

TransformComponent::~TransformComponent()
{
    //dtor
}

void TransformComponent::update(const Time &elapsedTime/*, uint32_t localTime*/)
{
    m_lastGlobalPosition = this->getGlobalPosition();

    if(m_needToUpdateModelMat)
        this->updateModelMatrix();

    ///for(auto node : m_childs)
       /// node->update(elapsedTime,localTime);
}

void TransformComponent::copyFrom(const TransformComponent *srcTransform)
{
    this->setPosition(srcTransform->getPosition());
    this->setRotationInRadians(srcTransform->getEulerRotation());
    this->setScale(srcTransform->getScale());
    this->setRigidity(srcTransform->getRigidity());
}

//void TransformComponent::combineTransform(TransformComponent *localTransform)
//{
    /*if(m_needToUpdateModelMat)
        this->updateModelMatrix();

    if(localTransform->m_needToUpdateModelMat)
        localTransform->updateModelMatrix();

    auto newModelMatrix = localTransform->getModelMatrix() * this->getModelMatrix();
    this->loadFromModelMatrix(newModelMatrix);*/

    /*auto newPos = this->apply(localTransform->getPosition());

    auto oldRotVector = localTransform->getLocalRotationVector();
    auto newRotVector = this->apply(oldRotVector) - this->getGlobalPosition();

    auto newRotation = convertRotationVectorToYZRotation(newRotVector);

    this->setPosition(newPos);
    this->setRotationInRadians(newRotation);*/
//}

glm::vec4 TransformComponent::apply(const glm::vec4 &srcPoint)
{
    if(m_needToUpdateModelMat)
        this->updateModelMatrix();

    glm::vec4 v = m_modelMatrix * srcPoint;
    return v;
}

glm::vec3 TransformComponent::apply(const glm::vec3 &srcPoint)
{
    auto v = this->apply(glm::vec4(srcPoint, 1));
    v /= v.w;
    return glm::vec3(v.x, v.y, v.z);
}

/*void TransformComponent::loadFromModelMatrix(const glm::mat4 &model)
{
    glm::vec3 scale;
    glm::quat orientation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;

    glm::decompose(model,
		scale,
		orientation,
		translation,
		skew,
		perspective
	);

	orientation = glm::conjugate(orientation);

	glm::vec3 eulerRotation = glm::eulerAngles(orientation);

	std::cout<<translation.x<<" "<<translation.y<<" "<<translation.z<<std::endl;

	this->setPosition(translation);
	this->setScale(scale);
	this->setRotationInRadians(eulerRotation);
}*/

void TransformComponent::setParent(TransformComponent *parent)
{
    if(m_parent == parent)
        return;

    this->stopListeningTo(m_parent);

    ///auto oldParent = m_parent;

    m_parent = parent;
    if(m_parent != nullptr)
        this->startListeningTo(m_parent);

    ///if(oldParent != nullptr && p != nullptr)
       /// oldParent->removeChildNode(this);

    this->askForUpdateModelMatrix();
}


void TransformComponent::move(float x, float y)
{
    this->move(x,y,0);
}

void TransformComponent::move(float x, float y, float z)
{
    this->move(glm::vec3(x,y,z));
}

void TransformComponent::move(const glm::vec2 &p)
{
    this->move(glm::vec3(p.x,p.y,0));
}

void TransformComponent::move(const glm::vec3 &p)
{
    /**for(auto child : m_childs)
        child->computeFlexibleMove(p);**/

    this->setPosition(this->getPosition()+p);
}

void TransformComponent::setPosition(float x, float y)
{
    this->setPosition(glm::vec2(x,y));
}

void TransformComponent::setPosition(float x, float y, float z)
{
    this->setPosition(glm::vec3(x, y, z));
}

void TransformComponent::setPosition(const glm::vec2 &xyPos)
{
    this->setPosition(glm::vec3(xyPos.x, xyPos.y,this->getPosition().z));
}

void TransformComponent::setPosition(const glm::vec3 &pos)
{
    if(m_position == pos)
        return;

    m_position = pos;
    this->askForUpdateModelMatrix();
}

void TransformComponent::setGlobalPosition(float x, float y)
{
    this->setGlobalPosition(glm::vec2(x,y));
}

void TransformComponent::setGlobalPosition(float x, float y, float z)
{
    this->setGlobalPosition(glm::vec3(x, y, z));
}

void TransformComponent::setGlobalPosition(const glm::vec2 &xyPos)
{
    this->setGlobalPosition(glm::vec3(xyPos.x, xyPos.y,this->getGlobalPosition().z));
}

void TransformComponent::setGlobalPosition(const glm::vec3 &pos)
{
    if(m_parent)
    {
        glm::vec4 newPos = m_parent->getInvModelMatrix() * glm::vec4(pos,1.0);
        this->setPosition({newPos.x, newPos.y, newPos.z});
    }
    else
        this->setPosition(pos);
}

void TransformComponent::setRigidity(float rigidity)
{
    if(rigidity < 0 || rigidity > 1)
        return;
    m_rigidity = rigidity;
}

void TransformComponent::scale(float scale)
{
    this->scale(glm::vec3(scale));
}

void TransformComponent::scale(const glm::vec3 &scale)
{
    this->setScale(this->getScale()*scale);
}

void TransformComponent::linearScale(float x, float y, float z)
{
    this->linearScale(glm::vec3(x,y,z));
}

void TransformComponent::linearScale(const glm::vec3 &scale)
{
    this->setScale(this->getScale()+scale);
}

void TransformComponent::setScale(float scale)
{
    this->setScale(glm::vec3(scale));
}

void TransformComponent::setScale(const glm::vec3 &scale)
{
    if(m_scale == scale)
        return;

    m_scale = scale;
    this->askForUpdateModelMatrix();
}

void TransformComponent::rotateInRadians(float value, const glm::vec3 &axis)
{
    this->rotateInRadians(value*axis);
}

void TransformComponent::rotateInDegrees(float value, const glm::vec3 &axis)
{
    this->rotateInDegrees(value*axis);
}

void TransformComponent::rotateInRadians(const glm::vec3 &values)
{
    this->setRotationInRadians(this->getEulerRotation()+values);
}

void TransformComponent::rotateInDegrees(const glm::vec3 &values)
{
    this->rotateInRadians(values * glm::pi<float>() / 180.0f);
    //this->setRotationInDegrees(this->getEulerRotation()+values);
}

void TransformComponent::setRotationInRadians(const glm::vec3 &rotation)
{
    auto eulerRotations = glm::mod(rotation+glm::pi<float>()*glm::vec3(1.0),
                                   glm::pi<float>()*2)-glm::pi<float>()*glm::vec3(1.0);

    if(eulerRotations == m_eulerRotations)
        return;

    m_eulerRotations = eulerRotations;
    this->askForUpdateModelMatrix();
}


void TransformComponent::setRotationInDegrees(const glm::vec3 &rotation)
{
    this->setRotationInRadians(rotation * glm::pi<float>() / 180.0f);
}

size_t TransformComponent::getTreeDepth() const
{
    return m_treeDepth;
}

const glm::vec3 &TransformComponent::getGlobalPosition() const
{
    return m_globalPosition;
}

glm::vec2 TransformComponent::getGlobalXYPosition() const
{
    return glm::vec2(m_globalPosition);
}

const glm::vec3 &TransformComponent::getPosition() const
{
    return m_position;
}

const glm::vec3 &TransformComponent::getScale() const
{
    return m_scale;
}

const glm::vec3 &TransformComponent::getEulerRotation() const
{
    return m_eulerRotations;
}

float TransformComponent::getRigidity() const
{
    return m_rigidity;
}

const glm::mat4 &TransformComponent::getModelMatrix() const
{
    return m_modelMatrix;
}

const glm::mat4 &TransformComponent::getInvModelMatrix() const
{
    return m_invModelMatrix;
}


/*glm::vec3 TransformComponent::getLocalXRotationVector() const
{
    glm::vec4 localRotationVector(1.0,0.0,0.0,1.0);

    auto localRotationMatrix = glm::mat4(1.0f);

    localRotationMatrix = glm::rotate(localRotationMatrix, this->getEulerRotation().x, glm::vec3(1.0,0.0,0.0));
    localRotationMatrix = glm::rotate(localRotationMatrix, this->getEulerRotation().y, glm::vec3(0.0,1.0,0.0));
    localRotationMatrix = glm::rotate(localRotationMatrix, this->getEulerRotation().z, glm::vec3(0.0,0.0,1.0));

    localRotationVector = localRotationMatrix * localRotationVector;
    localRotationVector /= localRotationVector.w;

    if(m_scale.x < 1)
        localRotationVector.x *= -1;
    if(m_scale.y < 1)
        localRotationVector.y *= -1;
    if(m_scale.z < 1)
        localRotationVector.z *= -1;

    return localRotationVector;
}

glm::vec3 TransformComponent::convertRotationVectorToYZRotation(const glm::vec3 &rotationVector)
{
    glm::vec3 rotation(0.0f);

    rotation.z = glm::atan(rotationVector.x, rotationVector.y);

    auto rotZMat = glm::mat4(1.0f);
    rotZMat = glm::rotate(rotZMat, -rotation.z, glm::vec3(0.0,0.0,1.0));

    auto newRotVector = glm::vec4(rotationVector, 1.0f);
    newRotVector = rotZMat * newRotVector;
    newRotVector /= newRotVector.w;

    rotation.y = glm::atan(newRotVector.x, newRotVector.z);

    return rotation;
}*/

///
///Protected
///

void TransformComponent::notify(NotificationSender* sender, int notificationType,
                        void* data)
{
    if(sender == m_parent)
    {
        if(notificationType == NotificationType_TransformChanged)
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

void TransformComponent::askForUpdateModelMatrix()
{
    m_needToUpdateModelMat = true;
}


void TransformComponent::updateGlobalPosition()
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

void TransformComponent::updateModelMatrix()
{
    glm::vec3 oldGlobalPosition = m_globalPosition;

    this->updateGlobalPosition();

    if(m_rigidity != 1.0 && m_parent != nullptr)
        m_modelMatrix = glm::translate(glm::mat4(1.0), m_parent->getGlobalPosition());
    else
        m_modelMatrix = glm::mat4(1.0);

    m_modelMatrix = glm::translate(m_modelMatrix, this->getPosition());

    m_modelMatrix = glm::rotate(m_modelMatrix, this->getEulerRotation().z, glm::vec3(0.0,0.0,1.0));
    m_modelMatrix = glm::rotate(m_modelMatrix, this->getEulerRotation().y, glm::vec3(0.0,1.0,0.0));
    m_modelMatrix = glm::rotate(m_modelMatrix, this->getEulerRotation().x, glm::vec3(1.0,0.0,0.0));

    m_modelMatrix = glm::scale(m_modelMatrix, this->getScale());

    if(m_rigidity == 1.0 && m_parent != nullptr)
    {
        m_modelMatrix       = m_parent->getModelMatrix() * m_modelMatrix;
      //  m_invModelMatrix    = m_parent->getInvModelMatrix();
    }// else
        m_invModelMatrix = glm::mat4(1.0);

    m_invModelMatrix = glm::scale(m_invModelMatrix, 1.0f/this->getScale());
    m_invModelMatrix = glm::rotate(m_invModelMatrix, -this->getEulerRotation().x, glm::vec3(1.0,0.0,0.0));
    m_invModelMatrix = glm::rotate(m_invModelMatrix, -this->getEulerRotation().y, glm::vec3(0.0,1.0,0.0));
    m_invModelMatrix = glm::rotate(m_invModelMatrix, -this->getEulerRotation().z, glm::vec3(0.0,0.0,1.0));
    m_invModelMatrix = glm::translate(m_invModelMatrix, -this->getPosition());

    if(m_rigidity != 1.0 && m_parent != nullptr)
        m_invModelMatrix = glm::translate(m_invModelMatrix, -m_parent->getGlobalPosition());

    if(m_rigidity == 1.0 && m_parent != nullptr)
        m_invModelMatrix = m_invModelMatrix * m_parent->getInvModelMatrix();

    m_needToUpdateModelMat = false;
    this->sendNotification(NotificationType_TransformChanged,&oldGlobalPosition);



    ///TEST
   /* glm::mat4 testMat = m_modelMatrix * m_invModelMatrix;

    if(testMat != glm::mat4(1.0))
    {
        for(int i = 0 ; i < 4 ; ++i)

        {
        for(int j = 0 ; j < 4 ; ++j)
            std::cout<<(float)testMat[i][j]<<" ";
            std::cout<<std::endl;
        }
        std::cout<<std::endl;
        std::cout<<std::endl;
    }*/
}


/**void TransformComponent::computeFlexibleMove(glm::vec3 m)
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
}**/

}
