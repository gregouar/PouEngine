#include "PouEngine/text/FreeTypeTextImpl.h"

#include "PouEngine/assets/FontAsset.h"

#include "PouEngine/tools/Logger.h"

namespace pou
{

FreeTypeTextImpl::FreeTypeTextImpl()
{
    this->init();
}

FreeTypeTextImpl::~FreeTypeTextImpl()
{
    //dtor
}

FontTypeId FreeTypeTextImpl::loadFontFromFile(const std::string &filePath)
{
    FontTypeId fontId = -1;
    auto font = std::make_shared<FreeTypeFont>();

    FT_Face &face = font->face;
    auto error = FT_New_Face(m_library, filePath.c_str(), 0, &face);
    /*if(error == FT_Err_Unknown_File_Format)
        Logger::warning("Failed to load font file "+filePath+": unsupported format");
    else*/ if (error)
        Logger::warning("Failed to load font file "+filePath+" (error "+std::to_string(error)+")");
    else
    {
        fontId = this->generateFontId();
        m_fonts.insert({fontId, font});
    }

    return fontId;
}


void FreeTypeTextImpl::useFontSize(FontTypeId fontId, int pt)
{
    auto font = this->getFont(fontId);
    if(!font)
        return;

    auto &face  = font->face;
    auto error  = FT_Set_Pixel_Sizes(face, 0,
                               static_cast<FT_UInt>(pt));

    if(error)
        Logger::warning("Failed to set font size to "+std::to_string(pt)+" (error "+std::to_string(error)+")");
}

std::shared_ptr<Glyph> FreeTypeTextImpl::generateGlyph(FontTypeId fontId, uint16_t characterCode)
{
    auto font = this->getFont(fontId);
    if(!font)
        return (nullptr);

    auto &face = font->face;

    auto ftGlyphIt = font->glyphes.find(characterCode);
    if(ftGlyphIt == font->glyphes.end())
    {
        auto glyphIndex = FT_Get_Char_Index(face, characterCode);
        FreeTypeGlyph ftGlyph;
        ftGlyph.charIndex = glyphIndex;
        ftGlyphIt = font->glyphes.insert({characterCode, ftGlyph}).first;
    }

    auto ftGlyph = ftGlyphIt->second;

    auto error = FT_Load_Glyph(face, ftGlyph.charIndex, FT_LOAD_DEFAULT);
    if(error)
        Logger::warning("Failed to load glyph (error "+std::to_string(error)+")");

    auto &glyphSlot = face->glyph;

    if(glyphSlot->format  != FT_GLYPH_FORMAT_BITMAP)
    {
        error = FT_Render_Glyph(glyphSlot, FT_RENDER_MODE_NORMAL);
        if(error)
            Logger::warning("Failed to render glyph (error "+std::to_string(error)+")");
    }

    if(glyphSlot->bitmap.pixel_mode != FT_PIXEL_MODE_GRAY || glyphSlot->bitmap.num_grays != 256)
    {
        Logger::warning("Failed to render glyph (unsupported pixel mode)");
        return (nullptr);
    }

    //auto width = static_cast<int>(glyphSlot->metrics.width / 64);
    //auto height = static_cast<int>(glyphSlot->metrics.height / 64);
    auto width = glyphSlot->bitmap.width;
    auto height = glyphSlot->bitmap.rows;
    auto advance = static_cast<int>(glyphSlot->advance.x / 64);

    auto bufferSize = width * height * 4;
    std::vector<uint8_t> buffer(bufferSize);

    auto rowBegin = glyphSlot->bitmap.buffer;
    int i = 0;

    for(size_t y = 0 ; y < height ; y++)
    {
        auto src = rowBegin;
        for(size_t x = 0 ; x < width ; ++x)
        {
            //std::cout<<*src;
            buffer[i++] = 0xff;
            buffer[i++] = 0xff;
            buffer[i++] = 0xff;
            buffer[i++] = *(src++);
        }
        rowBegin += glyphSlot->bitmap.pitch;
        //std::cout<<std::endl;
    }
    //std::cout<<std::endl;

    //std::cout<<"GenGlyph: "<<(char)characterCode<<std::endl;

    auto glyph = std::make_shared<Glyph>(glyphSlot->bitmap_left,
                                         glyphSlot->bitmap_top,
                                         width,
                                         height,
                                         advance,
                                         buffer);

    return glyph;
}


///
///Protected
///

bool FreeTypeTextImpl::init()
{
    auto error = FT_Init_FreeType(&m_library);
    if (error)
    {
        return (false);
    }

    return (true);
}

/*std::shared_ptr<FreeTypeFont> FreeTypeTextImpl::getFont(FontAsset *fontAsset)
{
    if(!fontAsset)
        return (nullptr);

    return this->getFont(fontAsset->getId());
}*/

std::shared_ptr<FreeTypeFont> FreeTypeTextImpl::getFont(FontTypeId id)
{
    auto fontIt = m_fonts.find(id);
    if(fontIt == m_fonts.end())
        return (nullptr);
    return fontIt->second;
}

}
