#include "PouEngine/ui/UiProgressBar.h"

#include "PouEngine/ui/UserInterface.h"

namespace pou
{

UiProgressBar::UiProgressBar(/**const NodeTypeId id,**/ UserInterface *interface) :
    UiPicture(/**id,**/interface),
    m_value(0.0f),
    m_minValue(0.0f),
    m_maxValue(1.0f),
    m_isVertical(false),
    m_totalSize(0.0f),
    m_totalTextureExtent(1.0f)
    //m_background(nullptr),
    //m_bar(nullptr),
    //m_customBackground(nullptr),
    //m_customBar(nullptr),
    //m_orBarTextureExtent(1.0)
{
    //m_customBackground = interface->createUiPicture();
    //m_customBar = interface->createUiPicture();
    //m_background = m_customBackground;
    //m_bar = m_customBar;

    //this->addChildNode(m_background);
    //this->addChildNode(m_bar);
}

UiProgressBar::~UiProgressBar()
{
    //delete m_customBackground;
    //delete m_customBar;
}


void UiProgressBar::setValue(float v)
{
    if(m_value >= m_minValue && m_value <= m_maxValue)
        m_value = v;
}

void UiProgressBar::setMinMaxValue(float mi, float ma)
{
    if(ma > mi)
    {
        m_minValue = mi;
        m_maxValue = ma;
    }
}

void UiProgressBar::setSize(glm::vec2 s)
{
    if(s.x >= 0 && s.y >= 0)
        m_totalSize = s;
    //UiElement::setSize(s);
    //m_orBarSize = s;
    //m_bar->setSize(s);
}

void UiProgressBar::setTextureExtent(glm::vec2 extent)
{
    m_totalTextureExtent = extent;
}

void UiProgressBar::setHorizontalOrientation(bool b)
{
    m_isVertical = !b;
}

void UiProgressBar::setVerticalOrientation(bool b)
{
    m_isVertical = b;
}

/*void UiProgressBar::setBackgroundElement(UiElement *background)
{
    if(m_background != background)
    {
        this->removeChildNode(m_background);
        m_background = background;
        this->addChildNode(m_background);
    }
}*/

/*void UiProgressBar::setBarElement(UiElement *bar)
{
    if(m_bar != bar)
    {
        this->removeChildNode(m_bar);
        m_bar = bar;
        this->addChildNode(m_bar);

        m_orBarSize = bar->getSize();
    }
}*/


/*void UiProgressBar::setBackgroundElement(UiPicture *background)
{
    UiProgressBar::setBackgroundElement((UiElement*) background);
}*/

/*void UiProgressBar::setBarElement(UiPicture *bar)
{
    //UiProgressBar::setBarElement((UiElement*) bar);

    if(m_bar != bar)
    {
        this->removeChildNode(m_bar);
        m_bar = bar;
        this->addChildNode(m_bar);

        m_orBarSize = bar->getSize();
    }

    m_orBarTextureExtent = bar->getTextureExtent();
}*/


void UiProgressBar::update(const Time &elapsedTime, uint32_t localTime)
{
    float p = (m_value-m_minValue)/(m_maxValue-m_minValue);

    if(m_isVertical)
    {
        UiElement::setSize({m_totalSize.x,m_totalSize.y*p});
        UiPicture::setTextureExtent({m_totalTextureExtent.x, m_totalTextureExtent.y*p});
        //m_bar->setSize(m_orBarSize.x,m_orBarSize.y*p);
        //m_bar->setTextureExtent({m_orBarTextureExtent.x, m_orBarTextureExtent.y*p});
    }
    else
    {
        UiElement::setSize({m_totalSize.x*p,m_totalSize.y});
        UiPicture::setTextureExtent({m_totalTextureExtent.x*p, m_totalTextureExtent.y});
        //m_bar->setSize(m_orBarSize.x*p,m_orBarSize.y);
        //m_bar->setTextureExtent({m_orBarTextureExtent.x*p, m_orBarTextureExtent.y});
    }

    UiPicture::update(elapsedTime,localTime);
}


}
