#include "assets/AiScriptModelAsset.h"

AiScriptModelAsset::AiScriptModelAsset() : AiScriptModelAsset(-1)
{
}

AiScriptModelAsset::AiScriptModelAsset(const pou::AssetTypeId id) : Asset(id)
{
    m_allowLoadFromFile     = true;
    m_allowLoadFromMemory   = false;
}


AiScriptModelAsset::~AiScriptModelAsset()
{
    //dtor
}

bool AiScriptModelAsset::loadFromFile(const std::string &filePath)
{
   return (true);
}
