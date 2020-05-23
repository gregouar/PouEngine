#ifndef GAMEWORLD_H
#define GAMEWORLD_H

#include "PouEngine/scene/Scene.h"
#include "PouEngine/scene/SceneNode.h"
#include "PouEngine/scene/SceneEntity.h"
#include "PouEngine/utils/IdAllocator.h"

#include "character/Character.h"
#include "net/NetMessageTypes.h"

class GameWorld
{
    public:
        GameWorld(bool renderable);
        virtual ~GameWorld();

        void update(const pou::Time elapsed_time);

        void generate();
        void destroy();

        void createWorldInitializationMsg(std::shared_ptr<NetMessage_WorldInitialization> worldInitMsg);
        void readWorldInitializationMsg(std::shared_ptr<NetMessage_WorldInitialization> worldInitMsg);

    protected:
        size_t addSyncNode(pou::SceneNode *node);
        size_t addSyncEntity(pou::SceneEntity *entity);

        pou::SceneNode* createNode(pou::SceneNode* parentNode, bool sync = false);
        pou::SpriteEntity* createSpriteEntity();

    private:
        pou::Scene *m_scene;
        bool m_isRenderable;


        pou::IdAllocator<pou::SceneNode>   m_syncNodes;
        pou::IdAllocator<pou::SceneEntity> m_syncEntities;
        pou::IdAllocator<Character>        m_syncCharacters;

};

#endif // GAMEWORLD_H
