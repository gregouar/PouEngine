#ifndef GAMEUI_H
#define GAMEUI_H

#include "PouEngine/ui/UserInterface.h"
#include "PouEngine/ui/UiPicture.h"

#include <memory>

class GameUi : public pou::UserInterface
{
    public:
        GameUi();
        virtual ~GameUi();

        bool init();

        void updateCharacterLife(float cur, float max);

    protected:
       // void cleanup();

    private:
        //std::unique_ptr<pou::UserInterface>     m_mainInterface;
        pou::UiPicture         *m_uiPictureTest;
        pou::UiProgressBar     *m_lifeBar;
};

#endif // GAMEUI_H
