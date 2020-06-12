#ifndef AISCRIPTMODELASSET_H
#define AISCRIPTMODELASSET_H

#include "PouEngine/assets/Asset.h"

class AiScriptModelAsset : public pou::Asset
{
    public:
        AiScriptModelAsset();
        AiScriptModelAsset(const pou::AssetTypeId);
        virtual ~AiScriptModelAsset();

        bool loadFromFile(const std::string &filePath);

    protected:

    private:
};

#endif // AISCRIPTMODELASSET_H
