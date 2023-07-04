#pragma once
/** @file */ //doxygen
#include "ISignal.h"
#include "IElement.h"
namespace mw
{
/**
 * Sygnał który po wywołaniu przesyła wartość z elementu input do elementu output
 */
class Connect : public ISignal
{
    std::shared_ptr<IElement> input;
    std::shared_ptr<IElement> output;
public:
    Connect ( std::shared_ptr<IElement> input, std::shared_ptr<IElement> output ) : input ( input ), output ( output )
    {

    }
    void send()
    {
        output->push ( input->pull() );
    }
};
}
