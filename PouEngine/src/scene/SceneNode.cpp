#include "PouEngine/scene/SceneNode.h"

#include "PouEngine/scene/Scene.h"
#include "PouEngine/scene/SceneObject.h"
#include "PouEngine/utils/Logger.h"


namespace pou
{

SceneNode::SceneNode(/**const NodeTypeId id**/) :
    ///SimpleNode(id),
    m_color(glm::vec4(1.0f),0),
    m_finalColor(1.0f)//,
    //m_lastColorUpdateTime(-1)
{
    m_scene = nullptr;
}

SceneNode::SceneNode(/**const NodeTypeId id,**/ Scene* scene) :
    SceneNode(/**id**/)
{
    this->setScene(scene);
}

SceneNode::~SceneNode()
{
    //this->removeAndDestroyAllChilds();
}


std::shared_ptr<SceneNode> SceneNode::createChildNode()
{
    return std::dynamic_pointer_cast<SceneNode>(SimpleNode::createChildNode());
}

std::shared_ptr<SceneNode> SceneNode::createChildNode(float x, float y)
{
    return std::dynamic_pointer_cast<SceneNode>(SimpleNode::createChildNode(x,y));
}

std::shared_ptr<SceneNode> SceneNode::createChildNode(float x, float y, float z)
{
    return std::dynamic_pointer_cast<SceneNode>(SimpleNode::createChildNode(x,y,z));
}

std::shared_ptr<SceneNode> SceneNode::createChildNode(glm::vec2 p)
{
    return std::dynamic_pointer_cast<SceneNode>(SimpleNode::createChildNode(p));
}

std::shared_ptr<SceneNode> SceneNode::createChildNode(glm::vec3 p)
{
    return std::dynamic_pointer_cast<SceneNode>(SimpleNode::createChildNode(p));
}


void SceneNode::attachObject(std::shared_ptr<SceneObject> e)
{
    if(e != nullptr)
    {
        m_attachedObjects.push_back(e);

        if(e->isAnEntity())
            m_attachedEntities.push_back(std::dynamic_pointer_cast<SceneEntity>(e));

        if(e->isALight())
            m_attachedLights.push_back(std::dynamic_pointer_cast<LightEntity>(e));

        if(e->isAShadowCaster())
            m_attachedShadowCasters.push_back(std::dynamic_pointer_cast<ShadowCaster>(e));

        if(e->setParentNode(this) != nullptr)
            Logger::warning("Attaching entity which has already a parent node");

        e->notify(this, NotificationType_NodeMoved);
    } else
        Logger::error("Cannot attach null entity");
}

void SceneNode::attachSound(std::shared_ptr<SoundObject> e, int id)
{
    this->attachObject(e);
    m_attachedSounds.insert({id,e});
}

void SceneNode::detachObject(SceneObject *e)
{
    if(!e)
        return;

    if(e->getParentNode() != this)
    {
        Logger::warning("Tried to detach object from non-parent node");
        return;
    }

    for(auto it = m_attachedObjects.begin() ; it != m_attachedObjects.end() ; ++it)
    if(it->get() == e)
    {
        m_attachedObjects.erase(it);
        break;
    }

    if(e->isAnEntity())
    {
        for(auto it = m_attachedEntities.begin() ; it != m_attachedEntities.end() ; ++it)
        if(it->get() == e)
        {
            m_attachedEntities.erase(it);
            break;
        }
    }

    if(e->isALight())
    {
        for(auto it = m_attachedLights.begin() ; it != m_attachedLights.end() ; ++it)
        if(it->get() == e)
        {
            m_attachedLights.erase(it);
            break;
        }
    }

    if(e->isAShadowCaster())
    {
        for(auto it = m_attachedShadowCasters.begin() ; it != m_attachedShadowCasters.end() ; ++it)
        if(it->get() == e)
        {
            m_attachedShadowCasters.erase(it);
            break;
        }
    }

    /**m_attachedObjects.remove(e);

    if(e != nullptr && e->isAnEntity())
        m_attachedEntities.remove(dynamic_cast<SceneEntity*>(e));

    if(e != nullptr && e->isALight())
        m_attachedLights.remove(dynamic_cast<LightEntity*>(e));

    if(e != nullptr && e->isAShadowCaster())
        m_attachedShadowCasters.remove(dynamic_cast<ShadowCaster*>(e));**/

    e->setParentNode(nullptr);
}

void SceneNode::detachSound(SoundObject *e,int id)
{
    /*auto founded = m_attachedSounds.find(id);
    if(founded == m_attachedSounds.end())
        return;

    this->detachObject(founded->second);
    m_attachedSounds.erase(founded);*/

    auto founded = m_attachedSounds.equal_range(id);

    for(auto s = founded.first ; s != founded.second ; ++s)
        if(s->second.get() == e)
        {
            s = m_attachedSounds.erase(s);
            this->detachObject(e);
        }
}


void SceneNode::detachAllObjects()
{
    while(!m_attachedObjects.empty())
        this->detachObject(m_attachedObjects.back().get());

    m_attachedSounds.clear();
}

Scene* SceneNode::getScene()
{
    return m_scene;
}

const glm::vec4 &SceneNode::getColor() const
{
    return m_color.getValue();
}

const glm::vec4 &SceneNode::getFinalColor() const
{
    return m_finalColor;
}

void SceneNode::colorize(const glm::vec4 &c)
{
    this->setColor(m_color.getValue() + c);
}

void SceneNode::setColor(const glm::vec4 &c)
{
    m_color.setValue(c);
    //m_lastColorUpdateTime = m_curLocalTime;
    this->setLastUpdateTime(m_curLocalTime);

    this->updateGlobalPosition();
}

void SceneNode::setScene(Scene *scene)
{
    m_scene = scene;

    for(auto node : m_childs)
        std::dynamic_pointer_cast<SceneNode>(node)->setScene(scene);
}

void SceneNode::setParent(SimpleNode *p)
{
    SimpleNode::setParent(p);

    if(this->getParent() != nullptr)
        this->setScene(dynamic_cast<SceneNode*>(m_parent)->getScene());
}

void SceneNode::syncFromNode(SceneNode* srcNode)
{
    m_color.syncFrom(srcNode->m_color);
   // if(m_lastSyncTime < srcNode->m_lastColorUpdateTime)
        //&& srcNode->m_lastColorUpdateTime != -1)
       // this->setColor(srcNode->getColor());

    SimpleNode::syncFromNode((SimpleNode*) srcNode);
}

void SceneNode::generateRenderingData(SceneRenderingInstance *renderingInstance)
{
    for(auto entity : m_attachedEntities)
        if(entity->isVisible())
            entity->generateRenderingData(renderingInstance);

    for(auto light : m_attachedLights)
        if(light->isVisible())
            renderingInstance->addToShadowLightsList(light.get());

    for(auto shadowCaster : m_attachedShadowCasters)
        if(shadowCaster->isVisible())
            renderingInstance->addToShadowCastersList(shadowCaster.get());

    for(auto node : m_childs)
        std::dynamic_pointer_cast<SceneNode>(node)->generateRenderingData(renderingInstance);
}

bool SceneNode::playSound(int id)
{
    /*auto sound = m_attachedSounds.find(id);
    if(sound == m_attachedSounds.end())
        return (false);

    return sound->second->play();*/

    bool r = true;

    auto founded = m_attachedSounds.equal_range(id);
    for(auto &s = founded.first ; s != founded.second ; ++s)
        if(!s->second->play()) r = false;

    return r;
}

void SceneNode::update(const Time &elapsedTime, uint32_t localTime)
{
    SimpleNode::update(elapsedTime, localTime);

    for(auto &object : m_attachedObjects)
        object->update(elapsedTime);
}


///Protected

void SceneNode::updateGlobalPosition()
{
    glm::vec4 parentColor(1.0f);
    if(m_parent != nullptr)
        parentColor = dynamic_cast<SceneNode*>(m_parent)->getFinalColor();
    m_finalColor = m_color.getValue() * parentColor;

    SimpleNode::updateGlobalPosition();
}

std::shared_ptr<SimpleNode> SceneNode::nodeAllocator(/**NodeTypeId id**/)
{
    return std::make_shared<SceneNode>(/**id**/);
}



/*void SceneNode::notify(NotificationSender* sender, NotificationType type,
                       size_t dataSize, char* data)
{
    if(sender == m_parent)
    {
        if(type == NotificationType_NodeMoved)
            this->updateGlobalPosition();
        if(type == NotificationType_SenderDestroyed)
        {
            m_parent = nullptr;
            this->updateGlobalPosition();
        }
    }
}*/


void SceneNode::serializeNode(Stream *stream, uint32_t clientTime)
{
    SimpleNode::serializeNode(stream, clientTime);

    bool hasColor = false;
    if(!stream->isReading() && uint32less(clientTime,m_color.getLastUpdateTime()))
        hasColor = true;
    stream->serializeBool(hasColor);
    if(hasColor)
    {
        glm::vec4 color = this->getColor();
        stream->serializeFloat(color.r, 0, 10, 2);
        stream->serializeFloat(color.g, 0, 10, 2);
        stream->serializeFloat(color.b, 0, 10, 2);
        stream->serializeFloat(color.a, 0, 1, 2);

        if(stream->isReading())
            this->setColor(color);
    }
}

}
