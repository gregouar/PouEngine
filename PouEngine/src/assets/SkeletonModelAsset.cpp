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

const SimpleNode* SkeletonModelAsset::getRootNode() const
{
    return &m_rootNode;
}


std::map<std::string, SimpleNode*> SkeletonModelAsset::getNodesByName()
{
    return m_nodesByName;
}

}
