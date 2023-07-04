#pragma once

/** @file */ //doxygen

#include "IPopUp.h"
#include <cmath>
namespace mw
{
class PopUpSelectString : public IPopUp
{
    sf::Event::SizeEvent size;
    //static, bo musi być zainicjalizowane przed wywołaniem konstruktora
    static const unsigned int initialW=600,  initialH=400;
protected:
    std::shared_ptr<ButtonBar> widget;
    std::vector<std::string> strings;
    int output = 0;
    virtual void addButton(int x, int y, int index) {
        widget->addButton ( x*300, y*100, 290, 90, static_cast<mw::actionCode> ( index+2 ), strings[index], "orange_kid.otf", sf::Color ( 150,150,250 ));
    }
    void addButtons()
    {
        int all = strings.size();
        if ( !all ) //pusty wektor, nie dodajemy przycisków
            return;
        int c = sqrt ( all );
        int r = all/c;

        for ( int y = 0; y < r; y++ ) {
            for ( int x  = 0; x<c; x++ ) {
                int index = y*c+x; //dodajemy 1, bo wartość 0 zwracana przez IClickable znaczy że nie trafiono na przycisk
//                 widget->addButton ( x*300, y*100, 290, 90, static_cast<mw::actionCode> ( index+2 ), strings[index], "orange_kid.otf", sf::Color ( 150,150,250 ) );
                addButton(x,y, index);
            }
        }
        //dorysuj brakujący rząd
        for ( int x  = 0; x< ( all-r*c ); x++ ) {
            int index = r*c+x; //dodajemy 1, bo wartość 0 zwracana przez IClickable znaczy że nie trafiono na przycisk
//             widget->addButton ( x*300, r*100, 290, 90, static_cast<mw::actionCode> ( index+2 ), strings[index], "orange_kid.otf", sf::Color ( 150,150,250 ) );
            addButton(x,r,index);
        }


    }
public:
    PopUpSelectString (std::string title ) : IPopUp ( initialW, initialH, title) {
        widget = std::make_shared<ButtonBar> ( sf::Vector2f ( initialW, initialH ), sf::FloatRect ( 0,0,1,1 ) );
        size.width = initialW;
        size.height = initialH;

    }
    PopUpSelectString (std::vector<std::string>&strings, std::string title ) : IPopUp ( initialW, initialH, title), strings(strings)
    {
        widget = std::make_shared<ButtonBar> ( sf::Vector2f ( initialW, initialH ), sf::FloatRect ( 0,0,1,1 ) );
        size.width = initialW;
        size.height = initialH;
        addButtons();
        widget->addMargin ( 30 );
    }
    virtual void handleEvent(sf::Event event) {
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
                output = index;
                return;

            }

        } else if ( event.type==sf::Event::KeyReleased ) {
            if ( event.key.code == sf::Keyboard::Return ) {
                window.close();
            }


        }
        window.clear ( mw::ColorScheme::getColor ( "main-background" ) );
        widget->resize ( size.width, size.height );
        widget->display();
        widget->draw ( &window );
        window.display();
    }
    int getValue()
    {
        output = 0;
        display();
        return output;
    }
    std::string getString()
    {
        int index = getValue();
        
        if ( index ) {
            return strings[index-2];
        }
        return "";

    }
};
}
