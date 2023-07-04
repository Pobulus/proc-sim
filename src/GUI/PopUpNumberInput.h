#pragma once
/** @file */ //doxygen
#include "TitleBar.h"
#include "IPopUp.h"
namespace mw
{
class PopUpNumberInput : public IPopUp
{
    //static, bo musi być zainicjalizowane przed wywołaniem konstruktora
    static const unsigned int initialW=200,  initialH=200;
    std::shared_ptr<TitleBar> widget;
    int number;
    sf::Vector2i lastSize;
    sf::Event::SizeEvent size;
    void addDigit ( int d )
    {
        number = number*10;
        number += d;
    }
    void removeDigit()
    {
        number = number/10;
    }
public:
    PopUpNumberInput ( std::string titlebar, int initial =0 ) : IPopUp ( initialW, initialH, titlebar )
    {
        widget = std::make_shared<TitleBar> (
                     sf::Vector2f ( initialW, initialH ),
                     sf::FloatRect ( 0.05,0.1,0.9,0.8 ),
                     "0", "orange_kid.otf" );
        widget->setBGColor ( mw::ColorScheme::getColor ( "block-fill" ) );
        widget->setTextColor ( mw::ColorScheme::getColor ( "outline-normal" ) );
        size.width = initialW;
        size.height = initialH;
        number = initial;
    }
    virtual void handleEvent(sf::Event event) {
        if ( event.type==sf::Event::Closed ) {
            window.close();
        } else if ( event.type==sf::Event::Resized ) {
            size = event.size;
        } else if ( event.type==sf::Event::KeyReleased ) {
            if ( event.key.code >=sf::Keyboard::Num0 and  event.key.code <= sf::Keyboard::Num9 ) {
                addDigit ( event.key.code-sf::Keyboard::Num0 );
            } else if ( event.key.code >=sf::Keyboard::Numpad0 and  event.key.code <= sf::Keyboard::Numpad9 ) {
                addDigit ( event.key.code-sf::Keyboard::Numpad0 );
            } else if ( event.key.code == sf::Keyboard::Dash ) {
                number = -number;
            } else if ( event.key.code == sf::Keyboard::Delete ) {
                number = 0;
            } else if ( event.key.code == sf::Keyboard::Backspace ) {
                removeDigit();
            } else if ( event.key.code == sf::Keyboard::Return ) {
                window.close();
            }

        }
        window.clear ( mw::ColorScheme::getColor ( "main-background" ) );
        widget->setText ( std::to_string ( number ) );
        widget->resize ( size.width, size.height );
        widget->display();
        widget->draw ( &window );
        window.display();
    }

    int getValue()
    {
        display();
        return number;
    }
};
}


