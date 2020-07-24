#ifndef TERRAINLAYERMODELASSET_H
#define TERRAINLAYERMODELASSET_H

#include "PouEngine/assets/Asset.h"
#include "PouEngine/assets/SpriteSheetAsset.h"

#include "generators/TerrainGenerator_TileModels.h"

enum TerrainGenerator_BorderType
{
    TerrainGenerator_BorderType_Blank = 0,
    TerrainGenerator_BorderType_SmallCorner_TL,
    TerrainGenerator_BorderType_SmallCorner_TR,
    TerrainGenerator_BorderType_Side_T,
    TerrainGenerator_BorderType_SmallCorner_BL,
    TerrainGenerator_BorderType_Side_L,
    TerrainGenerator_BorderType_Diag_BL_TR,
    TerrainGenerator_BorderType_BigCorner_BR,
    TerrainGenerator_BorderType_SmallCorner_BR,
    TerrainGenerator_BorderType_Diag_TL_BR,
    TerrainGenerator_BorderType_Side_R,
    TerrainGenerator_BorderType_BigCorner_BL,
    TerrainGenerator_BorderType_Side_B,
    TerrainGenerator_BorderType_BigCorner_TR,
    TerrainGenerator_BorderType_BigCorner_TL,
    TerrainGenerator_BorderType_Fill,
    NBR_BORDER_TYPES,
};


class TerrainLayerModelAsset : public pou::Asset
{
    public:
        TerrainLayerModelAsset();
        TerrainLayerModelAsset(const pou::AssetTypeId);
        virtual ~TerrainLayerModelAsset();

        bool loadFromFile(const std::string &filePath);

        pou::SpriteModel *getSpriteModel(const std::string &spriteSheetName, const std::string &spriteName);

        std::shared_ptr<WorldSprite> generateSprite(TerrainGenerator_BorderType borderType, pou::RNGenerator *rng = nullptr);

        float getSpawnPointSparsity();
        float getSpawnProbability();
        float getExpandProbability();

    protected:
        bool loadFromXML(TiXmlHandle *);
        bool loadSpriteSheet(TiXmlElement *element);
        bool loadTileModels(TiXmlElement *element);

    private:
        std::map<std::string, pou::SpriteSheetAsset*>  m_spriteSheets;
        TerrainGenerator_TileModels m_tileModels[NBR_BORDER_TYPES];

        ///I think this should be move somewhere else
        float m_spawnPointSparsity;
        float m_spawnProbability;
        float m_expandProbability;
};

#endif // TERRAINLAYERMODELASSET_H
