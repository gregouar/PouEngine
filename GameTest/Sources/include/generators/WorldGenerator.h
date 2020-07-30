#ifndef WORLDGENERATOR_H
#define WORLDGENERATOR_H

#include "generators/TerrainGenerator.h"
#include "audio/SoundModel.h"

struct WorldGenerator_Parameters
{
    SoundModel musicModel;
};

class WorldGenerator
{
    public:
        WorldGenerator();
        virtual ~WorldGenerator();

        bool loadFromFile(const std::string &filePath);

        void generatesOnNode(WorldNode *targetNode, int seed, GameWorld_Sync *syncComponent);

        void playWorldMusic();

        const std::string &getFilePath();
        int getGeneratingSeed();

    protected:
        bool loadFromXML(TiXmlHandle *hdl);

        bool loadParameters(TiXmlElement *element);

    private:
        pou::RNGenerator m_rng;

        std::string m_filePath;
        std::string m_fileDirectory;

        int m_generatingSeed;

        WorldGenerator_Parameters m_parameters;
        TerrainGenerator m_terrainGenerator;
};

#endif // WORLDGENERATOR_H
