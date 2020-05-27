#include "PouEngine/scene/SceneNode.h"

#include "PouEngine/scene/Scene.h"
#include "PouEngine/scene/SceneObject.h"
#include "PouEngine/utils/Logger.h"


namespace pou
{

SceneNode::SceneNode(const NodeTypeId id) :
    SimpleNode(id),
    m_color(1.0f),
    m_finalColor(1.0f),
    m_lastColorUpdateTime(-1)
{
    m_scene = nullptr;
}

SceneNode::SceneNode(const NodeTypeId id, Scene* scene) :
    SceneNode(id)
{
    this->setScene(scene);
}

SceneNode::~SceneNode()
{
    //this->removeAndDestroyAllChilds();
}


SceneNode* SceneNode::createChildNode()
{
    return dynamic_cast<SceneNode*>(SimpleNode::createChildNode());
}

SceneNode* SceneNode::createChildNode(float x, float y)
{
    return dynamic_cast<SceneNode*>(SimpleNode::createChildNode(x,y));
}

SceneNode* SceneNode::createChildNode(float x, float y, float z)
{
    return dynamic_cast<SceneNode*>(SimpleNode::createChildNode(x,y,z));
}

SceneNode* SceneNode::createChildNode(glm::vec2 p)
{
    return dynamic_cast<SceneNode*>(SimpleNode::createChildNode(p));
}

SceneNode* SceneNode::createChildNode(glm::vec3 p)
{
    return dynamic_cast<SceneNode*>(SimpleNode::createChildNode(p));
}

SceneNode* SceneNode::createChildNode(const NodeTypeId id)
{
    return dynamic_cast<SceneNode*>(SimpleNode::createChildNode(id));
}

void SceneNode::attachObject(SceneObject *e)
{
    if(e != nullptr)
    {
        m_attachedObjects.push_back(e);

        if(e->isAnEntity())
            m_attachedEntities.push_back(dynamic_cast<SceneEntity*>(e));

        if(e->isALight())
            m_attachedLights.push_back(dynamic_cast<LightEntity*>(e));

        if(e->isAShadowCaster())
            m_attachedShadowCasters.push_back(dynamic_cast<ShadowCaster*>(e));

        if(e->setParentNode(this) != nullptr)
            Logger::warning("Attaching entity which has already a parent node");

        e->notify(this, Notification_NodeMoved);
    } else
        Logger::error("Cannot attach null entity");
}

void SceneNode::attachSound(SoundObject *e, int id)
{
    this->attachObject(e);
    m_attachedSounds.insert({id,e});
}

void SceneNode::detachObject(SceneObject *e)
{
    if(e->getParentNode() != this)
    {
        Logger::warning("Tried to detach object from non-parent node");
        return;
    }

    m_attachedObjects.remove(e);

    if(e != nullptr && e->isAnEntity())
        m_attachedEntities.remove(dynamic_cast<SceneEntity*>(e));

    if(e != nullptr && e->isALight())
        m_attachedLights.remove(dynamic_cast<LightEntity*>(e));

    if(e != nullptr && e->isAShadowCaster())
        m_attachedShadowCasters.remove(dynamic_cast<ShadowCaster*>(e));

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
    for(auto &s = founded.first ; s != founded.second ; ++s)
    if(s->second == e)
    {
        this->detachObject(e);
        m_attachedSounds.erase(s);
    }
}


void SceneNode::detachAllObjects()
{
    while(!m_attachedObjects.empty())
        this->detachObject(m_attachedObjects.back());

    m_attachedSounds.clear();
}

Scene* SceneNode::getScene()
{
    return m_scene;
}

const glm::vec4 &SceneNode::getColor() const
{
    return m_color;
}

const glm::vec4 &SceneNode::getFinalColor() const
{
    return m_finalColor;
}

void SceneNode::colorize(const glm::vec4 &c)
{
    this->setColor(m_color + c);
}

void SceneNode::setColor(const glm::vec4 &c)
{
    m_color = c;
    m_lastColorUpdateTime = m_curLocalTime;
    this->setLastUpdateTime(m_curLocalTime);

    this->updateGlobalPosition();
}

void SceneNode::setScene(Scene *scene)
{
    m_scene = scene;

    for(auto node : m_childs)
        dynamic_cast<SceneNode*>(node.second)->setScene(scene);
}

void SceneNode::setParent(SimpleNode *p)
{
    SimpleNode::setParent(p);

    if(this->getParent() != nullptr)
        this->setScene(dynamic_cast<SceneNode*>(m_parent)->getScene());
}

void SceneNode::syncFromNode(SceneNode* srcNode)
{
    if(m_lastSyncTime < srcNode->m_lastColorUpdateTime)
        //&& srcNode->m_lastColorUpdateTime != -1)
        this->setColor(srcNode->getColor());

    if(!SimpleNode::syncFromNode((SimpleNode*) srcNode))
        return;
}

void SceneNode::generateRenderingData(SceneRenderingInstance *renderingInstance)
{
    for(auto entity : m_attachedEntities)
        if(entity->isVisible())
            entity->generateRenderingData(renderingInstance);

    for(auto light : m_attachedLights)
        if(light->isVisible())
            renderingInstance->addToShadowLightsList(light);

    for(auto shadowCaster : m_attachedShadowCasters)
        if(shadowCaster->isVisible())
            renderingInstance->addToShadowCastersList(shadowCaster);

    for(auto node : m_childs)
        dynamic_cast<SceneNode*>(node.second)->generateRenderingData(renderingInstance);
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

void SceneNode::update(const Time &elapsedTime, float localTime)
{
    SimpleNode::update(elapsedTime, localTime);

    //int i = 0;
    for(auto &object : m_attachedObjects)
    {
        object->update(elapsedTime);
        //std::cout<<i++<<std::endl;
    }
}


///Protected

void SceneNode::updateGlobalPosition()
{
    glm::vec4 parentColor(1.0f);
    if(m_parent != nullptr)
        parentColor = dynamic_cast<SceneNode*>(m_parent)->getFinalColor();
    m_finalColor = m_color * parentColor;

    SimpleNode::updateGlobalPosition();
}

SimpleNode* SceneNode::nodeAllocator(NodeTypeId id)
{
    return new SceneNode(id);
}



/*void SceneNode::notify(NotificationSender* sender, NotificationType type,
                       size_t dataSize, char* data)
{
    if(sender == m_parent)
    {
        if(type == Notification_NodeMoved)
            this->updateGlobalPosition();
        if(type == Notification_SenderDestroyed)
        {
            m_parent = nullptr;
            this->updateGlobalPosition();
        }
    }
}*/


void SceneNode::serializeNode(Stream *stream, float clientTime)
{
    SimpleNode::serializeNode(stream, clientTime);

    bool hasColor = false;
    if(!stream->isReading() && clientTime < m_lastColorUpdateTime)
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
