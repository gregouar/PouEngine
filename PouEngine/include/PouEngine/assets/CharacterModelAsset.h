#ifndef CHARACTERMODELASSET_H
#define CHARACTERMODELASSET_H

#include "PouEngine/assets/Asset.h"

namespace pou
{

class CharacterModelAsset : public Asset
{
    public:
        CharacterModelAsset();
        CharacterModelAsset(const AssetTypeId);
        virtual ~CharacterModelAsset();

    protected:

    private:
};

}

#endif // CHARACTERMODELASSET_H
