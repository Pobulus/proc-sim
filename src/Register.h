#pragma once
/** @file */ //doxygen
#include "IElement.h"
namespace mw
{
/**
 * Klasa reprezentująca rejestr. Szerokość bitowa podawana w konstruktorze.
 */
class Register : public IElement
{
public:
    Register ( unsigned short int width )
    {
        this->width = width;
    }

    void push ( int val )
    {
        setValue(val);
    }
    int const pull()
    {
        return value;
    }

};
}
