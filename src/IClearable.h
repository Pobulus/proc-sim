#pragma once
/** @file */ //doxygen
namespace mw
{
class IClearable
{
    /**
     * Interfejs IClearable, przeznaczony dla obiektów u ulotnym stanie, który należy wyczyścić przed innymi operacjami.
     */
public:
    IClearable() {}
    virtual  ~IClearable() {}
    virtual void clear() =0;
};
}
