#ifndef GAMEWORLD_H
#define GAMEWORLD_H

#include "PouEngine/scene/Scene.h"

class GameWorld
{
    public:
        GameWorld(bool renderable);
        virtual ~GameWorld();

        void update(const pou::Time elapsed_time);

        void generate();
        void destroy();


    protected:

    private:
        pou::Scene *m_scene;

        bool m_isRenderable;

};

#endif // GAMEWORLD_H
