#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED

class CharacterModelAsset;
class ItemModelAsset;

enum DamageType
{
    DamageType_Cut,
    DamageType_Blunt,
    DamageType_Pierce,
    DamageType_Axe,
    DamageType_Pickaxe,
    NBR_DAMAGE_TYPES,
};

enum GearType
{
    GearType_Weapon,
    NBR_GEAR_TYPES,
};

template<class AssetType> class AssetHandler;
typedef pou::AssetHandler<CharacterModelAsset>  CharacterModelsHandler;
typedef pou::AssetHandler<ItemModelAsset>       ItemModelsHandler;

#endif // TYPES_H_INCLUDED
