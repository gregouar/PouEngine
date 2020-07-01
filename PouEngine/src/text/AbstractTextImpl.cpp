#include "PouEngine/text/AbstractTextImpl.h"

namespace pou
{

AbstractTextImpl::AbstractTextImpl() :
    m_curFontId(0)
{

}

AbstractTextImpl::~AbstractTextImpl()
{

}


FontTypeId AbstractTextImpl::generateFontId()
{
    return m_curFontId++;
}



}

