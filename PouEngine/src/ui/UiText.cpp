#include "PouEngine/ui/UiText.h"

#include "PouEngine/text/TextEngine.h"
#include "PouEngine/assets/FontAsset.h"

namespace pou
{

UiText::UiText(UserInterface *interface) : UiElement(interface),
    m_font(nullptr),
    m_fontSize(12),
    m_needToUpdateGlyphes(false)
{
    //ctor
}

UiText::~UiText()
{
    //dtor
}

void UiText::render(UiRenderer *renderer)
{
    UiElement::render(renderer);

    if(!m_font)
        return;

    if(m_needToUpdateGlyphes)
        this->generatedGlyphes();

}

void UiText::setFont(FontAsset *font)
{
    if(m_font != font)
    {
        m_font = font;
        m_needToUpdateGlyphes = true;
    }
}

void UiText::setText(const std::string &text)
{
    if(m_text != text)
    {
        m_text = text;
        m_needToUpdateGlyphes = true;
    }
}

void UiText::setFontSize(int pt)
{
    if(m_fontSize != pt)
    {
        m_fontSize = pt;
        m_needToUpdateGlyphes = true;
    }
}

///
///Protected
///

void UiText::generatedGlyphes()
{
    for(auto &g : m_glyphes)
        g->removeFromParent();
    m_glyphes.clear();

    bool autoJumpLine = (this->getSize().x != 0);

    glm::vec2 pos(0);

    int charPos = 0;
    for(auto c : m_text)
    {
        if(autoJumpLine && c == ' ')
        {
            auto nextWordSize = this->computeWordSize(charPos+1);
            if(pos.x + nextWordSize > this->getSize().x)
            {
                pos.x = 0;
                pos.y += m_fontSize;
                charPos++;
                continue;
            }
        }

        charPos++;

        if(c == '\n')
        {
            pos.x = 0;
            pos.y += m_fontSize;
            continue;
        }

        auto glyph = m_font->getGlyph(c, m_fontSize);//TextEngine::getGlyph(m_font, static_cast<uint16_t>(c));

        if(glyph)
        {
            auto texture = glyph->getTexture();

            if(texture)
            {
                auto glyphElement = std::make_shared<UiPicture>(m_interface);

                auto relPos = pos;
                relPos.x += glyph->getLeft();
                relPos.y -= glyph->getTop();

                glm::vec2 glyphSize = glyph->getTextureExtent();

                if(this->getSize().x > 0 && relPos.x + glyphSize.x > this->getSize().x)
                    glyphSize.x = std::max(this->getSize().x - relPos.x ,0.0f);
                if(this->getSize().y > 0 && relPos.y + glyphSize.y > this->getSize().y)
                    glyphSize.y = std::max(this->getSize().y - relPos.y ,0.0f);

                glyphElement->setPosition(relPos);
                glyphElement->setSize(glyphSize);

                glyphElement->setTexture(texture);
                glyphElement->setTextureRect(glyph->getTexturePosition(),
                                             glyphSize, false);

                this->addChildNode(glyphElement);
                m_glyphes.push_back(glyphElement);
            }

            pos.x += glyph->getAdvance();
        }
    }

    m_needToUpdateGlyphes = false;
}

int UiText::computeWordSize(size_t charPos)
{
    if(charPos >= m_text.size())
        return (0);

    int predictedSize = 0;
    while(charPos < m_text.length() && m_text[charPos] != ' ' && m_text[charPos] != '\n')
    {
        auto glyph = m_font->getGlyph(m_text[charPos], m_fontSize);
        if(glyph)
            predictedSize +=  glyph->getAdvance();
        charPos++;
    }

    return predictedSize;
}

}
