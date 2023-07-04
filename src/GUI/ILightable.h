#pragma once
/** @file */ //doxygen
#include "IDrawable.h"
namespace mw
{
class ILightable :  public IDrawable
{
protected:
    bool active;
public:
    ILightable()
    {
        active = false;
    }
    virtual ~ILightable() {};
    virtual void lightOn() =0;
    virtual void lightOff() =0;
};
}
