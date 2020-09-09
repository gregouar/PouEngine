#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED

class CharacterModelAsset;
class ItemModelAsset;
class AiScriptModelAsset;
class PrefabAsset;

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

enum Predefined_AiComponent_Type
{
    Predefined_AiComponent_AutoLight,
    NBR_Predefined_AiComponent_Types,
};

template<class AssetType> class AssetHandler;
typedef pou::AssetHandler<CharacterModelAsset>  CharacterModelsHandler;
typedef pou::AssetHandler<ItemModelAsset>       ItemModelsHandler;
typedef pou::AssetHandler<AiScriptModelAsset>   AiScriptModelsHandler;
typedef pou::AssetHandler<PrefabAsset>          PrefabsHandler;

#endif // TYPES_H_INCLUDED
