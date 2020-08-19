#ifndef FREETYPETEXTIMPL_H
#define FREETYPETEXTIMPL_H

#include "PouEngine/text/AbstractTextImpl.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <map>

namespace pou
{

struct FreeTypeGlyph
{
    FT_UInt charIndex;
};

struct FreeTypeFont
{
    FT_Face face;
    std::unordered_map<uint16_t, FreeTypeGlyph> glyphes;
};

class FreeTypeTextImpl : public AbstractTextImpl
{
    public:
        FreeTypeTextImpl();
        virtual ~FreeTypeTextImpl();

        virtual FontTypeId loadFontFromFile(const std::string &filePath);

        virtual void useFontSize(FontTypeId fontId, int pt);
        virtual std::shared_ptr<Glyph> generateGlyph(FontTypeId fontId, uint16_t characterCode);

    protected:
        bool init();

        //std::shared_ptr<FreeTypeFont> getFont(FontAsset *fontAsset);
        std::shared_ptr<FreeTypeFont> getFont(FontTypeId id);

    private:
        FT_Library  m_library;

        std::unordered_map<FontTypeId, std::shared_ptr<FreeTypeFont> > m_fonts;
};

}

#endif // FREETYPETEXTIMPL_H
