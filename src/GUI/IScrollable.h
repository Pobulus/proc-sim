#pragma once
/** @file */ //doxygen
#include "IClickable.h"

namespace mw
{

class IScrollable : public IClickable
{
protected:
    int currentScroll;
public:
    IScrollable() : IClickable(){}
    virtual ~IScrollable() {}
    void setScroll(int v){
        currentScroll = v;
    }
    /**
    Sprawdza czy przewijanie na danych koordynatach zawiera się w  polu interakcji obiektu
    jeśli tak zwraca wynik metody scrolled, jeśli nie - pusta akcja
    */
    virtual bool scroll(sf::Vector2f event, int ammount){
         if ( area.contains ( event ) ) {
            return scrolled ( event, ammount );
        }return false;
    }
    virtual bool scrolled (sf::Vector2f event, int ammount ) =0;
};
}
