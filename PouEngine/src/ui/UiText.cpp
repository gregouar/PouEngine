#include "PouEngine/ui/UiText.h"

#include "PouEngine/text/TextEngine.h"
#include "PouEngine/assets/FontAsset.h"

namespace pou
{

UiText::UiText(UserInterface *interface) : UiElement(interface),
    m_font(nullptr),
    m_fontSize(12),
    m_color(glm::vec4(1)),
    m_textAlign(TextAlignType_Left),
    m_verticalAlign(false),
    m_needToUpdateGlyphes(false)
{
    //ctor
}

UiText::~UiText()
{
    //dtor
}

void UiText::update(const Time &elapsedTime, uint32_t localTime)
{
    if(m_needToUpdateGlyphes)
        this->generatedGlyphes();

    UiElement::update(elapsedTime, localTime);
}

void UiText::render(UiRenderer *renderer)
{
    UiElement::render(renderer);
}

void UiText::setSize(glm::vec2 s)
{
    if(this->getSize() != s)
        m_needToUpdateGlyphes = true;
    UiElement::setSize(s);
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

void UiText::setColor(const glm::vec4 &color)
{
    if(color != m_color)
    {
        m_color = color;
        for(auto glyph : m_glyphes)
            glyph->setColor(m_color);
    }
}

void UiText::setTextAlign(TextAlignType textAlign)
{
    if(m_textAlign != textAlign)
    {
        m_textAlign = textAlign;
        m_needToUpdateGlyphes = true;
    }
}

void UiText::setVerticalAlign(bool verticalAlign)
{
    if(m_verticalAlign != verticalAlign)
    {
        m_verticalAlign = verticalAlign;
        m_needToUpdateGlyphes = true;
    }
}

const std::string &UiText::getText()
{
    return m_text;
}

///
///Protected
///

void UiText::generatedGlyphes()
{
    if(!m_font)
        return;

    for(auto &g : m_glyphes)
        g->removeFromParent();
    m_glyphes.clear();

    bool autoJumpLine = (this->getSize().x != 0);

    glm::vec2 pos(0,m_fontSize);

    int lastJump = 0;
    int lineSize = 0;
    int verticalSize = m_fontSize*1.5;
    int charPos = 0;
    int charNbr = 0;
    for(auto c : m_text)
    {
        bool jumpLine = false;

        if(autoJumpLine && c == ' ')
        {
            auto nextWordSize = this->computeWordSize(charPos+1);
            if(pos.x + nextWordSize > this->getSize().x)
                jumpLine = true;
        }

        if(c == '\n' || c == '\0')
            jumpLine = true;

        charPos++;

        if(jumpLine)
        {
            pos.x = 0;
            pos.y += m_fontSize;
            verticalSize += m_fontSize;

            if(m_textAlign == TextAlignType_Center)
            {
                int delta = this->getSize().x/2 - lineSize/2;
                int i = 0;
                for(auto it = m_glyphes.rbegin() ;  it != m_glyphes.rend() && i < charNbr - lastJump ; ++i, ++it)
                    (*it)->move(delta,0);
            }

            lastJump = charNbr;
            lineSize = 0;
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

                //This will not work for centered text align
                if(this->getSize().x > 0 && relPos.x + glyphSize.x > this->getSize().x)
                    glyphSize.x = std::max(this->getSize().x - relPos.x ,0.0f);
                if(this->getSize().y > 0 && relPos.y + glyphSize.y > this->getSize().y)
                    glyphSize.y = std::max(this->getSize().y - relPos.y ,0.0f);

                glyphElement->setPosition(relPos);
                glyphElement->setSize(glyphSize);
                glyphElement->setColor(m_color);

                glyphElement->setTexture(texture);
                glyphElement->setTextureRect(glyph->getTexturePosition(),
                                             glyphSize, false);

                this->addChildNode(glyphElement);
                m_glyphes.push_back(glyphElement);
                charNbr++;
            }

            pos.x += glyph->getAdvance();
            lineSize += glyph->getAdvance();
        }
    }

    ///I could create a sub uiElement per line, and a sub uiElement for the whole text, would be easier to handle centering
    if(m_textAlign == TextAlignType_Center)
    {
        int delta = this->getSize().x/2 - lineSize/2;
        int i = 0;
        for(auto it = m_glyphes.rbegin() ;  it != m_glyphes.rend() && i < charNbr - lastJump ; ++i, ++it)
            (*it)->move(delta,0);
    }

    if(m_verticalAlign)
    {
        int delta = this->getSize().y/2 - verticalSize/2;
        for(auto &glyph : m_glyphes)
            glyph->move(0,delta);
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

/*void UiText::jumpLine()
{

}*/

}
