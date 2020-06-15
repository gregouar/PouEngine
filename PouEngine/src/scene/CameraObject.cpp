#include "PouEngine/scene/CameraObject.h"

#include "PouEngine/scene/SceneNode.h"

namespace pou
{

CameraObject::CameraObject() :
    m_isListening(false),
    m_3DListener(0),
    m_zoom(1.0f),
    m_offset(0.0f, 0.0f),
    m_extent(1.0f, 1.0f)
{
    //ctor
}

CameraObject::~CameraObject()
{
    this->setListening(false);
}


void CameraObject::setViewport(glm::vec2 offset, glm::vec2 extent)
{
    m_offset = offset;
    m_extent = extent;
}

void CameraObject::setListening(bool listening)
{
    if(!m_isListening && listening)
    {
        m_3DListener    = AudioEngine::add3DListener();
        m_isListening   = true;
        return;
    }

    if(m_isListening && !listening)
    {
        AudioEngine::remove3DListener(m_3DListener);
        m_3DListener    = 0;
        m_isListening   = false;
        return;
    }
}

void CameraObject::setZoom(float zoom)
{
    if(zoom != 0)
        m_zoom = zoom;
}

void CameraObject::zoom(float zoom)
{
    if(zoom != 0)
        m_zoom *= zoom;
}

float CameraObject::getZoom()
{
    return m_zoom;
}

glm::vec2 CameraObject::getViewportOffset()
{
    return m_offset;
}

glm::vec2 CameraObject::getViewportExtent()
{
    return m_extent;
}


void CameraObject::updateListeningPosition()
{
    if(m_isListening && m_parentNode != nullptr)
    {
        //glm::vec4 up     = m_parentNode.getModelMatrix() * glm::vec4(0.0,0.0,0.0,1.0)
        //glm::vec4 forwrd = m_parentNode.getModelMatrix() * glm::vec4(0.0,0.0,0.0,1.0);
        //glm::vec3 up(0.0,0.0,1.0);
        //glm::vec3 forwrd(0.0,1.0,0.0);
        AudioEngine::set3DListenerPosition(m_3DListener, m_parentNode->getGlobalPosition());
    }
}

void CameraObject::notify(NotificationSender *sender, int notificationType,
                          void* data)
{
    if(notificationType == NotificationType_NodeMoved)
        this->updateListeningPosition();

    SceneObject::notify(sender,notificationType,data);
}


}
