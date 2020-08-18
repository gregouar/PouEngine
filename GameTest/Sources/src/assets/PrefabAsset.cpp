#include "assets/PrefabAsset.h"

#include "PouEngine/tools/Logger.h"
#include "PouEngine/tools/Parser.h"

#include "PouEngine/assets/MeshesHandler.h"
#include "PouEngine/scene/CollisionObject.h"

#include "character/Character.h"

PrefabAsset::PrefabAsset() : PrefabAsset(-1)
{
}

PrefabAsset::PrefabAsset(const pou::AssetTypeId id) : Asset(id)
{
    m_allowLoadFromFile     = true;
    m_allowLoadFromMemory   = false;
}


PrefabAsset::~PrefabAsset()
{
    //dtor
}

std::shared_ptr<PrefabInstance> PrefabAsset::generate()
{
    auto newNode = std::make_shared<PrefabInstance>();
    this->generateOnNode(newNode.get());
    return newNode;
}

void PrefabAsset::generateOnNode(PrefabInstance *targetNode)
{
    targetNode->copyFrom(&m_rootNode);
    targetNode->setPrefabModel(this);
}

bool PrefabAsset::loadFromFile(const std::string &filePath)
{
    TiXmlDocument file(filePath.c_str());

    if(!file.LoadFile())
    {
        pou::Logger::error("Cannot load prefab from file: "+filePath);
        std::ostringstream errorReport;
        errorReport << "Because: "<<file.ErrorDesc();
        pou::Logger::error(errorReport);
        return (false);
    }

    TiXmlHandle hdl(&file);
    hdl = hdl.FirstChildElement();

    return this->loadFromXML(&hdl);
}

bool PrefabAsset::loadFromXML(TiXmlHandle *hdl)
{
    bool loaded = true;

    if(hdl == nullptr)
        return (false);

    TiXmlElement* element;

    element = hdl->FirstChildElement("spritesheet").Element();
    while(element != nullptr)
    {
        if(!this->loadSpriteSheet(element))
            loaded = false;
        element = element->NextSiblingElement("spritesheet");
    }

    element = hdl->FirstChildElement("node").Element();
    if(element != nullptr)
    {
        if(!this->loadNode(&m_rootNode, element))
            loaded = false;
    }

    /*auto characterElement = element->FirstChildElement("character");
    while(characterElement)
    {
        this->loadCharacter(rootNode, characterElement);
        characterElement = characterElement->NextSiblingElement("character");
    }*/


    if(loaded)
        pou::Logger::write("Prefab model loaded from file: "+m_filePath);

    return (loaded);
}

bool PrefabAsset::loadSpriteSheet(TiXmlElement *element)
{
    std::string spriteSheetName = "spriteSheet"+std::to_string(m_spriteSheets.size());

    auto nameAtt = element->Attribute("name");
    if(nameAtt != nullptr)
        spriteSheetName = std::string(nameAtt);


    auto pathAtt = element->Attribute("path");
    if(pathAtt == nullptr)
        return (false);

    if(!m_spriteSheets.insert({spriteSheetName,
                              pou::SpriteSheetsHandler::loadAssetFromFile(m_fileDirectory+std::string(pathAtt), m_loadType)}).second)
        pou::Logger::warning("Multiple spritesheets with name \""+spriteSheetName+"\" in prefab:"+m_filePath);

    return (true);
}


