#pragma once
/** @file */ //doxygen
#include "ICallable.h"
#include "OpSet.h"
namespace mw
{
/**
 * Interfejs elementu. Element to obiekt który przechowuje pewną wartość [value] o określonej szerokości bitowe [width].
 * Do bezpośredniego ustawiania i pobierania wartości służą metody getValue() oraz setValue().
 * Szerokość elementu można sprawdzić metoda getWidth().
 * Do przesyłania wartości między elementami należy wykorzystywać metody push() i pull().
 * Metoda reset() zeruje wartość.
 */
class IElement
{
protected:
    unsigned short int width;
    int value=0;
public:
    IElement() {}
    virtual ~IElement() {};
    int const getValue()
    {
        return value;
    }
    void setValue ( int val )
    {
        //Jeśli wartość większa niż szerokość - bierzemy modulo
        value = val% ( 1<<width );
        if(value < 0) { // symulacja underflow
            value = (1<<width)+value;
        }
    }
    unsigned short int getWidth() {
        return width;
    }
    virtual void push ( int val ) =0;
    virtual int const pull() =0;
    virtual void reset()
    {
        value = 0;
    }
};
}
