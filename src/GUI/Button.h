#pragma once
/** @file */ //doxygen
#include "Block.h"
#include "IClickable.h"


namespace mw
{
class Button : public Block, public IClickable
{
    sf::Text text;
    sf::Font font;
    actionCode cc;
public:

    Button ( float x, float y, float w, float h, actionCode ClickCode, sf::String txt, std::string fontname="" ) : Block ( x,y,w,h ), cc ( ClickCode )
    {

        this->area = {x,y,w,h};
        font.loadFromFile ( fontname );
        text.setFont ( font );
        text.setString ( txt );
        text.setCharacterSize ( 60 );
        text.setFillColor ( sf::Color::Black );
        //wyskaluj text
        auto txtbound = text.getGlobalBounds();
        float scale = std::min ( w/txtbound.width, h/txtbound.height ) *0.8;
        text.setScale ( scale, scale );
        //wyśrodkuj  po przeskalowaniu
        text.setPosition ( {x,y} );
        txtbound = text.getGlobalBounds();
        sf::Vector2f posBoxCenter = {x+ ( w/2 ),y+ ( h/2 ) };
        sf::Vector2f posTextCenter = {txtbound.left+ ( txtbound.width/2 ),txtbound.top+ ( txtbound.height/2 ) };
        sf::Vector2f displacement = posTextCenter-posBoxCenter;
        sf::Vector2f posFinal = sf::Vector2f ( x, y )-displacement;
        text.setPosition ( posFinal );
    }



    void setTextColor ( sf::Color color )
    {
        text.setFillColor ( color );
    }
    actionCode clicked ( sf::Vector2f event )
    {
        return cc;
    };
    virtual void draw ( sf::RenderTarget* RT )
    {
        mw::Block::draw ( RT );
        RT->draw ( text );

    }
};
}
