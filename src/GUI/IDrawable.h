#pragma once
/** @file */ //doxygen
#include "ColorScheme.h"
/**
 * Ta klasa interfejs rysowalnych elementów
 **/
namespace mw
{
class IDrawable
{
public:
    IDrawable() {}
    virtual ~IDrawable() {}
    virtual void draw ( sf::RenderTarget* RT ) = 0;
};
}
