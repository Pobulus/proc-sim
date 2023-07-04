#pragma once

/** @file */ //doxygen

#include "Register.h"
#include "IClearable.h"

namespace mw
{
/**
 * Klasa jednostki arytmetyczno-logicznej. \n 
 * Traktowana jako element. Jej zawartość jest ulotna i powinna być wyczyszczona.
 */
class ALU : public ICallable, public IClearable, public Register
{
    Flags* flags;
    std::function<int ( int, int ) > op;
    bool op_set = false, inp_set = false;

    void setOperation ( std::function<int ( int, int ) > o )
    {
        if ( !op_set ) {
            op = o;
            op_set = true;
        } else {
            throw mw::ALUoverwriteOperation();
        }
    }
    void opSetAdd()
    {
        setOperation ( [] ( int x, int y ) {
            return x+y;
        } );
    }
    void opSetSub()
    {
        setOperation ( [&] ( int x, int y ) {
            int r = x-y;
            return ( int ) ( r>=0 ) ?r: ( 1<<width )+r;
        } );
    }
    void opSetPass()
    {
        setOperation ( [] ( int x, int y ) {
            return y;
        } );
    }
    void ALUin()
    {
        inp_set = true;
    }

    bool ZeroInAK()
    {
        return !value;
    }
    bool NegativeInAK()
    {
        return value>> ( width-1 );
    }


public:
    // Przy tworzeniu należy podać element z którego ALU będzie pobierało dane
    ALU ( short int width ) : Register ( width )
    {
        flags = &mw::Flags::getInstance();
    }

    virtual void push ( int val )
    {
        //alu jest jak rejestr, ale przed wpisaniem do niego danych, musi być włączone wejście i ustawiona operacja
        if ( op_set ) {
            if ( inp_set ) {
                //skróć wartośc do swojej szerokości
                value = op ( value, val ) % ( 1<<width );
                flags->set ( "Z", NegativeInAK() ); //aktualizuj flagi
                flags->set ( "zak", ZeroInAK() );
            } else {
                throw mw::ALUmissingInput();
            }
        } else {
            throw mw::ALUmissingOperation();
        }
    }




    virtual void reset()
    {
        value = 0;
    }


    void clear()
    {
        inp_set = false;
        op_set = false;
    }
    /**
     * ID wywołania: \n 
     * 0 -> ustaw operację na dodawanie \n 
     * 1 -> ustaw operację na odejmowanie \n 
     * 2 -> ustaw operację na przepisywanie \n 
     * 3 -> aktywuj wejście JAL
     */
    virtual void call ( short int call_id )
    {
        switch ( call_id ) {
        case 0: //dod
            opSetAdd();
            break;
        case 1: //ode
            opSetSub();
            break;
        case 2: //przep
            opSetPass();
            break;
        case 3: //weja
            ALUin();
            break;
        default:
            throw CallableMissingCallId ( call_id );

        };
    }
};
}
