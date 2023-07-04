#pragma once
/** @file */ //doxygen
#include <map>
#include <string>
namespace mw
{
/**
 * Obiekt przechowujący flagi (wartości bool o przypisanej nazwie)<Singleton> 
 */
class Flags
{
    std::map<std::string, bool> flags;
    Flags()
    {

    }
public:
    static Flags&getInstance();
    Flags ( const Flags&original ) = delete;
    Flags& operator= ( const Flags&original ) = delete;
    bool get ( std::string f )
    {
        return flags[f];
    }
    void set ( std::string f, bool  v )
    {
        flags[f] = v;
    }
};
}
mw::Flags &mw::Flags::getInstance()
{
    static mw::Flags singleton;
    return singleton;
}
