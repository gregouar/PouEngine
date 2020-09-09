#ifndef TERRAINGENERATOR_TILEMODELS_H
#define TERRAINGENERATOR_TILEMODELS_H

#include "tinyxml/tinyxml.h"

#include "PouEngine/tools/RNGesus.h"
#include "PouEngine/scene/SpriteEntity.h"
#include "assets/PrefabAsset.h"

#include <memory>

class TerrainLayerModelAsset;

struct TerrainGenerator_TileSpriteModel
{
    std::shared_ptr<pou::SpriteEntity> spriteModel;
    bool allowFlipX;
    bool allowFlipY;
};

struct TerrainGenerator_TilePrefabModel
{
    PrefabAsset* prefabAsset;
    bool allowFlipX;
    bool allowFlipY;
    float rotation;
};

class TerrainGenerator_TileModels
{
    public:
        TerrainGenerator_TileModels();
        virtual ~TerrainGenerator_TileModels();

        bool loadFromXML(TiXmlElement *element, TerrainLayerModelAsset *parentLayerModel);

        void addTileSpriteModel(float probability, TerrainGenerator_TileSpriteModel tileModel);
        void addTilePrefabModel(float probability, TerrainGenerator_TilePrefabModel tileModel);

        void generatesOnNode(pou::SceneNode *targetNode, GameWorld_Sync *syncComponent,
                             int modValue = -1, pou::RNGenerator *rng = nullptr);

    protected:
        void loadSpriteElement(TiXmlElement *element, TerrainLayerModelAsset *parentLayerModel);
        void loadPrefabElement(TiXmlElement *element, TerrainLayerModelAsset *parentLayerModel);

        std::shared_ptr<pou::SpriteEntity> generateSprite(int modValue, pou::RNGenerator *rng);
        std::shared_ptr<PrefabInstance> generatePrefab(int modValue, pou::RNGenerator *rng);

    private:
        /**bool m_chosenSpriteMap; ///We use two maps in order to add dithering
        std::map<float, TerrainGenerator_TileModel > m_spriteMap1;
        float m_totalProbability1;

        std::map<float, TerrainGenerator_TileModel > m_spriteMap2;
        float m_totalProbability2;**/

        std::map<float, TerrainGenerator_TileSpriteModel > m_spriteMap;
        float m_totalSpriteProbability;

        std::map<float, TerrainGenerator_TilePrefabModel > m_prefabMap;
        float m_totalPrefabProbability;

};
#endif // TERRAINGENERATOR_TILEMODELS_H
