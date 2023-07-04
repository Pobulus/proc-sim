#pragma once
/** @file */ //doxygen
#include <map>
#include <functional>
#include "Exceptions.h"
namespace mw
{
class ICallable
{
    /**
     * Interfejs ICallable dla obiektów które mogą być wywołane. 
     * Jeśli metoda call() zostanie wywołana z niedefiniowaną wartością - powinna rzucić wyjątek CallableMissingCallId
     */
public:
    ICallable() {};
    virtual ~ICallable() {};
    virtual void call ( short int call_id ) = 0;

};
}
