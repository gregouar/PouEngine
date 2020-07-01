#include "PouEngine/text/Glyph.h"

#include "PouEngine/text/TextEngine.h"

namespace pou
{

Glyph::Glyph(int left, int top, int width, int height, int advance, const std::vector<uint8_t> &buffer) :
    m_left(left),
    m_top(top),
    m_advance(advance),
    m_texture(nullptr),
    m_texturePosition(0, 0),
    m_textureExtent(width, height)
{
    if(!buffer.empty())
    {
        auto [texture, box] = TextEngine::packGlyph(width, height, buffer);
        m_texture = texture;
        m_texturePosition = box.center;
    }

    //if(!buffer.empty())
      //  m_texture = TextureAsset::generateTexture(width, height, buffer);
}

Glyph::~Glyph()
{
    //dtor
}

int Glyph::getAdvance()
{
    return m_advance;
}

int Glyph::getTop()
{
    return m_top;
}

int Glyph::getLeft()
{
    return m_left;
}

int Glyph::getWidth()
{
    return m_textureExtent.x;
}

int Glyph::getHeight()
{
    return m_textureExtent.y;
}


TextureAsset *Glyph::getTexture()
{
    return m_texture;
}

glm::vec2 Glyph::getTexturePosition()
{
    return m_texturePosition;
}

glm::vec2 Glyph::getTextureExtent()
{
    return m_textureExtent;
}


}
