#ifndef ABSTRACTTEXTIMPL_H_INCLUDED
#define ABSTRACTTEXTIMPL_H_INCLUDED

#include "PouEngine/Types.h"
#include "PouEngine/text/Glyph.h"

namespace pou
{

class AbstractTextImpl
{
    public:
        AbstractTextImpl();
        virtual ~AbstractTextImpl();

        virtual FontTypeId loadFontFromFile(const std::string &filePath) = 0;

        virtual void useFontSize(FontTypeId fontId, int pt) = 0;
        virtual std::shared_ptr<Glyph> generateGlyph(FontTypeId fontId, uint16_t characterCode) = 0;

    protected:
        FontTypeId generateFontId();

    private:
        FontTypeId m_curFontId;
};
}


#endif // ABSTRACTTEXTIMPL_H_INCLUDED
