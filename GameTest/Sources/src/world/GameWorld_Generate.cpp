#include "world/GameWorld.h"

#include "PouEngine/Types.h"
#include "PouEngine/assets/AssetHandler.h"
#include "PouEngine/assets/SpriteSheetAsset.h"
#include "PouEngine/renderers/SceneRenderer.h"


void GameWorld::generate()
{
    this->createScene();

    m_dayTime = glm::linearRand(0,360);

    auto *grassSheet = pou::SpriteSheetsHandler::loadAssetFromFile("../data/grasslands/grassXML.txt");
    this->syncElement(grassSheet);

    auto *rockSheet = pou::SpriteSheetsHandler::loadAssetFromFile("../data/grasslands/rocksSpritesheetXML.txt");
    this->syncElement(rockSheet);

    for(auto x = -10 ; x < 10 ; x++)
    for(auto y = -10 ; y < 10 ; y++)
    {
        pou::SceneNode *grassNode = m_scene->getRootNode()->createChildNode(x*64,y*64);
        this->syncElement(grassNode);

        pou::SpriteEntity *spriteEntity = nullptr;

        int rd = x+y;//glm::linearRand(0,96);
        int modulo = 4;
        if(abs(rd % modulo) == 0)
            spriteEntity = m_scene->createSpriteEntity(grassSheet->getSpriteModel("grass1_2"));
        else if(abs(rd % modulo) == 1)
            spriteEntity = m_scene->createSpriteEntity(grassSheet->getSpriteModel("grass1_1"));
        else if(abs(rd % modulo) == 2)
            spriteEntity = m_scene->createSpriteEntity(grassSheet->getSpriteModel("grass1_3"));
        else if(abs(rd % modulo) == 3)
            spriteEntity = m_scene->createSpriteEntity(grassSheet->getSpriteModel("grass1_4"));

        grassNode->attachObject(spriteEntity);
        this->syncElement(spriteEntity);

        /*grassNode->setScale(glm::vec3(
                                glm::linearRand(0,10) > 5 ? 1 : -1,
                                1,//glm::linearRand(0,10) > 5 ? 1 : -1,
                                1));*/
    }


    auto treeModel = CharacterModelsHandler::loadAssetFromFile("../data/grasslands/treeXML.txt");
    this->syncElement(treeModel);

    for(auto x = -3 ; x < 3 ; x++)
    {
        glm::vec2 p = glm::vec2(glm::linearRand(-640,640), glm::linearRand(-640,640));
        if(x == 0)
            p = glm::vec2(30,0);

        auto tree = new Character();

        tree->setModel(treeModel);
        tree->setPosition(p);
        tree->rotate(glm::vec3(0,0,glm::linearRand(-180,180)));
        tree->scale(glm::vec3(
                    glm::linearRand(0,10) > 5 ? 1 : -1,
                    glm::linearRand(0,10) > 5 ? 1 : -1,
                    1));
        float red = glm::linearRand(1.0,1.0);
        float green = glm::linearRand(0.9,1.0);
        float blue = green;//glm::linearRand(0.9,1.0);
        tree->setColor(glm::vec4(red,green,blue,1));
        m_scene->getRootNode()->addChildNode(tree);

        this->syncElement(tree);
    }


    auto lanternModel = CharacterModelsHandler::loadAssetFromFile("../data/poleWithLantern/poleWithLanternXML.txt");
    this->syncElement(lanternModel);


    for(auto i = 0 ; i < 3 ; ++i)
    {
        glm::vec2 p = glm::vec2(glm::linearRand(-640,640), glm::linearRand(-640,640));
        auto *lantern = new Character();
        lantern->setModel(lanternModel);
        lantern->setPosition(p);
        lantern->rotate(glm::vec3(0,0,glm::linearRand(-180,180)));
        m_scene->getRootNode()->addChildNode(lantern);

        this->syncElement(lantern);
    }

    m_scene->update(pou::Time(0));
}

void GameWorld::destroy()
{
    m_curLocalTime = 0;

    m_syncNodes.clear();
    m_syncSpriteSheets.clear();
    m_syncSpriteEntities.clear();
    m_syncCharacterModels.clear();

    for(auto character : m_syncCharacters)
        delete character.second;
    m_syncCharacters.clear();

    if(m_scene)
        delete m_scene;
    m_scene = nullptr;
}


