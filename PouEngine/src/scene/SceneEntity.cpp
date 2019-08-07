#include "PouEngine/scene/SceneEntity.h"

namespace pou
{

SceneEntity::SceneEntity() :
    m_isVisible(true)
{
    m_isAnEntity = true;
}

SceneEntity::~SceneEntity()
{
    //dtor
}

bool SceneEntity::isVisible()
{
    return m_isVisible;
}

void SceneEntity::setVisible(bool visible)
{
    m_isVisible = visible;
}

}
