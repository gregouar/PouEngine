#ifndef UIPROGRESSBAR_H
#define UIPROGRESSBAR_H

#include "PouEngine/ui/UiElement.h"
#include "PouEngine/ui/UiPicture.h"

namespace pou
{

class UiProgressBar : public UiPicture
{
    public:
        UiProgressBar(const NodeTypeId, UserInterface *interface);
        virtual ~UiProgressBar();

        void setValue(float v);
        void setMinMaxValue(float mi, float ma);

        virtual void setSize(glm::vec2 s);
        virtual void setTextureExtent(glm::vec2 s);

        void setHorizontalOrientation(bool = true);
        void setVerticalOrientation(bool = true);

        //void setBackgroundElement(UiPicture *background);
        //void setBarElement(UiPicture *bar);

        //void setBackgroundElement(UiElement *background);
        //void setBarElement(UiElement *bar);


        virtual void update(const Time &elapsedTime);

    protected:

    private:
        float m_value, m_minValue, m_maxValue;

        bool m_isVertical;
        //UiPicture /**m_background,*/ *m_bar;

        //UiPicture /**m_customBackground,*/ *m_customBar;
        //glm::vec2 m_orBarSize;
        //glm::vec2 m_orBarTextureExtent;

        glm::vec2 m_totalSize;
        glm::vec2 m_totalTextureExtent;
};

}

#endif // UIPROGRESSBAR_H
