#pragma once
/** @file */ //doxygen
#include "TitleBar.h"
#include "IPopUp.h"
#include "SFX.h"

namespace mw
{
class PopUpWarning: public IPopUp
{
    //static, bo musi być zainicjalizowane przed wywołaniem konstruktora
    static const unsigned int initialW=300,  initialH=200;
    std::shared_ptr<TitleBar> widget;
    sf::Vector2i lastSize;
    sf::Event::SizeEvent size;
public:
    PopUpWarning ( std::string warning ) : IPopUp ( initialW, initialH, "Warning!" )
    {

        widget = std::make_shared<TitleBar> (
                     sf::Vector2f ( initialW, initialH ),
                     sf::FloatRect ( 0.05,0.1,0.9,0.8 ),
                     warning, "monospace.otf" );
        widget->setBGColor ( mw::ColorScheme::getColor ( "block-fill" ) );
        widget->setTextColor ( mw::ColorScheme::getColor ( "outline-normal" ) );
        size.width = initialW;
        size.height = initialH;
        mw::SFX::get().Error();
    }
    virtual void handleEvent(sf::Event event){
         if ( event.type==sf::Event::Closed ) {
                    window.close();
                } else if ( event.type==sf::Event::Resized ) {
                    size = event.size;
                } else if ( event.type==sf::Event::MouseButtonPressed ) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition ( window );
                    auto mousePosInWin = window.mapPixelToCoords ( mousePos );

                    int index = widget->check ( mousePosInWin );
                    
                    if ( index ) { // nie puste kliknięcie
                        window.close();
                        return;

                    }
                } else if ( event.type==sf::Event::KeyReleased ) {
                    if ( event.key.code == sf::Keyboard::Return ) {
                        window.close();
                    }

                }

                window.clear ( mw::ColorScheme::getColor ( "warning-background" ) );
                widget->resize ( size.width, size.height );
                widget->display();
                widget->draw ( &window );
                window.display();
    }

};
}
