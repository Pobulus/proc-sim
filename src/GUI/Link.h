#pragma once
/** @file */ //doxygen
#include "IDrawable.h"
#include <vector>
#include <cmath>
#include <memory>

namespace mw
{
class Link : public ILightable
{
    sf::Vector2f from;
    sf::Vector2f to;
    bool directed;
    std::shared_ptr<sf::RectangleShape> horizontal_line, vertical_line;
    std::shared_ptr<sf::Shape> arrowTip;
public:
    Link ( sf::Vector2f from, sf::Vector2f to, bool hor, std::string style ) : from ( from ), to ( to )
    {
        auto color = mw::ColorScheme::getColor ( "outline-normal" );
        //fabsf() to wartość bezwzględna z float, zwracająca float
        horizontal_line = std::make_shared<sf::RectangleShape> ( sf::Vector2f ( fabsf ( from.x-to.x )+2.f, 4.f ) );
        // +2.f żeby linie się przecinały
        horizontal_line->setFillColor ( color );
        vertical_line = std::make_shared<sf::RectangleShape> ( sf::Vector2f ( 4.f, fabsf ( from.y-to.y )+2.f ) );
        vertical_line->setFillColor ( color );
        auto crossbar = std::make_shared<sf::RectangleShape> ( sf::Vector2f ( 20,3 ) );
        crossbar->setPosition ( to-sf::Vector2f ( 5,0 ) );
        crossbar->setFillColor ( color );
        auto tip = std::make_shared<sf::CircleShape> ( 10,3 );
        tip->setPosition ( to );
        tip->setFillColor ( color );
        auto narrow_tip = std::make_shared<sf::CircleShape> ( 10,3 );
        narrow_tip->setFillColor ( color );
        narrow_tip->scale ( {0.5, 1} );
        narrow_tip->setPosition ( to );
        if ( hor ) {
            sf::Vector2f bend = {from.x, to.y};
            crossbar->rotate ( 90 );
            if ( from.y<to.y ) {
                vertical_line->setPosition ( from );
            } else {
                vertical_line->setPosition ( bend );
            }

            if ( from.x<to.x ) { //Skierowany w prawo
                horizontal_line->setPosition ( bend );
                tip->rotate ( 90 );
                tip->move ( {0, -8} );

                narrow_tip->rotate ( 90 );
                narrow_tip->move ( {0, -3} );
                crossbar->move ( {-20, -8} );
            } else { //Skierowany w lewo
                horizontal_line->setPosition ( to );
                tip->rotate ( -90 );
                tip->move ( {-2, 12} );
                narrow_tip->rotate ( -90 );
                narrow_tip->move ( {-2, 7} );
                crossbar->move ( {20, -8} );
            }
        } else { //vertical
            sf::Vector2f bend = {to.x, from.y};
            if ( from.y<to.y ) {
                vertical_line->setPosition ( bend );
                tip->rotate ( 180 );
                tip->move ( {12, 0} );
                narrow_tip->rotate ( 180 );
                narrow_tip->move ( {7, 0} );
                crossbar->move ( {-4, -20} );
            } else {
                vertical_line->setPosition ( to );
                tip->move ( {-8, 0} );
                narrow_tip->move ( {-3, 0} );
                crossbar->move ( {-4, 20} );
            }

            if ( from.x<to.x ) {
                horizontal_line->setPosition ( from );
            } else {
                horizontal_line->setPosition ( bend );
            }
        }


        if ( style == "Arrow" ) {
            arrowTip = tip;
        } else if ( style == "Flat" ) {
            arrowTip = crossbar;
        } else if ( style == "Pulse" ) {
            tip->setFillColor ( sf::Color::White );
            tip->setOutlineThickness ( 2.f );
            tip->setOutlineColor ( mw::ColorScheme::getColor ( "outline-normal" ) );
            arrowTip = narrow_tip;
        } else { //brak strzałki -> prostokąt 0x0
            arrowTip = std::make_shared<sf::RectangleShape> ( sf::Vector2f ( 0,0 ) );
        }

    }
    virtual void draw ( sf::RenderTarget * RT )
    {
        RT->draw ( *horizontal_line );
        RT->draw ( *vertical_line );
        RT->draw ( *arrowTip );
    }
    virtual void lightOn()
    {
        auto clr = mw::ColorScheme::getColor ( "outline-active" );
        horizontal_line->setFillColor ( clr );
        vertical_line->setFillColor ( clr );
        arrowTip->setFillColor ( clr );
    }
    virtual void lightOff()
    {
        auto clr = mw::ColorScheme::getColor ( "outline-normal" );
        horizontal_line->setFillColor ( clr );
        vertical_line->setFillColor ( clr );
        arrowTip->setFillColor ( clr );
    }
};
}