void GameWorld::createWorldInitializationMsg(std::shared_ptr<NetMessage_WorldInitialization> worldInitMsg)
{
    worldInitMsg->localTime = m_curLocalTime;
    worldInitMsg->dayTime = (int)m_dayTime;

    worldInitMsg->nbr_nodes = m_syncNodes.size();
    worldInitMsg->nodes.resize(worldInitMsg->nbr_nodes);
    size_t i = 0;
    for(auto it = m_syncNodes.begin() ; it != m_syncNodes.end() ; ++it, ++i)
    {
        //auto& [ nodeId, parentNodeId, nodePtr ] = worldInitMsg->nodes[i];

        auto &[ nodeId, nodeSync ] = worldInitMsg->nodes[i];

        nodeId = it->first;

        if(it->second->getParent() != m_scene->getRootNode())
            nodeSync.parentNodeId = m_syncNodes.findId((pou::SceneNode*)it->second->getParent());
        else
            nodeSync.parentNodeId = 0;

        nodeSync.node = it->second;
    }

    worldInitMsg->nbr_spriteSheets = m_syncSpriteSheets.size();
    worldInitMsg->spriteSheets.resize(worldInitMsg->nbr_nodes);
    i = 0;
    for(auto it = m_syncSpriteSheets.begin() ; it != m_syncSpriteSheets.end() ; ++it, ++i)
    {
        auto &[ spriteSheetId, spriteSheetPath ] = worldInitMsg->spriteSheets[i];
        spriteSheetId = it->first;

        spriteSheetPath = it->second->getFilePath();
    }

    worldInitMsg->nbr_spriteEntities = m_syncSpriteEntities.size();
    worldInitMsg->spriteEntities.resize(worldInitMsg->nbr_spriteEntities);
    i = 0;
    for(auto it = m_syncSpriteEntities.begin() ; it != m_syncSpriteEntities.end() ; ++it, ++i)
    {
        auto &[ spriteEntityId, spriteEntitySync ] = worldInitMsg->spriteEntities[i];
        spriteEntityId = it->first;

        auto spriteEntity = it->second;

        auto spriteModel = spriteEntity->getSpriteModel();
        if(spriteModel == nullptr)
            continue;

        auto spriteSheet = spriteModel->getSpriteSheet();
        if(spriteSheet == nullptr)
            continue;

        size_t spriteSheetId = m_syncSpriteSheets.findId(spriteSheet);

        size_t nodeId = 0;

        auto parentNode = spriteEntity->getParentNode();
        if(parentNode != nullptr)
            nodeId = m_syncNodes.findId(parentNode);

        spriteEntitySync.spriteSheetId = spriteSheetId;
        spriteEntitySync.spriteId = spriteModel->getSpriteId();
        spriteEntitySync.nodeId = nodeId;
    }


    worldInitMsg->nbr_characterModels = m_syncCharacterModels.size();
    worldInitMsg->characterModels.resize(worldInitMsg->nbr_characterModels);
    i = 0;
    for(auto it = m_syncCharacterModels.begin() ; it != m_syncCharacterModels.end() ; ++it, ++i)
    {
        auto &[ characterModelId, characterModelPath ] = worldInitMsg->characterModels[i];
        characterModelId = it->first;
        characterModelPath = it->second->getFilePath();
    }



    worldInitMsg->nbr_characters = m_syncCharacters.size();
    worldInitMsg->characters.resize(worldInitMsg->nbr_characters);
    i = 0;
    for(auto it = m_syncCharacters.begin() ; it != m_syncCharacters.end() ; ++it, ++i)
    {
        auto &[ characterId, characterSync ] = worldInitMsg->characters[i];
        characterId = it->first;

        auto character = it->second;

        size_t characterModelId = 0;
        auto characterModel = character->getModel();
        if(characterModel != nullptr)
            characterModelId = m_syncCharacterModels.findId(characterModel);

        size_t nodeId = 0;
        nodeId = m_syncNodes.findId((pou::SceneNode*)character);

        characterSync.characterModelId = characterModelId;
        characterSync.nodeId = nodeId;
    }
}

