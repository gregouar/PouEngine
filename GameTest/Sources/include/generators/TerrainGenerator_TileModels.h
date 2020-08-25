#ifndef TERRAINGENERATOR_TILEMODELS_H
#define TERRAINGENERATOR_TILEMODELS_H

#include "tinyxml/tinyxml.h"

#include "PouEngine/tools/RNGesus.h"
#include "PouEngine/scene/SpriteEntity.h"

#include <memory>

class TerrainLayerModelAsset;

struct TerrainGenerator_TileModel
{
    std::shared_ptr<pou::SpriteEntity> spriteModel;
    bool allowFlipX;
    bool allowFlipY;
};

class TerrainGenerator_TileModels
{
    public:
        TerrainGenerator_TileModels();
        virtual ~TerrainGenerator_TileModels();

        bool loadFromXML(TiXmlElement *element, TerrainLayerModelAsset *parentLayerModel);

        void addTileModel(float probability, TerrainGenerator_TileModel tileModel);
        std::shared_ptr<pou::SpriteEntity> generateSprite(int modValue = -1, pou::RNGenerator *rng = nullptr);

    protected:

    private:
        bool m_chosenSpriteMap;
        std::map<float, TerrainGenerator_TileModel > m_spriteMap1;
        float m_totalProbability1;

        std::map<float, TerrainGenerator_TileModel > m_spriteMap2;
        float m_totalProbability2;
};

#endif // TERRAINGENERATOR_TILEMODELS_H
