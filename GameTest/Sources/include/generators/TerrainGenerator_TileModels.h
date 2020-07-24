#ifndef TERRAINGENERATOR_TILEMODELS_H
#define TERRAINGENERATOR_TILEMODELS_H

#include "tinyxml/tinyxml.h"
#include "world/WorldSprite.h"

#include "PouEngine/tools/RNGesus.h"

#include <memory>

class TerrainLayerModelAsset;

class TerrainGenerator_TileModels
{
    public:
        TerrainGenerator_TileModels();
        virtual ~TerrainGenerator_TileModels();

        bool loadFromXML(TiXmlElement *element, TerrainLayerModelAsset *parentLayerModel);

        void addTileModel(float probability, std::shared_ptr<WorldSprite> spriteModel);
        std::shared_ptr<WorldSprite> generateSprite(pou::RNGenerator *rng = nullptr);

    protected:

    private:
        std::map<float, std::shared_ptr<WorldSprite> > m_spriteMap;
        float m_totalProbability;
};

#endif // TERRAINGENERATOR_TILEMODELS_H
