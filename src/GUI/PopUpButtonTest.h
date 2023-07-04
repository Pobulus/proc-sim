#pragma once
#include "IPopUp.h"
#include <filesystem>
#include <vector>
#include <cmath>
namespace mw
{
class PopUpButtonTest: public IPopUp
{
    std::shared_ptr<ButtonBar> widget;
    sf::Vector2i lastSize;
    sf::Event::SizeEvent size;
    std::vector<std::string> filenames;
    //static, bo musi być zainicjalizowane przed wywołaniem konstruktora
    static const unsigned int initialW=400,  initialH=400;
    int output;
    void addButtons ( int f, int t )
    {

        int all = t-f+1;
        if ( !all ) //pusty wektor, nie dodajemy przycisków
            return;
        int c = sqrt ( all );
        int r = all/c;

        for ( int y = 0; y < r; y++ ) {
            for ( int x  = 0; x<c; x++ ) {
                int index = f+y*c+x; //dodajemy 1, bo wartość 0 zwracana przez IClickable znaczy że nie trafiono na przycisk
                widget->addButton ( x*100, y*100, 90, 90, static_cast<mw::actionCode> ( index+2 ), std::string ( 1, ( char ) index ), "PWSmallIcons.ttf", sf::Color ( 150,150,250 ) );
            }
        }
        //dorysuj brakujący rząd
        for ( int x  = 0; x< ( all-r*c ); x++ ) {
            int index = r*c+x; //dodajemy 1, bo wartość 0 zwracana przez IClickable znaczy że nie trafiono na przycisk
            widget->addButton ( x*100, r*100, 90, 90, static_cast<mw::actionCode> ( index+2 ), std::string ( 1, ( char ) index ), "PWSmallIcons.ttf", sf::Color ( 150,150,250 ) );
        }


    }
public:
    PopUpButtonTest ( int from, int to ) : IPopUp ( initialW, initialH, "Test znaków" )
    {
        widget = std::make_shared<ButtonBar> ( sf::Vector2f ( initialW, initialH ), sf::FloatRect ( 0,0,1,1 ) );
        size.width = initialW;
        size.height = initialH;
        addButtons ( from, to );
        widget->addMargin ( 30 );
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
            std::cout << "index"<<index << std::endl;
            if ( index ) { // nie puste kliknięcie
                window.close();
                output = index-2;
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
        return  output; 
    }

};
}