void GameWorld::generate(std::shared_ptr<NetMessage_WorldInitialization> worldInitMsg)
{
    this->createScene();

    m_curLocalTime = worldInitMsg->localTime;
    std::cout<<"Server local time:"<<m_curLocalTime<<std::endl;

    //Need to load this kind of info from WorldTemplate XML
    m_scene->getRootNode()->attachObject(m_sunLight);

    m_dayTime = worldInitMsg->dayTime;

    for(auto &characterModelit : worldInitMsg->characterModels)
    {
        auto& [ characterModelId, characterModelPath ] = characterModelit;
        auto *characterModelPtr = CharacterModelsHandler::loadAssetFromFile(characterModelPath);
        m_syncCharacterModels.insert(characterModelId, characterModelPtr);
    }

    for(auto &characterIt : worldInitMsg->characters)
    {
        auto& [ characterId, characterSync ] = characterIt;

        auto *characterModel = m_syncCharacterModels.findElement(characterSync.characterModelId);
        if(characterModel == nullptr)
            continue;

        if(characterSync.nodeId == 0)
            continue;

        characterSync.character->setModel(characterModel);
        m_scene->getRootNode()->addChildNode(characterSync.character);

        m_syncCharacters.insert(characterId, characterSync.character);
        m_syncNodes.insert(characterSync.nodeId , characterSync.character);
    }

    for(auto &node : worldInitMsg->nodes)
    {
        auto& [ nodeId, nodeSync ] = node;

        //If not nullptr, then the node is a Character
        auto nodePtr = m_syncNodes.findElement(nodeId);

        if(nodePtr == nullptr)
        {
            nodePtr = m_scene->getRootNode()->createChildNode();
            m_syncNodes.insert(nodeId, nodePtr);
        }

        //nodePtr->copyModifiersFrom(nodeSync.node);
        nodePtr->setPosition(nodeSync.node->getPosition());
        nodePtr->setRotation(nodeSync.node->getEulerRotation());
        nodePtr->setScale(nodeSync.node->getScale());
        nodePtr->setColor(nodeSync.node->getColor());

        delete nodeSync.node;
    }

    for(auto &node : worldInitMsg->nodes)
    {
        auto& [ nodeId, nodeSync ] = node;

        if(nodeSync.parentNodeId != 0)
        {
            auto nodePtr = m_syncNodes.findElement(nodeId);
            auto parentptr = m_syncNodes.findElement(nodeSync.parentNodeId);
            if(parentptr)
            {

                parentptr->addChildNode(nodePtr);
            }
        }
    }

    for(auto &spriteSheetIt : worldInitMsg->spriteSheets)
    {
        auto& [ spriteSheetId, spriteSheetPath ] = spriteSheetIt;
        auto *spriteSheetPtr = pou::SpriteSheetsHandler::loadAssetFromFile(spriteSheetPath);
        m_syncSpriteSheets.insert(spriteSheetId, spriteSheetPtr);
    }


    for(auto &spriteEntityIt : worldInitMsg->spriteEntities)
    {
        auto& [ spriteEntityId, spriteEntitySync ] = spriteEntityIt;

        /*m_syncSpriteEntities.insert(spriteEntityId, spriteEntitySync );*/

        auto *spriteSheetPtr = m_syncSpriteSheets.findElement(spriteEntitySync.spriteSheetId);
        if(spriteSheetPtr == nullptr)
            continue;

        auto *spriteEntityModel = spriteSheetPtr->getSpriteModel(spriteEntitySync.spriteId);
        auto *spriteEntity = m_scene->createSpriteEntity(spriteEntityModel);

        //if(spriteEntitySync.nodeId == 0)
            //m_scene->getRootNode()->attachObject(spriteEntity);
        //else
        if(spriteEntitySync.nodeId != 0)
        {
            auto *node = m_syncNodes.findElement(spriteEntitySync.nodeId);
            if(node == nullptr)
                continue;
            node->attachObject(spriteEntity);
        }

        m_syncSpriteEntities.insert(spriteEntityId, spriteEntity);
    }

    m_scene->update(pou::Time(0));
}


