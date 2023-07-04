#pragma once
/** @file */ //doxygen
#include "Register.h"
namespace mw
{
/**
 * Klasa reprezentująca pamięć ram. \n 
 * Przechowuje wektor komórek pamięci oraz dwa rejestry sterjuące Adres [A] orac Słowo [S].
 */
class RAM : public ICallable
{
    std::shared_ptr<mw::Register> A;
    std::shared_ptr<mw::Register> S;
    std::vector<std::shared_ptr<mw::Register>> Memory;

    void read()
    {
        S->push ( Memory[A->pull()]->pull() );
    }
    void write()
    {
        Memory[A->pull()]->push ( S->pull() );
    }

public:
    RAM ( short int width, short int addr_width )
    {
        A = std::make_shared<mw::Register> ( addr_width );
        S = std::make_shared<mw::Register> ( width );
        int cells = ( int ) pow ( 2, addr_width );
        for ( int i = 0; i< cells; i++ ) {
            Memory.push_back ( std::make_shared<mw::Register> ( width ) );
        }

    }
    ~RAM()
    {
    }
    /**
     * ID wywołań: \n 
     * 0 -> odczytaj wartość komórki pod adresem w rejestze A i zapisz w rejestze S \n 
     * 1 -> zapisz zawartość rejestru S do komórki pod adresem w rejestze A
     */
    virtual void call ( short int call_id )
    {
        switch ( call_id ) {
        case 0: //czyt
            read();
            break;
        case 1: //pisz
            write();
            break;
        default:
            throw CallableMissingCallId ( call_id );

        };
    }
    /**
     * Załaduj dane z wektora do pamięci, jeśli nie zmieszczą się w przestrzeni adresowej rzuca wyjątek RAMnotEnoughMemory
     */
    std::shared_ptr<mw::IElement>  loadData ( std::vector<int> &data )
    {
        if(data.size() > Memory.size()) {
            throw RAMnotEnoughMemory(data.size()-Memory.size());
        }

        for ( int i = 0; i< data.size(); i++ ) {
            Memory[i]->push ( data[i] );
        }
        /**zwraca wskaźnik do swojego rejestru adresowego*/
        return A;
    }
    void reset() {
        A->reset();
        S->reset();
    }
    /**
     * Zwraca wektor wskaźników na zawarte w sobie elementy, gdzie przedostatnim elementem jest rejestr A, a ostatnim rejestr S
     */
    std::vector<std::shared_ptr<mw::Register>> getRegisters()
    {
        std::vector<std::shared_ptr<mw::Register>> out = Memory;
        out.push_back ( A );
        out.push_back ( S );
        return out;
    }
};
}
