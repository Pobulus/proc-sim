#pragma once
/** @file */ //doxygen
namespace mw
{
/**
 * Interfejs sygnału aktywowanego metodą send().
 */
class ISignal
{
public:
    ISignal() {}
    virtual ~ISignal() {}
    virtual void send() =0;
};
}
