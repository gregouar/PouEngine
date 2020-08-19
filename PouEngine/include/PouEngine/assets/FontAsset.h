#ifndef FONTASSET_H
#define FONTASSET_H

#include "PouEngine/assets/Asset.h"
#include "PouEngine/text/Glyph.h"

namespace pou
{

class FontAsset : public Asset
{
    public:
        FontAsset();
        FontAsset(const AssetTypeId);
        virtual ~FontAsset();

        bool loadFromFile(const std::string &filePath);
        FontTypeId getFontId();

        Glyph *getGlyph(uint16_t characterCode, int pt);

    protected:

    private:
        FontTypeId m_fontId;

        ///std::map< std::pair<uint16_t, int>, std::shared_ptr<Glyph> > m_glyphes;
        std::unordered_map< std::pair<int, int>, std::shared_ptr<Glyph>, IntPairHash > m_glyphes;
};

}

#endif // FONTASSET_H
