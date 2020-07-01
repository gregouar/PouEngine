#include "PouEngine/text/TextEngine.h"

namespace pou
{

const size_t TextEngine::GLYPHES_ATLAS_PAGE_SIZE = 1024;

TextEngine::TextEngine() :
    m_glyphesPacker(glm::vec2(GLYPHES_ATLAS_PAGE_SIZE))
{
    //ctor
}

TextEngine::~TextEngine()
{
    //dtor
}


bool TextEngine::init(std::unique_ptr<AbstractTextImpl> impl)
{
    m_impl = std::move(impl);
    return (true);
}

bool TextEngine::cleanup()
{
    m_impl.reset();
    return (true);
}

FontTypeId TextEngine::loadFontFromFile(const std::string &filePath)
{
    return instance()->m_impl->loadFontFromFile(filePath);
}

void TextEngine::useFontSize(FontTypeId fontId, int pt)
{
    return instance()->m_impl->useFontSize(fontId, pt);
}

std::shared_ptr<Glyph> TextEngine::generateGlyph(FontTypeId fontId, uint16_t characterCode)
{
    return instance()->m_impl->generateGlyph(fontId, characterCode);
}

std::pair<TextureAsset *, MathTools::Box> TextEngine::packGlyph(size_t width, size_t height, const std::vector<uint8_t> buffer)
{
    return instance()->m_glyphesPacker.packTexture(width, height, buffer);
}


}
