#ifndef UIPICTURE_H
#define UIPICTURE_H

#include "PouEngine/ui/RenderableUiElement.h"

#include "PouEngine/assets/TextureAsset.h"

namespace pou
{

class UiPicture : public RenderableUiElement
{
    public:
        UiPicture(const NodeTypeId);
        virtual ~UiPicture();

    protected:
        virtual void updateDatum();

    private:
        TextureAsset *m_texture;
};

}

#endif // UIPICTURE_H
