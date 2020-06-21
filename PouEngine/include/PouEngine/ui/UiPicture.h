#ifndef UIPICTURE_H
#define UIPICTURE_H

#include "PouEngine/ui/RenderableUiElement.h"

#include "PouEngine/assets/TextureAsset.h"

namespace pou
{

class UiPicture : public RenderableUiElement
{
    public:
        UiPicture(/**const NodeTypeId,**/ UserInterface *interface);
        virtual ~UiPicture();

        void setTexture(TextureAsset *texture);

        void setTextureRect(glm::vec2 pos, glm::vec2 extent, bool isRelative = true);
        virtual void setTexturePosition(glm::vec2 pos);
        virtual void setTextureExtent(glm::vec2 extent);

        glm::vec2 getTexturePosition();
        glm::vec2 getTextureExtent();

        glm::vec2 getRelativeTexturePosition();
        glm::vec2 getRelativeTextureExtent();

    protected:
        virtual void updateDatum();

    private:
        TextureAsset *m_texture;
        glm::vec2 m_texturePosition;
        glm::vec2 m_textureExtent;
        bool      m_useRelativeTextureRect;
};

}

#endif // UIPICTURE_H
