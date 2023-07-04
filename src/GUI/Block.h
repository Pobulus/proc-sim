#pragma once
/** @file */ //doxygen
#include <vector>
#include <memory>
#include "ILightable.h"


namespace mw
{
class Block : public mw::ILightable
{
    sf::RectangleShape body;


public:
    Block ( float x, float y, float w, float h ) :
        body ( {w, h} )
    {
        body.setPosition ( x,y );
        body.setOutlineThickness ( 3.f );
        body.setOutlineColor ( mw::ColorScheme::getColor ( "outline-normal" ) );
        body.setFillColor ( mw::ColorScheme::getColor ( "block-fill" ) );

    }
    ~Block() {};
    virtual void draw ( sf::RenderTarget* RT )
    {
        RT->draw ( body );
    }
    void setColor ( sf::Color color )
    {
        body.setFillColor ( color );
    }
    sf::Vector2f getNode ( float disx, float disy )
    {
        //+3.f dla obramowania
        return {body.getGlobalBounds().left+disx+3.f, body.getGlobalBounds().top+disy+3.f};
    }
    void lightOn()
    {
        active = true;
        body.setOutlineColor ( mw::ColorScheme::getColor ( "outline-active" ) );
    }
    void lightOff()
    {
        active = false;
        body.setOutlineColor ( mw::ColorScheme::getColor ( "outline-normal" ) );
    }
};
}
