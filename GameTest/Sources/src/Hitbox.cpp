#include "Hitbox.h"

#include "PouEngine/utils/Logger.h"
#include "PouEngine/utils/Parser.h"

Hitbox::Hitbox()
{
    m_factors.resize(NBR_DAMAGE_TYPES,0.0f);
}



Hitbox::~Hitbox()
{
    //dtor
}


float Hitbox::getFactor(DamageType type) const
{
    if(type == NBR_DAMAGE_TYPES)
        return (0.0f);

    return (m_factors[type]);
}

float Hitbox::getFactor(int type) const
{
    if(type < 0 || type >= NBR_DAMAGE_TYPES)
        return (0.0f);

    return (m_factors[type]);
}

const std::string &Hitbox::getSkeleton() const
{
    return m_skeleton;
}

const std::string &Hitbox::getNode() const
{
    return m_node;
}

const pou::MathTools::Box &Hitbox::getBox() const
{
    return m_box;
}


bool Hitbox::loadFromXML(TiXmlElement *boxElement)
{
    auto skeletonAtt= boxElement->Attribute("skeleton");
    auto nodeAtt    = boxElement->Attribute("node");

    if(skeletonAtt == nullptr || nodeAtt == nullptr)
        return (false);

    m_skeleton = std::string(skeletonAtt);
    m_node = std::string(nodeAtt);


    auto factorChild = boxElement->FirstChildElement("factor");
    while(factorChild != nullptr)
    {
        float factor = pou::Parser::parseFloat(factorChild->GetText());

        if(factorChild->Attribute("type") != nullptr)
        {
            std::string type = std::string(factorChild->Attribute("type"));
            if(type == "cut")
                m_factors[DamageType_Cut] = factor;
            else if(type == "blunt")
                m_factors[DamageType_Blunt] = factor;
            else if(type == "pierce")
                m_factors[DamageType_Pierce] = factor;
            else if(type == "axe")
                m_factors[DamageType_Axe] = factor;
            else if(type == "pickaxe")
                m_factors[DamageType_Pickaxe] = factor;
        } else {
            for(auto i = 0 ; i < NBR_DAMAGE_TYPES ; ++i)
                m_factors[i] = factor;
        }

        factorChild = factorChild->NextSiblingElement("factor");
    }

    auto sizeElement = boxElement->FirstChildElement("size");
    if(sizeElement != nullptr)
    {
        if(sizeElement->Attribute("x") != nullptr)
            m_box.size.x = pou::Parser::parseFloat(std::string(sizeElement->Attribute("x")));
        if(sizeElement->Attribute("y") != nullptr)
            m_box.size.y = pou::Parser::parseFloat(std::string(sizeElement->Attribute("y")));
    }

    auto centerElement = boxElement->FirstChildElement("center");
    if(centerElement != nullptr)
    {
        if(centerElement->Attribute("x") != nullptr)
            m_box.center.x = pou::Parser::parseFloat(std::string(centerElement->Attribute("x")));
        if(centerElement->Attribute("y") != nullptr)
            m_box.center.y = pou::Parser::parseFloat(std::string(centerElement->Attribute("y")));
    }

    return (true);
}

