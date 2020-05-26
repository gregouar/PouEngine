#include "PouEngine/scene/SceneObject.h"

#include "PouEngine/scene/SceneNode.h"

namespace pou
{


SceneObject::SceneObject()
{
    m_parentNode        = nullptr;
    m_isALight          = false;
    m_isAnEntity        = false;
    m_isAShadowCaster   = false;

    m_curLocalTime = 0;
    m_lastUpdateTime = -1;
    m_lastNodeUpdateTime = -1;
}

SceneObject::~SceneObject()
{
    //dtor
}


SceneNode* SceneObject::setParentNode(SceneNode *newParent)
{
    SceneNode* oldParent = getParentNode();

    if(m_parentNode != newParent)
    {
        this->stopListeningTo(m_parentNode);
        m_parentNode = newParent;
        m_lastNodeUpdateTime = m_curLocalTime;
        this->setLastUpdateTime(m_curLocalTime);
        this->startListeningTo(m_parentNode);
    }
    return oldParent;
}

SceneNode* SceneObject::getParentNode()
{
    return m_parentNode;
}


bool SceneObject::isALight()
{
    return m_isALight;
}

bool SceneObject::isAnEntity()
{
    return m_isAnEntity;
}

bool SceneObject::isAShadowCaster()
{
    return m_isAShadowCaster;
}

void SceneObject::setLocalTime(float time)
{
    m_curLocalTime = time;
}

void SceneObject::setLastUpdateTime(float time, bool force)
{
    if(force || m_lastUpdateTime < time)
        m_lastUpdateTime = time;
}

float SceneObject::getLastUpdateTime()
{
    return m_lastUpdateTime;
}

float SceneObject::getLastNodeUpdateTime()
{
    return m_lastNodeUpdateTime;
}

void SceneObject::update(const Time &elapsedTime, float localTime)
{
    m_curLocalTime = localTime;
}

void SceneObject::notify(NotificationSender* sender, NotificationType type,
                         size_t dataSize, char* data)
{
    if(sender == (NotificationSender*)m_parentNode)
    {
        if(type == Notification_SenderDestroyed)
            m_parentNode = nullptr;
    }
}


}
