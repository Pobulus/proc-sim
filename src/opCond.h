#pragma once
/** @file */ //doxygen
#include "opNode.h"
#include "Flags.h"
namespace mw
{
/**
 * Węzeł drzewa rozkazów dla warunku. Przechowuje wskaźniki na węzły jeśli flaga jest prawda albo fałszem.
 */
class opCond : public opNode
{
    std::shared_ptr<mw::opNode> NEXT=nullptr;
    std::string flag;
    std::pair<std::shared_ptr<mw::opNode>, std::shared_ptr<mw::opNode>> nexts;
    Flags* flags;
    int count=0;
public:
    opCond ( std::string flag ) : flag ( flag )
    {
        flags = &mw::Flags::getInstance();
    }
    std::set<std::string> get()
    {
        if ( flags->get ( flag ) ) {
            NEXT = nexts.first;
        } else {
            NEXT = nexts.second;
        }
        return NEXT->get();
    }
    void addNodeTrue ( std::shared_ptr<mw::opNode> N )
    {
        nexts.first  = N;
    }
    void addNodeFalse ( std::shared_ptr<mw::opNode> N )
    {
        nexts.second = N;
    }
    std::shared_ptr<mw::opNode> next ( unsigned short int opCode )
    {
        return NEXT->next ( opCode );
    }
};
}