bool PrefabAsset::loadNode(pou::SceneNode* rootNode, TiXmlElement *element)
{
    std::string nodeName;
    glm::vec3 nodePos(0,0,0);

    auto nameAtt = element->Attribute("name");
    if(nameAtt != nullptr)
    {
        nodeName = std::string(nameAtt);
        rootNode->setName(nodeName);
    }

    /*int nodeId = -1;
    if(!nodeName.empty())
    {
        if(!m_nodesByName.insert({nodeName,rootNode}).second)
            Logger::warning("Multiple nodes with the same name \"" + nodeName + "\" in Skeleton Asset : "+m_filePath);

        nodeId = this->generateNodeId(nodeName);
        m_nodesById.insert({nodeId, rootNode});
    }*/

    auto attribute = element->Attribute("x");
    if(attribute != nullptr)
        nodePos.x = pou::Parser::parseFloat(std::string(attribute));

    attribute = element->Attribute("y");
    if(attribute != nullptr)
        nodePos.y = pou::Parser::parseFloat(std::string(attribute));

    attribute = element->Attribute("z");
    if(attribute != nullptr)
        nodePos.z = pou::Parser::parseFloat(std::string(attribute));


    /*attribute = element->Attribute("state");
    if(attribute != nullptr)
    {
        int stateId = this->generateStateId(std::string(attribute));
        m_initialStates.push_back({nodeId, stateId});
    }

    attribute = element->Attribute("rigidity");
    if(attribute != nullptr)
    {
        float rigidity = Parser::parseFloat(std::string(attribute));
        rootNode->setRigidity(rigidity);
    }*/

    rootNode->transform()->setPosition(nodePos);

    auto rotElement = element->FirstChildElement("rotate");
    if(rotElement != nullptr)
    {
        glm::vec3 rotation = glm::vec3(0);
        auto att = rotElement->Attribute("x");
        if(att != nullptr)
            rotation.x = pou::Parser::parseFloat(att);

        att = rotElement->Attribute("y");
        if(att != nullptr)
            rotation.y = pou::Parser::parseFloat(att);

        att = rotElement->Attribute("z");
        if(att != nullptr)
            rotation.z = pou::Parser::parseFloat(att);

        rootNode->transform()->rotateInDegrees(rotation);
    }

    auto scaleElement = element->FirstChildElement("scale");
    if(scaleElement != nullptr)
    {
        glm::vec3 scale = glm::vec3(1);
        auto att = scaleElement->Attribute("x");
        if(att != nullptr)
            scale.x = pou::Parser::parseFloat(att);

        att = scaleElement->Attribute("y");
        if(att != nullptr)
            scale.y = pou::Parser::parseFloat(att);

        att = scaleElement->Attribute("z");
        if(att != nullptr)
            scale.z = pou::Parser::parseFloat(att);

        rootNode->transform()->scale(scale);
    }


    auto collisionElement = element->FirstChildElement("collision");
    while(collisionElement)
    {
        this->loadCollision(rootNode, collisionElement);
        collisionElement = collisionElement->NextSiblingElement("collision");
    }

    auto lightElement = element->FirstChildElement("light");
    while(lightElement)
    {
        this->loadLight(rootNode, lightElement);
        lightElement = lightElement->NextSiblingElement("light");
    }

    auto meshElement = element->FirstChildElement("mesh");
    while(meshElement)
    {
        this->loadMesh(rootNode, meshElement);
        meshElement = meshElement->NextSiblingElement("mesh");
    }

    auto soundElement = element->FirstChildElement("sound");
    while(soundElement)
    {
        this->loadSound(rootNode, soundElement);
        soundElement = soundElement->NextSiblingElement("sound");
    }

    auto spriteElement = element->FirstChildElement("sprite");
    while(spriteElement)
    {
        this->loadSprite(rootNode, spriteElement);
        spriteElement = spriteElement->NextSiblingElement("sprite");
    }

    auto prefabElement = element->FirstChildElement("prefab");
    while(prefabElement)
    {
        this->loadPrefab(rootNode, prefabElement);
        prefabElement = prefabElement->NextSiblingElement("prefab");
    }

    auto child = element->FirstChildElement("node");
    if(child == nullptr)
        return (true);

    TiXmlElement* childElement = child->ToElement();
    while(childElement != nullptr)
    {
        auto node = rootNode->createChildNode();
        this->loadNode(node.get(), childElement);

        childElement = childElement->NextSiblingElement("node");
    }

    return (true);
}

