#include "PouEngine/assets/SkeletonAsset.h"

#include "PouEngine/Types.h"

#include "PouEngine/assets/AssetHandler.h"
#include "PouEngine/assets/TextureAsset.h"

#include "PouEngine/utils/Logger.h"
#include "PouEngine/utils/Parser.h"

namespace pou
{


SkeletonAsset::SkeletonAsset() : SkeletonAsset(-1)
{

}

SkeletonAsset::SkeletonAsset(const AssetTypeId id) : Asset(id), m_rootNode(0)
{
    m_allowLoadFromFile     = true;
    m_allowLoadFromMemory   = false;
}

SkeletonAsset::~SkeletonAsset()
{
    //dtor
}


bool SkeletonAsset::loadFromFile(const std::string &filePath)
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


bool SkeletonAsset::loadFromXML(TiXmlHandle *hdl)
{
    bool loaded = true;

    if(hdl == nullptr)
        return (false);

    //TiXmlElement *nodeElement =  hdl->FirstChildElement("node").Element();
    this->loadNode(&m_rootNode, hdl->FirstChildElement("node").Element());

    /*if(hdl->Element()->Attribute("texture") == nullptr)
        return (false);

    std::string textureName = std::string(hdl->Element()->Attribute("texture"));

    m_texture = TexturesHandler::instance()
                    ->loadAssetFromFile(m_fileDirectory+textureName,m_loadType);
    this->startListeningTo(m_texture);
    if(!m_texture->isLoaded())
        loaded = false;


    TiXmlElement* spriteElement = hdl->FirstChildElement("sprite").Element();
    int i = 0;
    while(spriteElement != nullptr)
    {
        std::unique_ptr<SpriteModel> spriteModel(new SpriteModel());

        std::string spriteName = std::to_string(i);
        glm::vec2 spriteSize(0,0);
        glm::vec2 spriteCenter(0,0);
        glm::vec2 spritePosition(0,0);
        bool customSize = false;
        bool customCenter = false;
        bool customPosition = false;

        if(spriteElement->Attribute("name") != nullptr)
            spriteName = std::string(spriteElement->Attribute("name"));

        auto sizeElement = spriteElement->FirstChildElement("size");
        if(sizeElement != nullptr)
        {
            if(sizeElement->Attribute("x") != nullptr)
                spriteSize.x = Parser::parseFloat(std::string(sizeElement->Attribute("x")));
            if(sizeElement->Attribute("y") != nullptr)
                spriteSize.y = Parser::parseFloat(std::string(sizeElement->Attribute("y")));

            customSize = true;
        }

        auto centerElement = spriteElement->FirstChildElement("center");
        if(centerElement != nullptr)
        {

            if(centerElement->Attribute("x") != nullptr)
                spriteCenter.x = Parser::parseFloat(std::string(centerElement->Attribute("x")));
            if(centerElement->Attribute("y") != nullptr)
                spriteCenter.y = Parser::parseFloat(std::string(centerElement->Attribute("y")));

            customCenter = true;
        }

        auto positionElement = spriteElement->FirstChildElement("position");
        if(positionElement != nullptr)
        {
            if(positionElement->Attribute("x") != nullptr)
                spritePosition.x = Parser::parseFloat(std::string(positionElement->Attribute("x")));
            if(positionElement->Attribute("y") != nullptr)
                spritePosition.y = Parser::parseFloat(std::string(positionElement->Attribute("y")));

            customPosition = true;
        }


        spriteModel->setTexture(m_texture);

        if(!customSize)
            spriteSize = m_texture->getExtent();
        if(!customCenter)
            spriteCenter = {spriteSize.x*0.5,
                            spriteSize.y*0.5};

        spriteModel->setSize(spriteSize);
        spriteModel->setCenter(spriteCenter);

        if(customPosition)
            spriteModel->setTextureRect(spritePosition,spriteSize,false);

        if(!m_sprites.insert(std::make_pair(spriteName,std::move(spriteModel))).second)
            Logger::error("Multiple sprites named \""+spriteName+"\" in the sprite sheet : "+m_filePath);

        spriteElement = spriteElement->NextSiblingElement("sprite");
        ++i;
    }*/

    if(loaded)
        Logger::write("Skeleton loaded from file: "+m_filePath);

    return (loaded);
}

void SkeletonAsset::loadNode(SimpleNode* rootNode, TiXmlElement *element)
{
    std::string nodeName = "node";
    glm::vec3 nodePos(0,0,0);

    auto nameAtt = element->Attribute("name");
    if(nameAtt != nullptr)
        nodeName = std::string(nameAtt);

    if(!m_nodes.insert({nodeName,rootNode}).second)
        Logger::warning("Multiple nodes with the same name \"" + nodeName + "\" in Skeleton Asset : "+m_filePath);

    auto attribute = sizeElement->Attribute("x");
    if(attribute != nullptr)
        nodePos.x = Parser::parseFloat(std::string(attribute));

    attribute = sizeElement->Attribute("y");
    if(attribute != nullptr)
        nodePos.y = Parser::parseFloat(std::string(attribute));

    attribute = sizeElement->Attribute("z");
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


}
