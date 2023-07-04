#pragma once
/** @file */ //doxygen
#include <memory>
#include <string>
#include <sstream>
#include <set>

namespace mw
{
/**
 * Interfejs węzła drzewa rozkazu
 * 
 */
class opNode
{
protected:
public:
    virtual ~opNode() {}
    /**
     * Zwraca wskaźnik na następny węzeł
     */
    virtual std::shared_ptr<mw::opNode> next ( unsigned short int opCode ) =0;
    /**
     * Zwraca zbiór sygnałów dla danego węzła 
     */
    virtual std::set<std::string> get() = 0;

};
}
