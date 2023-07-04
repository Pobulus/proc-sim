#pragma once
/** @file */ //doxygen
#include "IWidget.h"
namespace mw
{
class TitleBar : public IWidget
{
    sf::Text title;
    sf::Font font;
    sf::Color bg;
public:
    TitleBar ( sf::Vector2f windowSize, sf::FloatRect location, std::string text, std::string fontname="Computerfont.ttf" ) : IWidget ( windowSize, location )
    {
        title.setCharacterSize ( 100 );
        title.setFillColor ( sf::Color ( 229, 88, 7 ) );
        font.loadFromFile ( fontname );
        title.setFont ( font );
        setText ( addLineBreaks(text,  40) );
        bg = sf::Color ( 236, 248, 249 ) ;

    }
    actionCode clicked ( sf::Vector2f event )
    {
        return handled;
    }
    void setBGColor ( sf::Color clr )
    {
        bg = clr;
    }
    void setTextColor ( sf::Color clr )
    {
        title.setFillColor ( clr );
    }
    void display()
    {
        Texture.clear ( bg );
        Texture.draw ( title );
        Texture.display();
    }
    void setText ( std::string str )
    {
        title.setString ( str );
        Bounds = title.getGlobalBounds();
        boundsAddMargin ( 30.f );
    }
};
}
