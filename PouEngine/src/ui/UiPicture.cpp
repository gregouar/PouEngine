#include "PouEngine/ui/UiPicture.h"

namespace pou
{

UiPicture::UiPicture(/**const NodeTypeId id,**/ UserInterface *interface) :
    RenderableUiElement(/**id,**/ interface),
    m_texture(nullptr),
    m_texturePosition(0.0f),
    m_textureExtent(1.0f),
    m_useRelativeTextureRect(true)
{
    //ctor
}

UiPicture::~UiPicture()
{
    //dtor
}

void UiPicture::setTexture(TextureAsset *texture)
{
    m_texture = texture;
}


void UiPicture::setTextureRect(glm::vec2 pos, glm::vec2 extent, bool isRelative)
{
    m_useRelativeTextureRect = isRelative;
    this->setTexturePosition(pos);
    this->setTextureExtent(extent);
}

void UiPicture::setTexturePosition(glm::vec2 pos)
{
    m_texturePosition = pos;
}

void UiPicture::setTextureExtent(glm::vec2 extent)
{
    m_textureExtent = extent;
}

glm::vec2 UiPicture::getTexturePosition()
{
    return m_texturePosition;
}

glm::vec2 UiPicture::getTextureExtent()
{
    return m_textureExtent;
}

glm::vec2 UiPicture::getRelativeTextureExtent()
{
    if(!m_useRelativeTextureRect)
    {
        if(m_texture == nullptr)
            return {0,0};
        auto texExtent = m_texture->getExtent();
        if(texExtent.x == 0 || texExtent.y == 0)
            return {0,0};

        return {m_textureExtent.x/texExtent.x,
                m_textureExtent.y/texExtent.y};
    }

    return m_textureExtent;
}

glm::vec2 UiPicture::getRelativeTexturePosition()
{
    if(!m_useRelativeTextureRect)
    {
        if(m_texture == nullptr)
            return {0,0};
        auto texExtent = m_texture->getExtent();
        if(texExtent.x == 0 || texExtent.y == 0)
            return {0,0};

        return {m_texturePosition.x/texExtent.x,
                m_texturePosition.y/texExtent.y};
    }

    return m_texturePosition;
}


void UiPicture::updateDatum()
{
    RenderableUiElement::updateDatum();

    if(m_texture != nullptr)
    m_datum.albedo_texId = {m_texture->getVTexture().getTextureId(),
                            m_texture->getVTexture().getTextureLayer()};

    m_datum.texExtent = this->getRelativeTextureExtent();
    m_datum.texPos = this->getRelativeTexturePosition();
}

}
