#include "PouEngine/scene/ShadowCaster.h"

namespace pou
{

ShadowCaster::ShadowCaster() : SceneEntity(),
    m_shadowCastingType(ShadowCasting_None)
{
    m_isAShadowCaster = true;
}

ShadowCaster::~ShadowCaster()
{
    //dtor
}

void ShadowCaster::setShadowCastingType(ShadowCastingType type)
{
    m_shadowCastingType = type;
}

ShadowCastingType ShadowCaster::getShadowCastingType()
{
    return m_shadowCastingType;
}

}
