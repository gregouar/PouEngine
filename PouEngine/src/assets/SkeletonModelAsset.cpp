#include "PouEngine/assets/SkeletonModelAsset.h"

#include "PouEngine/Types.h"

#include "PouEngine/assets/AssetHandler.h"
#include "PouEngine/assets/TextureAsset.h"

#include "PouEngine/utils/Logger.h"
#include "PouEngine/utils/Parser.h"

namespace pou
{


SkeletonModelAsset::SkeletonModelAsset() : SkeletonModelAsset(-1)
{

}

SkeletonModelAsset::SkeletonModelAsset(const AssetTypeId id) : Asset(id), m_rootNode(-1)
{
    m_allowLoadFromFile     = true;
    m_allowLoadFromMemory   = false;
}

SkeletonModelAsset::~SkeletonModelAsset()
{
    //dtor
}


bool SkeletonModelAsset::loadFromFile(const std::string &filePath)
{
    TiXmlDocument file(filePath.c_str());

    if(!file.LoadFile())
    {
        Logger::error("Cannot load skeleton from file: "+filePath);
        std::ostringstream errorReport;
        errorReport << "Because: "<<file.ErrorDesc();
        Logger::error(errorReport);
        return (false);
    }

    TiXmlHandle hdl(&file);
    hdl = hdl.FirstChildElement();

    return this->loadFromXML(&hdl);
}


bool SkeletonModelAsset::loadFromXML(TiXmlHandle *hdl)
{
    bool loaded = true;

    if(hdl == nullptr)
        return (false);

    this->loadNode(&m_rootNode, hdl->FirstChildElement("node").Element());

    /*auto child = hdl->FirstChildElement("node");
    if(child == nullptr)
        return;*/


    auto animElement = hdl->FirstChildElement("animation").Element();
    while(animElement != nullptr)
    {
        this->loadAnimation(animElement);
        animElement = animElement->NextSiblingElement("animation");
    }

    if(loaded)
        Logger::write("Skeleton loaded from file: "+m_filePath);

    return (loaded);
}

void SkeletonModelAsset::loadNode(SimpleNode* rootNode, TiXmlElement *element)
{
    std::string nodeName;
    glm::vec3 nodePos(0,0,0);

    auto nameAtt = element->Attribute("name");
    if(nameAtt != nullptr)
    {
        nodeName = std::string(nameAtt);
        rootNode->setName(nodeName);
    }

    if(!nodeName.empty())
    if(!m_nodesByName.insert({nodeName,rootNode}).second)
        Logger::warning("Multiple nodes with the same name \"" + nodeName + "\" in Skeleton Asset : "+m_filePath);

    auto attribute = element->Attribute("x");
    if(attribute != nullptr)
        nodePos.x = Parser::parseFloat(std::string(attribute));

    attribute = element->Attribute("y");
    if(attribute != nullptr)
        nodePos.y = Parser::parseFloat(std::string(attribute));

    attribute = element->Attribute("z");
    if(attribute != nullptr)
        nodePos.z = Parser::parseFloat(std::string(attribute));


    /*attribute = element->Attribute("flexible");
    if(attribute != nullptr && Parser::parseBool(std::string(attribute)))
    {
        SimpleNode* parent = rootNode->getParent();
        if(parent != nullptr)
        {
            parent->moveChildNode(rootNode, parent->getParent());
            m_joints.insert({parent->getName(), rootNode->getName()});
        }
    }*/
    attribute = element->Attribute("rigidity");
    if(attribute != nullptr)
    {
        float rigidity = Parser::parseFloat(std::string(attribute));
        rootNode->setRigidity(rigidity);
    }

    rootNode->setPosition(nodePos);

    auto child = element->FirstChildElement("node");
    if(child == nullptr)
        return;

    TiXmlElement* childElement = child->ToElement();
    while(childElement != nullptr)
    {
        SimpleNode* node = rootNode->createChildNode();
        this->loadNode(node, childElement);

        childElement = childElement->NextSiblingElement("node");
    }
}

void SkeletonModelAsset::loadAnimation(TiXmlElement *element)
{
    std::string animationName;
    std::unique_ptr<SkeletalAnimationModel> animationModel(new SkeletalAnimationModel());

    auto nameAtt = element->Attribute("name");
    if(nameAtt != nullptr)
        animationName = std::string(nameAtt);
    else
        animationName = "Animation"+std::to_string(m_animations.size());

    animationModel->setName(animationName);
    animationModel->loadFromXml(element);

    if(!m_animations.insert(std::make_pair(animationName,std::move(animationModel))).second)
        Logger::warning("Multiple animations named \""+animationName+"\" in the skeleton : "+m_filePath);
}

const SimpleNode* SkeletonModelAsset::getRootNode() const
{
    return &m_rootNode;
}


std::map<std::string, SimpleNode*> SkeletonModelAsset::getNodesByName()
{
    return m_nodesByName;
}

SkeletalAnimationModel* SkeletonModelAsset::findAnimation(const std::string &name)
{
    auto animation = m_animations.find(name);
    if(animation == m_animations.end())
        return (nullptr);
    return animation->second.get();
}

/**                        **/
/// SkeletalAnimationModel ///
/**                        **/


SkeletalAnimationModel::SkeletalAnimationModel() :
    m_isLooping(false)
{

}

SkeletalAnimationModel::~SkeletalAnimationModel()
{

}

bool SkeletalAnimationModel::loadFromXml(TiXmlElement *element)
{
    auto loopAtt = element->Attribute("loop");
    if(loopAtt != nullptr)
        m_isLooping = Parser::parseBool(loopAtt);

    SkeletalAnimationFrameModel *lastFrame = nullptr;
    auto frameElement = element->FirstChildElement("frame");
    while(frameElement != nullptr)
    {
        m_frames.push_back(SkeletalAnimationFrameModel ());
        m_frames.back().loadFromXml(frameElement->ToElement());
        if(lastFrame != nullptr)
            lastFrame->setNextFrame(&m_frames.back());
        lastFrame = &m_frames.back();
        frameElement = frameElement->NextSiblingElement("frame");
    }

    return (true);
}

SkeletalAnimationFrameModel* SkeletalAnimationModel::nextFrame(SkeletalAnimationFrameModel* curFrame)
{
    if(curFrame == nullptr)
        return &m_frames.front();

    auto frame = curFrame->m_nextFrame;
    if(frame == nullptr && m_isLooping)
        frame = &m_frames.front();

    return frame;
}

void SkeletalAnimationModel::setName(const std::string &name)
{
    m_name = name;
}

const std::string &SkeletalAnimationModel::getName() const
{
    return m_name;
}

bool SkeletalAnimationModel::isLooping()
{
    return m_isLooping;
}


/**                              **/
/// SkeletalAnimationFrameModel  ///
/**                              **/

SkeletalAnimationFrameModel::SkeletalAnimationFrameModel() :
    m_nextFrame(nullptr)
{

}

SkeletalAnimationFrameModel::~SkeletalAnimationFrameModel()
{

}

bool SkeletalAnimationFrameModel::loadFromXml(TiXmlElement *element)
{
    auto commandElement = element->FirstChildElement("command");
    while(commandElement != nullptr)
    {
        m_commands.push_back(SkeletalAnimationCommandModel ());
        m_commands.back().loadFromXml(commandElement->ToElement());
        commandElement = commandElement->NextSiblingElement("command");
    }

    return (true);
}

const std::list<SkeletalAnimationCommandModel> *SkeletalAnimationFrameModel::getCommands()
{
    return &m_commands;
}

void SkeletalAnimationFrameModel::setNextFrame(SkeletalAnimationFrameModel *nextFrame)
{
    m_nextFrame = nextFrame;
}


/**                                 **/
/// SkeletalAnimationCommandModel   ///
/**                                 **/

SkeletalAnimationCommandModel::SkeletalAnimationCommandModel() :
    m_type(Unknown_Command),
    m_rate(0)
{
    m_amount = {0,0,0};
    m_enabledDirection = {false,false,false};
}

SkeletalAnimationCommandModel::~SkeletalAnimationCommandModel()
{
}

bool SkeletalAnimationCommandModel::loadFromXml(TiXmlElement *element)
{
    auto att = element->Attribute("type");
    if(att != nullptr)
    {
        std::string type = std::string(att);
        if(type.compare("moveTo") == 0)
            m_type = Move_To;
        else if(type.compare("rotateTo") == 0)
            m_type = Rotate_To;
        else if(type.compare("scaleTo") == 0)
            m_type = Scale_To;
        /**  Add more **/
    }
    else
        Logger::warning("There are animation commands without type");

    att = element->Attribute("node");
    if(att != nullptr)
        m_node = std::string(att);

    att = element->Attribute("x");
    if(att != nullptr)
        m_amount.x = Parser::parseFloat(att), m_enabledDirection.x = true;

    att = element->Attribute("y");
    if(att != nullptr)
        m_amount.y = Parser::parseFloat(att), m_enabledDirection.y = true;

    att = element->Attribute("z");
    if(att != nullptr)
        m_amount.z = Parser::parseFloat(att), m_enabledDirection.z = true;

    att = element->Attribute("rate");
    if(att != nullptr)
        m_rate = glm::abs(Parser::parseFloat(att));

    return (true);
}

SkelAnimCmdType SkeletalAnimationCommandModel::getType() const
{
    return m_type;
}

std::pair<const glm::vec3&,const glm::vec3&> SkeletalAnimationCommandModel::getAmount() const
{
    return {m_amount, m_enabledDirection};
}

float SkeletalAnimationCommandModel::getRate() const
{
    return m_rate;
}

const std::string &SkeletalAnimationCommandModel::getNode() const
{
    return m_node;
}

}
