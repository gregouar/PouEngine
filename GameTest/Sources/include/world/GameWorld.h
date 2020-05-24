#ifndef GAMEWORLD_H
#define GAMEWORLD_H

#include "PouEngine/scene/Scene.h"
#include "PouEngine/scene/SceneNode.h"
#include "PouEngine/scene/SceneEntity.h"
#include "PouEngine/utils/IdAllocator.h"
#include "PouEngine/renderers/RenderWindow.h"

#include "character/Character.h"
#include "net/NetMessageTypes.h"


class GameWorld
{
    public:
        GameWorld(bool renderable);
        virtual ~GameWorld();

        void update(const pou::Time elapsed_time);
        void render(pou::RenderWindow *renderWindow);

        void generate();
        void destroy();

        void createWorldInitializationMsg(std::shared_ptr<NetMessage_WorldInitialization> worldInitMsg);
        void generate(std::shared_ptr<NetMessage_WorldInitialization> worldInitMsg);

    protected:
        //size_t addSyncNode(pou::SceneNode *node);
        //size_t addSyncEntity(pou::SceneEntity *entity);

        void createScene();

        size_t syncElement(pou::SceneNode *node);
        size_t syncElement(pou::SpriteSheetAsset *spriteSheet);
        size_t syncElement(pou::SpriteEntity *spriteEntity);
        size_t syncElement(CharacterModelAsset *characterModel);
        size_t syncElement(Character *character);

        void updateSunLight(const pou::Time elapsed_time);

        /*size_t syncNode(pou::SceneNode *node);
        size_t syncNode(pou::SceneNode *node, size_t parentNodeId);
        size_t syncSpriteSheet(pou::SpriteSheetAsset *spriteSheet);
        size_t syncSpriteEntity(pou::SpriteEntity *spriteEntity);
        size_t syncSpriteEntity(pou::SpriteEntity *spriteEntity, size_t spriteSheetId);
        size_t syncSpriteEntity(pou::SpriteEntity *spriteEntity, size_t spriteSheetId, size_t nodeId);*/

       // pou::SceneNode* createNode(pou::SceneNode* parentNode, bool sync = false);
        //std::pair<size_t, pou::SpriteSheetAsset*> syncSpriteSheet(const std::string &path);
       /* std::pair<size_t, SpriteEntitySync> createSpriteEntitySync(size_t spriteSheetId, const std::string &spriteName);
        std::pair<size_t, SpriteEntitySync> createSpriteEntitySync(std::pair<size_t, pou::SpriteSheetAsset*> spriteSheetPair, const std::string &spriteName);*/
        //pou::SpriteEntity* createSpriteEntity();


    private:
        pou::Scene *m_scene;
        bool m_isRenderable;

        float m_curLocalTime;

        pou::CameraObject *m_camera;
        pou::LightEntity  *m_sunLight;
        float              m_dayTime; //Between 0 and 360

        pou::IdAllocator<pou::SceneNode*>           m_syncNodes;
        pou::IdAllocator<pou::SpriteSheetAsset*>    m_syncSpriteSheets;
        pou::IdAllocator<pou::SpriteEntity*>        m_syncSpriteEntities;
        pou::IdAllocator<CharacterModelAsset*>      m_syncCharacterModels;
        pou::IdAllocator<Character*>                m_syncCharacters;

    public:
        static const glm::vec3  GAMEWORLD_MAX_SIZE; //Used for - and +

        static const float      NODE_MAX_SCALE;
        static const uint8_t    NODE_SCALE_DECIMALS;

        static const int        NODEID_BITS;
        static const int        SPRITESHEETID_BITS;
        static const int        SPRITEENTITYID_BITS;
        static const int        CHARACTERMODELSID_BITS;
        static const int        CHARACTERSID_BITS;


};

#endif // GAMEWORLD_H