bool PrefabAsset::loadCollision(pou::SceneNode *node, TiXmlElement *element)
{
    auto typeAtt = element->Attribute("type");
    if(typeAtt == nullptr)
        return (false);


    auto collisionObject = std::make_shared<pou::CollisionObject>();

    if(std::string(typeAtt) == "box")
    {
        pou::MathTools::Box box;

        auto sizeElement = element->FirstChildElement("size");
        if(sizeElement != nullptr)
        {
            if(sizeElement->Attribute("x") != nullptr)
                box.size.x = pou::Parser::parseFloat(std::string(sizeElement->Attribute("x")));
            if(sizeElement->Attribute("y") != nullptr)
                box.size.y = pou::Parser::parseFloat(std::string(sizeElement->Attribute("y")));
        }

        auto centerElement = element->FirstChildElement("center");
        if(centerElement != nullptr)
        {
            if(centerElement->Attribute("x") != nullptr)
                box.center.x = pou::Parser::parseFloat(std::string(centerElement->Attribute("x")));
            if(centerElement->Attribute("y") != nullptr)
                box.center.y = pou::Parser::parseFloat(std::string(centerElement->Attribute("y")));
        }

        collisionObject->setBox(box);
    }
    else if(std::string(typeAtt) == "disk")
    {
        float radius = 0;

        auto radiusChild = element->FirstChildElement("radius");
        if(radiusChild != nullptr)
            radius = pou::Parser::parseFloat(radiusChild->GetText());

        collisionObject->setDisk(radius);
    }
    else
        return (false);

    float mass = 0;

    auto massChild = element->FirstChildElement("mass");
    if(massChild != nullptr)
        mass = pou::Parser::parseFloat(massChild->GetText());

    collisionObject->setMass(mass);
    node->attachObject(collisionObject);

    return (true);
}

bool PrefabAsset::loadLight(pou::SceneNode *node, TiXmlElement *element)
{
    pou::LightModel lightModel;
    lightModel.loadFromXML(element);

    auto lightEntity = std::make_shared<pou::LightEntity>();
    lightEntity->setModel(lightModel);
    node->attachObject(lightEntity);

    return (true);
}


bool PrefabAsset::loadMesh(pou::SceneNode *node, TiXmlElement *element)
{
    auto pathAtt = element->Attribute("path");
    if(pathAtt == nullptr)
        return (false);

    auto meshModel = pou::MeshesHandler::loadAssetFromFile(m_fileDirectory + std::string(pathAtt), pou::LoadType_InThread);

    auto mesh = std::make_shared<WorldMesh>();
    mesh->setMeshModel(meshModel);
    mesh->setShadowCastingType(pou::ShadowCasting_All);
    node->attachObject(mesh);

    return (true);
}


bool PrefabAsset::loadSound(pou::SceneNode *node, TiXmlElement *element)
{
    return (true);
}


bool PrefabAsset::loadSprite(pou::SceneNode *node, TiXmlElement *element)
{
    auto spriteAtt      = element->Attribute("sprite");
    auto spriteSheetAtt = element->Attribute("spritesheet");

    if(spriteAtt == nullptr || spriteSheetAtt == nullptr)
        return (false);

    auto sprite = std::make_shared<WorldSprite>();

    auto spritesheetIt = m_spriteSheets.find(std::string(spriteSheetAtt));
    if(spritesheetIt != m_spriteSheets.end())
    {
        auto spriteModel = spritesheetIt->second->getSpriteModel(std::string(spriteAtt));
        if(spriteModel == nullptr)
        {
            pou::Logger::warning("Sprite named \""+std::string(spriteAtt)+"\" not found in spritesheet \""
                            +std::string(spriteSheetAtt)+"\"");
            return (false);
        }
        sprite->setSpriteModel(spriteModel);
    } else {
        pou::Logger::warning("Spritesheet named \""+std::string(spriteSheetAtt)+"\" not found");
        return (false);
    }

    node->attachObject(sprite);

    return (true);
}


bool PrefabAsset::loadPrefab(pou::SceneNode *node, TiXmlElement *element)
{
    auto pathAtt = element->Attribute("path");
    if(pathAtt == nullptr)
        return (false);

    auto prefabModel = PrefabsHandler::loadAssetFromFile(m_fileDirectory + std::string(pathAtt));

    auto newNode = prefabModel->generate();
    node->addChildNode(newNode);

    return (true);
}


bool PrefabAsset::loadCharacter(pou::SceneNode *node, TiXmlElement *element)
{
    auto pathAtt = element->Attribute("path");
    if(pathAtt == nullptr)
        return (false);

    //auto characterModel = CharacterModelsHandler::loadAssetFromFile(m_fileDirectory + std::string(pathAtt));
    auto character = std::make_shared<Character>();
    if(character->createFromModel(m_fileDirectory + std::string(pathAtt)))
        node->addChildNode(character);

    return (true);
}




