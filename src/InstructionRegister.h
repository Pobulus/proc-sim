#pragma once
/** @file */ //doxygen
#include "Register.h"

namespace mw
{
/**
 * Rejestr Rozkazów. Umożliwia dekodowanie kodu rozkazu z wpisanej wartości
 */
class InstructionRegister : public Register
{
    short unsigned int data_bits;

public:
    InstructionRegister ( bit_widths bits ) : Register ( bits.code+bits.data )
    {
        this->data_bits = bits.data;
    }
    /**
     * Dekoduje kod operacji z wartości wpisanej do rejestru.
     */
    int getOpCode()
    {
        return ( value>>data_bits );
    }
    void push ( int val )
    {
        //Jeśli wartość większa niż szerokość magistrali - bierzemy modulo
        value = val% ( 1<<width );
    }
    void reset()
    {
        mw::Register::reset();
    }
};
}
