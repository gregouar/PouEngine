#include "PouEngine/scene/CollisionObject.h"

#include "PouEngine/scene/SceneNode.h"

namespace pou
{

CollisionObject::CollisionObject() :
    m_isBoxBody(false),
    m_isDiskBody(false)
{
    m_boxBody.mass = -1;
}

CollisionObject::~CollisionObject()
{
    //dtor
}

std::shared_ptr<SceneObject> CollisionObject::createCopy()
{
    auto newObject = std::make_shared<CollisionObject>();
    newObject->m_isBoxBody  = m_isBoxBody;
    newObject->m_boxBody    = m_boxBody;
    newObject->m_isDiskBody = m_isDiskBody;
    newObject->m_diskBody   = m_diskBody;
    return newObject;
}

void CollisionObject::update(const Time &elapsedTime, uint32_t localTime)
{
    SceneObject::update(elapsedTime, localTime);

    if(elapsedTime.count() <= 0)
        return;

    if(m_parentNode && m_parentNode->areCollisionsDisabled())
        return;

    if(m_isBoxBody)
        PhysicsEngine::addBoxBody(m_boxBody);

    if(m_isDiskBody)
        PhysicsEngine::addDiskBody(m_diskBody);
}

void CollisionObject::setMass(float mass)
{
    if(mass < 0)
        mass = -1;
    m_boxBody.mass = mass;
    m_diskBody.mass = mass;
}

void CollisionObject::setBox(const glm::vec2 &size)
{
    this->setBox(size, size*0.5f);
}

void CollisionObject::setBox(const glm::vec2 &size, const glm::vec2 &center)
{
    this->setBox(MathTools::Box{size, center});
}

void CollisionObject::setBox(const MathTools::Box &box)
{
    m_isBoxBody = true;
    m_boxBody.box = box;

}

void CollisionObject::setDisk(float radius)
{
    m_isDiskBody = true;
    m_diskBody.radius = radius;

}

///
///Protected
///


SceneNode *CollisionObject::setParentNode(SceneNode* parentNode)
{
    auto node = SceneObject::setParentNode(parentNode);
    m_boxBody.node = this->getParentNode();
    m_diskBody.node = this->getParentNode();
    return node;
}

}
