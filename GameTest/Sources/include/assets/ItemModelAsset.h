#ifndef ITEMMODELASSET_H
#define ITEMMODELASSET_H

#include "PouEngine/assets/Asset.h"

#include "tinyxml/tinyxml.h"

#include "character/Hitbox.h"
#include "Types.h"

#include "assets/CharacterModelAsset.h"
#include "character/Character.h"

#include <list>

struct ItemAttributes
{
    float attackDamages;
    float attackDelay;
    GearType type;
    //float armor;
};

class ItemModelAsset : public pou::Asset
{
    public:
        ItemModelAsset();
        ItemModelAsset(const pou::AssetTypeId);
        virtual ~ItemModelAsset();

        bool loadFromFile(const std::string &filePath);

        bool removeFromCharacter(Character *character);
        bool generateOnCharacter(Character *character);

        const ItemAttributes &getAttributes() const;
        const std::list<Hitbox> *getHitboxes() const;

    protected:
        bool loadFromXML(TiXmlHandle *);
        bool loadSpriteSheet(TiXmlElement *element);
        bool loadLightModel(TiXmlElement *element);
        bool loadSoundBank(TiXmlElement *element);
        bool loadSkeleton(TiXmlElement *element);
        bool loadHitboxes(TiXmlElement *element, std::list<Hitbox> &boxList);
        bool loadAttributes(TiXmlElement *element);

    private:
        std::unordered_map<pou::HashedString, pou::SpriteSheetAsset*> m_spriteSheets;
        std::unordered_map<pou::HashedString, pou::LightModel>        m_lightModels;
        std::unordered_map<pou::HashedString, AssetsForSkeletonModel> m_skeletonAssetsModels;

        ItemAttributes m_attributes;

        std::list<Hitbox> m_hitboxes;
};

#endif // ITEMMODELASSET_H
