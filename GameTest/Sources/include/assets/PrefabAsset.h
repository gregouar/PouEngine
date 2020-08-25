#ifndef PREFABASSET_H
#define PREFABASSET_H

#include "PouEngine/assets/Asset.h"

#include "tinyxml/tinyxml.h"

#include "PouEngine/scene/SceneNode.h"
#include "PouEngine/scene/SpriteEntity.h"
#include "PouEngine/scene/MeshEntity.h"
#include "world/PrefabInstance.h"
#include "assets/characterModelAsset.h"

struct PrefabCharacter
{
    CharacterModelAsset *modelAsset;
    pou::TransformComponent transform;
};

class PrefabAsset : public pou::Asset
{
    public:
        PrefabAsset();
        PrefabAsset(const pou::AssetTypeId);
        virtual ~PrefabAsset();

        std::shared_ptr<PrefabInstance> generate();
        void generatesToNode(PrefabInstance *targetNode);
        void generateCharacters(pou::SceneNode *targetNode, pou::TransformComponent *transform);

        bool loadFromFile(const std::string &filePath);

    protected:
        bool loadFromXML(TiXmlHandle *handle);

        bool loadSpriteSheet(TiXmlElement *element);

        bool loadNode(pou::SceneNode *node, TiXmlElement *element);

        bool loadCollision(pou::SceneNode *node, TiXmlElement *element);
        bool loadLight(pou::SceneNode *node, TiXmlElement *element);
        bool loadMesh(pou::SceneNode *node, TiXmlElement *element);
        bool loadSound(pou::SceneNode *node, TiXmlElement *element);
        bool loadSprite(pou::SceneNode *node, TiXmlElement *element);
        bool loadPrefab(pou::SceneNode *node, TiXmlElement *element);

        bool loadCharacter(pou::SceneNode *node, TiXmlElement *element); //This could be a lot of troubles

    private:
        pou::SceneNode m_rootNode;

        std::unordered_map<pou::HashedString, pou::SpriteSheetAsset*>   m_spriteSheets;
        std::list<PrefabCharacter> m_prefabCharacters;
};

#endif // PREFABASSET_H
