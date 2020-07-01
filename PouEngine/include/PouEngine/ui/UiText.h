#ifndef UITEXT_H
#define UITEXT_H

#include "PouEngine/ui/UiElement.h"
#include "PouEngine/ui/UiPicture.h"

namespace pou
{

class UiText : public UiElement
{
    public:
        UiText(UserInterface *interface);
        virtual ~UiText();

        virtual void render(UiRenderer *renderer);

        void update();

        void setFont(FontAsset *font);
        void setText(const std::string &text);
        void setFontSize(int pt);

    protected:
        void generatedGlyphes();

    private:
        FontAsset *m_font;
        std::string m_text;
        int m_fontSize;

        bool m_needToUpdateGlyphes;
        std::vector< std::shared_ptr<UiPicture> > m_glyphes;
};

}

#endif // UITEXT_H
