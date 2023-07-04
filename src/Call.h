#pragma once
/** @file */ //doxygen
#include "ISignal.h"
#include "ICallable.h"
#include <memory>
namespace mw
{
class Call : public ISignal
{
    const std::shared_ptr<mw::ICallable> target;
    const short int call_id;
    std::string call_name;
public:
    Call ( std::shared_ptr<mw::ICallable> target, short int call_id ) : target ( target ), call_id ( call_id )
    {

    }
    void send()
    {
        target->call ( call_id );
    }
};
}
