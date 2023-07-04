#pragma once
/** @file */ //doxygen
#include "ISignal.h"
namespace mw
{
/**
 * Pusty sygnał
 */
class Empty : public ISignal
{
public:
    Empty() { }
    void send() { }
};
}
