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

    glm::vec2 pos(0);

    for(auto c : m_text)
    {
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

                glyphElement->setPosition(relPos);
                glyphElement->setSize(glyph->getTextureExtent());

                glyphElement->setTexture(texture);
                glyphElement->setTextureRect(glyph->getTexturePosition(),
                                             glyph->getTextureExtent(), false);

                this->addChildNode(glyphElement);
                m_glyphes.push_back(glyphElement);
            }

            pos.x += glyph->getAdvance();

            continue;
        }

        if(c != ' ')
        {
            auto glyphElement = std::make_shared<UiPicture>(m_interface);

            glyphElement->setPosition(pos);
            glyphElement->setSize(glm::vec2(14,16));

            this->addChildNode(glyphElement);
            m_glyphes.push_back(glyphElement);
        }

        pos.x += 16;
    }

    m_needToUpdateGlyphes = false;
}

}
