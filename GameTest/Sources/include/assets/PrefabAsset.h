#ifndef PREFABASSET_H
#define PREFABASSET_H

#include "PouEngine/assets/Asset.h"

#include "tinyxml/tinyxml.h"

#include "world/WorldNode.h"
#include "world/WorldSprite.h"
#include "world/WorldMesh.h"
#include "world/PrefabInstance.h"
#include "assets/characterModelAsset.h"


class PrefabAsset : public pou::Asset
{
    public:
        PrefabAsset();
        PrefabAsset(const pou::AssetTypeId);
        virtual ~PrefabAsset();

        std::shared_ptr<PrefabInstance> generate();
        void generateOnNode(PrefabInstance *targetNode);

        bool loadFromFile(const std::string &filePath);

    protected:
        bool loadFromXML(TiXmlHandle *handle);

        bool loadSpriteSheet(TiXmlElement *element);

        bool loadNode(WorldNode *node, TiXmlElement *element);

        bool loadCollision(WorldNode *node, TiXmlElement *element);
        bool loadLight(WorldNode *node, TiXmlElement *element);
        bool loadMesh(WorldNode *node, TiXmlElement *element);
        bool loadSound(WorldNode *node, TiXmlElement *element);
        bool loadSprite(WorldNode *node, TiXmlElement *element);
        bool loadPrefab(WorldNode *node, TiXmlElement *element);

        bool loadCharacter(WorldNode *node, TiXmlElement *element); //This could be a lot of troubles

    private:
        WorldNode m_rootNode;

        std::map<std::string, pou::SpriteSheetAsset*>   m_spriteSheets;
};

#endif // PREFABASSET_H
