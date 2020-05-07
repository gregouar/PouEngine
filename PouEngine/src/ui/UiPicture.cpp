#include "PouEngine/ui/UiPicture.h"

namespace pou
{

UiPicture::UiPicture(const NodeTypeId id) :
    RenderableUiElement(id),
    m_texture(nullptr)
{
    //ctor
}

UiPicture::~UiPicture()
{
    //dtor
}

void UiPicture::updateDatum()
{
    RenderableUiElement::updateDatum();

    if(m_texture != nullptr)
    m_datum.albedo_texId = {m_texture->getVTexture().getTextureId(),
                            m_texture->getVTexture().getTextureLayer()};
}

}
