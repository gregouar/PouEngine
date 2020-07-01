#ifndef GLYPH_H
#define GLYPH_H

#include "PouEngine/assets/TextureAsset.h"

namespace pou
{

class Glyph
{
    public:
        Glyph(int left, int top, int width, int height, int advance, const std::vector<uint8_t> &buffer);
        virtual ~Glyph();

        int getAdvance();
        int getTop();
        int getLeft();
        int getWidth();
        int getHeight();

        TextureAsset *getTexture();
        glm::vec2 getTexturePosition();
        glm::vec2 getTextureExtent();

    protected:

    private:
        int m_left;
        int m_top;
        int m_advance;

        TextureAsset *m_texture;
        glm::vec2 m_texturePosition;
        glm::vec2 m_textureExtent;
};

}

#endif // GLYPH_H
