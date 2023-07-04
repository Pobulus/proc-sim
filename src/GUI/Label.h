#pragma once
/** @file */ //doxygen
#include "ILightable.h"

namespace mw
{
class Label : public ILightable
{
    sf::Font font;
    sf::Text label;
    sf::Vector2f node;
public:
    Label ( float x, float y, std::string text )
    {
        label.setString ( text );
        label.setFillColor ( mw::ColorScheme::getColor ( "outline-normal" ) );
        label.setPosition ( {x, y} ); //wyśrodkuj w elemencie
        label.setCharacterSize ( 60 ); //renderuj jako większy rozmiar znaków, żeby mniej się rozmywał
        label.setScale ( 0.5,0.5 ); //przeskaluj do pierwotnego rozmiaru, ale mniej rozmyte
        node = {label.getGlobalBounds().left, label.getGlobalBounds().top};
    }
    void draw ( sf::RenderTarget* RT )
    {
        RT->draw ( label );
    }
    sf::Vector2f getNode ( float dx, float dy )
    {
        /**  dx/dy  0   1   2
         *      0   .   .   .
         *      1   .   .   .
         *      2   .   .   .
         **/
        auto bounds = label.getGlobalBounds();

        float x=bounds.left+ ( dx* ( bounds.width/2+2.f ) )-2.f;
        float y=bounds.top+ ( dy* ( bounds.height/2+2.f ) )-2.f;

        return {x, y};

    }
    sf::Vector2f getSize(){
      return label.getGlobalBounds().getSize();
    }
    void setFont ( const sf::Font &F )
    {
        font = F; //zapisz czcionkę
        label.setFont ( font );
    }
    virtual void lightOn()
    {
        active = true;
        label.setFillColor ( mw::ColorScheme::getColor ( "outline-active" ) );
        label.setStyle ( sf::Text::Bold );
    }
    virtual void lightOff()
    {
        active = false;
        label.setFillColor ( mw::ColorScheme::getColor ( "outline-normal" ) );
        label.setStyle ( sf::Text::Regular );
    }
};
}
