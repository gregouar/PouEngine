#include "PouEngine/assets/FontAsset.h"

#include "PouEngine/text/TextEngine.h"
#include "PouEngine/tools/Logger.h"

namespace pou
{


FontAsset::FontAsset() : FontAsset(-1)
{
}

FontAsset::FontAsset(const AssetTypeId id) : Asset(id),
    m_fontId(0)
{
    m_allowLoadFromFile = true;
    m_allowLoadFromMemory = false;
}

FontAsset::~FontAsset()
{
    //dtor
}

bool FontAsset::loadFromFile(const std::string &filePath)
{
    m_fontId = TextEngine::loadFontFromFile(filePath);
    if(m_fontId == (uint32_t)(-1))
    {
        Logger::error("Failed to load font from file: "+filePath);
        return (false);
    }

    Logger::write("Font loaded from file: "+filePath);

    return (true);
}

FontTypeId FontAsset::getFontId()
{
    return m_fontId;
}

Glyph *FontAsset::getGlyph(uint16_t characterCode, int pt)
{
    auto glyphIt = m_glyphes.find({characterCode,pt});
    if(glyphIt != m_glyphes.end())
        return glyphIt->second.get();

    //auto glyph = std::make_shared<Glyph>();

    TextEngine::useFontSize(m_fontId, pt);
    auto glyph = TextEngine::generateGlyph(m_fontId, characterCode);

    if(glyph)
        m_glyphes.insert({{characterCode, pt}, glyph});

    return glyph.get();
}

}
