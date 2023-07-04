#pragma once
#include "ICallable.h"
#include "IElement.h"

#include "Connect.h"
#include <vector>
/**
 * Ta klasa to zamysł rozszerzenia - multiplekser
 * pozwoliłaby zmienić cel sygnału w zaleźności od wybranych bitów
 * na magistrali
 *
 **/
namespace mw
{
class MUX : public ICallable, public IElement
{
    std::vector connections;
    int pull()
    {
        return 0;
    }

}
}
