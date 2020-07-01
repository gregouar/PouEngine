#ifndef TEXTENGINE_H
#define TEXTENGINE_H

#include "PouEngine/text/AbstractTextImpl.h"
#include "PouEngine/core/Singleton.h"
#include "PouEngine/renderers/UiRenderer.h"
#include "PouEngine/tools/MathTools.h"
#include "PouEngine/tools/TexturesPacker.h"

#include <memory>

namespace pou
{


class TextEngine : public Singleton<TextEngine>
{
    friend class Singleton<TextEngine>;

    public:
        TextEngine();
        virtual ~TextEngine();

        bool init(std::unique_ptr<AbstractTextImpl> impl);
        bool cleanup();

        static FontTypeId loadFontFromFile(const std::string &filePath);

        static void useFontSize(FontTypeId fontId, int pt);
        static std::shared_ptr<Glyph> generateGlyph(FontTypeId fontId, uint16_t characterCode);

        static std::pair<TextureAsset *, MathTools::Box> packGlyph(size_t width, size_t height, const std::vector<uint8_t> buffer);

    protected:

    private:
        std::unique_ptr<AbstractTextImpl> m_impl;

        TexturesPacker m_glyphesPacker;

    public:
        static const size_t GLYPHES_ATLAS_PAGE_SIZE;
};

}

#endif // TEXTENGINE_H
