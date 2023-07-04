#pragma once
/** @file */ //doxygen
#include "opNode.h"

namespace mw
{
/**
 * Węzeł drzewa rozkazu przechowujący pojedynczą linię i wzkaźnik na następny węzeł
 */
class opLine : public opNode
{
    std::shared_ptr<mw::opNode> NEXT=nullptr;
    /**
     * sygnały przechowywane w zbiorze, by uniknąc duplikatów
     *
     **/
    std::set<std::string> line;
public:
    opLine ( std::string line )
    {
        setText ( line );
    }
    std::set<std::string> get()
    {
        return line;
    }

    void setText ( std::string text )
    {
        std::string sig;
        std::stringstream ss ( text );
        while ( ss >> sig ) {
            line.insert ( sig );
        }
    }

    void addNode ( std::shared_ptr<mw::opNode> N )
    {
        NEXT = N;
    }
    // potencjalny konflikt nazw, jeśli nie to line można zmienić na text
    std::shared_ptr<mw::opNode> next ( unsigned short int opCode )
    {
        return NEXT;
    }
};
}
