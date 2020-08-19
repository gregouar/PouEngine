#include "PouEngine/scene/SceneNode.h"

#include "PouEngine/scene/Scene.h"
#include "PouEngine/scene/SceneObject.h"
#include "PouEngine/tools/Logger.h"


namespace pou
{

SceneNode::SceneNode() :
    //SimpleNode(),
    m_nodeId(0),
    m_parentNode(nullptr),
    m_disableCollisions(false),
    m_color(1.0f),
    m_finalColor(1.0f),
    m_name(0)
{
    m_scene = nullptr;
    //m_transformComponent = new TransformComponent();
    this->startListeningTo(&m_transformComponent, NotificationType_TransformChanged);
}

SceneNode::SceneNode(Scene* scene) :
    SceneNode()
{
    this->setScene(scene);
}

SceneNode::~SceneNode()
{
    //delete m_transformComponent;
    //this->removeAndDestroyAllChilds();
}

std::shared_ptr<SceneNode> SceneNode::createChildNode()
{
    auto childNode = std::make_shared<SceneNode>();
    this->addChildNode(childNode);
    return childNode;
}

void SceneNode::addChildNode(std::shared_ptr<SceneNode> childNode)
{
    if(containsChildNode(childNode.get()))
        return;

    childNode->setParentNode(this);
    m_childNodes.push_back(childNode);
}

bool SceneNode::containsChildNode(SceneNode *childNode)
{
    for(auto it = m_childNodes.begin() ; it != m_childNodes.end() ; ++it)
        if(it->get() == childNode)
            return (true);
    return (false);
}

bool SceneNode::removeChildNode(SceneNode *childNode)
{
    for(auto it = m_childNodes.begin() ; it != m_childNodes.end() ; ++it)
        if(it->get() == childNode)
        {
            m_childNodes.erase(it);
            (*it)->setParentNode(nullptr);
            return (true);
        }
    return (false);
}

void SceneNode::removeFromParent()
{
    if(m_parentNode)
        m_parentNode->removeChildNode(this);
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

    for(auto s = founded.first ; s != founded.second ; )
    {
        if(s->second.get() == e)
        {
            s = m_attachedSounds.erase(s);
            this->detachObject(e);
        }
        else
            ++s;
    }
}

void SceneNode::detachAllObjects()
{
    while(!m_attachedObjects.empty())
        this->detachObject(m_attachedObjects.back().get());

    m_attachedSounds.clear();
}

void SceneNode::copyFrom(const SceneNode* srcNode)
{
    if(srcNode == nullptr)
        return;

    m_transformComponent.copyFrom(srcNode->const_transform());

    this->setColor(srcNode->getColor());
    this->setName(srcNode->getName());

    for(auto object : srcNode->m_attachedObjects)
    {
        if(object->isASound())
            continue;

        auto newObject = object->createCopy();
        this->attachObject(newObject);
    }

    for(auto sound : srcNode->m_attachedSounds)
    {
        auto newObject = sound.second->createSoundCopy();
        this->attachSound(newObject, sound.first);
    }

    for(auto child : srcNode->m_childNodes)
    {
        std::shared_ptr<SceneNode> newNode = this->createChildNode();
        if(newNode != nullptr)
            newNode->copyFrom(child.get());
    }
}

Scene* SceneNode::getScene()
{
    return m_scene;
}

SceneNode* SceneNode::getParentNode()
{
    return m_parentNode;
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
    this->setColor(this->getColor() + c);
}

void SceneNode::setColor(const glm::vec4 &c)
{
    if(m_color == c)
        return;

    m_color = c;
    this->updateFinalColor();
}


void SceneNode::disableCollisions(bool disable)
{
    m_disableCollisions = disable;
}

bool SceneNode::areCollisionsDisabled()
{
    return m_disableCollisions;
}

void SceneNode::generateRenderingData(SceneRenderingInstance *renderingInstance)
{
    this->generateRenderingDataWithoutPropagating(renderingInstance);

    for(auto child : m_childNodes)
        child->generateRenderingData(renderingInstance);
}

void SceneNode::generateRenderingDataWithoutPropagating(SceneRenderingInstance *renderingInstance)
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
    m_transformComponent.update(elapsedTime);

    for(auto child : m_childNodes)
        child->update(elapsedTime,localTime);

    for(auto &object : m_attachedObjects)
        object->update(elapsedTime);
}

TransformComponent *SceneNode::transform()
{
    return &m_transformComponent;
}

const TransformComponent *SceneNode::const_transform() const
{
    return &m_transformComponent;
}

void SceneNode::setName(HashedString name)
{
    m_name = name;
}

HashedString SceneNode::getName() const
{
    return m_name;
}

void SceneNode::getNodesByName(std::unordered_map<HashedString, SceneNode*> &namesAndResMap)
{
    if(m_name != 0)
    {
        auto res = namesAndResMap.find(m_name);
        if(res != namesAndResMap.end())
            namesAndResMap.insert_or_assign(res, m_name, this);
    }

    for(auto child : m_childNodes)
        child->getNodesByName(namesAndResMap);
}

void SceneNode::setNodeId(uint32_t id)
{
    m_nodeId = id;
}

uint32_t SceneNode::getNodeId() const
{
    return m_nodeId;
}

///
///Protected
///


void SceneNode::notify(NotificationSender *sender, int notificationType, void* data)
{
    if(sender == &m_transformComponent)
    {
        //if(notificationType == NotificationType_SenderDestroyed)
          //  m_transformComponent = nullptr;

        if(notificationType == NotificationType_TransformChanged)
            this->sendNotification(NotificationType_NodeMoved, data);
    }

    if(sender == m_parentNode)
    {
        if(notificationType == NotificationType_SenderDestroyed)
            this->setParentNode(nullptr);

        /*if(notificationType == NotificationType_NodeMoved)
            this->sendNotification(NotificationType_NodeMoved);*/
    }
}

void SceneNode::setScene(Scene *scene)
{
    m_scene = scene;

    for(auto child : m_childNodes)
        child->setScene(scene);
}

bool SceneNode::setParentNode(SceneNode *parent)
{
    if(m_parentNode == parent)
        return (false);

    if(m_parentNode && parent)
        m_parentNode->removeChildNode(this);

    if(parent)
    {
        this->setScene(parent->getScene());
        m_transformComponent.setParent(parent->transform());
    }
    else
    {
        this->setScene(nullptr);
        m_transformComponent.setParent(nullptr);
    }

    m_parentNode = parent;

    this->sendNotification(NotificationType_NodeParentChanged);

    return (true);
}

bool SceneNode::setAsParentTo(SceneNode *parentNode, SceneNode *childNode)
{
    return childNode->setParentNode(parentNode);
}

void SceneNode::updateFinalColor()
{
    if(m_parentNode)
        m_finalColor = m_color * m_parentNode->getFinalColor();
    else
        m_finalColor = m_color;

    for(auto child : m_childNodes)
        child->updateFinalColor();

    this->sendNotification(NotificationType_NodeMoved);
}

}
