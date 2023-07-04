#pragma once
/** @file */ //doxygen
#include <cmath>
#include "IElement.h"
#include "IClearable.h"

namespace mw
{
/**
 * Klasa magistrali. Zawartość magistrali jest ulotna więc powinna zostać wyczyszczona przed użyciem.
 */
class Bus : public mw::IElement, public mw::IClearable
{

    bool isEmpty;
public:
    Bus ( int width=0 )
    {
        this-> width = width;
    }
    void push ( int val )
    {
        if ( !isEmpty )
            throw mw::BusInUse();

        //Jeśli wartość większa niż szerokość magistrali - bierzemy modulo
        value = val% ( 1<<width );
        isEmpty = false;
    }
    int const pull()
    {

        if ( isEmpty )
            throw mw::BusEmpty();
        return value;
    }
    void clear()
    {
        isEmpty = true;
    }

};

}
