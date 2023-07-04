#pragma once
/** @file */ //doxygen
#include <vector>
#include "opNode.h"
#include "Exceptions.h"
namespace mw
{

/**
 * Ta klasa jest korzeniem drzewa rozkazów.
 * Wszystkie rozkazy muszą zaczynać się od tej samej linii, 
 * jeśli ta zasada zostanie złamana rzucony jest wyjątek OpPrimaryLineMismatch
 **/
class opCommon : public opNode
{
    std::shared_ptr<mw::opNode> NEXT=nullptr;
    std::set<std::string> line;
    std::vector<std::shared_ptr<mw::opNode>> nexts;
    unsigned short int size;
public:
    opCommon ( unsigned short int size ) : size ( size )
    {

    }
    void setText ( std::set<std::string> input )
    {
        if ( input != line ) { //jeśli wczytana warstość różni się od obecnej
            if ( line.size() ==0 ) { //jeśli obecna wartość jest pusta
                line = input;
            } else { //jeśli wczytana wartość się różni, a obecna nie jest pusta: rzuć wyjątek
                throw mw::OpPrimaryLineMismatch();
            }
        }
    }

    void replaceNode ( std::shared_ptr<mw::opNode> N, int opCode )
    {
        if ( opCode < nexts.size() ) {
            nexts[opCode]=N;
        }else{
            throw OpSetCannotReplace(opCode);
        }

    }

    void addNode ( std::shared_ptr<mw::opNode> N )
    {
        if ( nexts.size() < size ) {
            nexts.push_back ( N );
        } else {
            throw mw::OpSetFull();

        }
    }
    std::set<std::string> get()
    {
        return line;
    }
    std::shared_ptr<mw::opNode> next ( unsigned short int opCode )
    {
        std::cout << nexts.size()<<std::endl;
        if(nexts.size()==0)
          throw mw::OpSetEmpty();
        if ( opCode >= nexts.size() )
            throw mw::OpSetNotDefined ( opCode );
        return nexts[opCode];

    }
};

}
