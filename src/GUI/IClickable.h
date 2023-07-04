#pragma once
/** @file */ //doxygen
#include <SFML/Graphics.hpp>


namespace mw
{
enum actionCode {
    empty,
    handled,

    reset,
    do_tick,
    do_op,
    do_prog,

    config,
    reload,
    color,
    add_inst,
    open_prog,
    open_inst,
    open_editor,
    assemble,

    quit
};

class IClickable
{
protected:
    sf::FloatRect area;
    void areaAddMargin ( float m )
    {
        area.height += m;
        area.width  += m;
        area.left   -= m/2;
        area.top    -= m/2;
    }

public:
    virtual ~IClickable() {}
    /**
    Sprawdza czy kliknięcie na danych koordynatach zawiera się w polu interakcji obiektu
    jeśli tak zwraca wynik metody clicked, jeśli nie - pusta akcja
    Jest Vectorem2f, a nie eventem, ponieważ przy przekazywaniu kliknięcia do wewnętrznych obiektów,
    potrzena jest konwersja koordynatów i łatwiej ją wywołać w ten sposób
    */
    actionCode check ( sf::Vector2f event )
    {
        if ( area.contains ( event ) ) {
            return clicked ( event );
        }
        return empty;
    }
    virtual actionCode clicked ( sf::Vector2f event ) =0;
};
}
