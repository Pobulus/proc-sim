#pragma once

/** @file */ //doxygen

#include "Register.h"

namespace mw
{
/**
 * Licznik, Rejestr z możliwością inkrementacji/dekrementacji wartości
 */
class Counter : public Register, public ICallable
{
protected:
    void increment()
    {
        push ( getValue()+1 );
    }
    void decrement()
    {
        push ( getValue()-1 );
    }
public:
    Counter ( unsigned short int width ) : Register ( width )
    {

    }
	/**
	 * ID wywołań:\n 
	 * 0 -> inkrementuj
	 * 1 -> dekrementuj
	 */
    virtual void call ( short int call_id )
    {
        switch ( call_id ) {
        case 0:
            increment();
            break;
        case 1:
            decrement();
            break;
        default:
            throw CallableMissingCallId ( call_id );
        };
    }


};
}
