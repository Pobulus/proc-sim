#pragma once
#include "Bus.h"
#include <functional>

namespace mw
{
class OperationBus : public Bus
{
    std::function<int ( int, int ) > op;
    std::shared_ptr<mw::Register> accumulator;
    bool op_set = false, inp_set = false;
public:
    OperationBus()
    {

    }
    void push ( int val )
    {
        if ( !isEmpty )
            throw mw::BusInUse();
        inp_set = true;
        if ( op_set ) {
            value = op ( val, accumulator->getValue() ) % ( int ) pow ( 2, width );
            isEmpty = false; //zewnętrzny rejestr może pobrać dane
        } else { //Jeśli wartość większa niż szerokość magistrali - bierzemy modulo
            value = val% ( int ) pow ( 2, width );
        }
    }
    void setOp ( std::function<int ( int, int ) > o )
    {
        op = o;
        op_set = true;
        if ( inp_set ) {
            value = op ( value, accumulator->getValue() ) % ( int ) pow ( 2, width );
            isEmpty = false;
        }

    }
    void clear()
    {
        op_set = false;
        inp_set = false;
        isEmpty = true;
    }
};
}
