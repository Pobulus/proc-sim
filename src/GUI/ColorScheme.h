#pragma once
/** @file */ //doxygen
#include <SFML/Graphics.hpp>

#include <map>
#include "../JSON.h"

namespace mw
{
class ColorScheme
{
    ColorScheme() {};
    std::map<std::string, sf::Color> colors;
public:
    void loadFromFile ( std::string filename )
    {

        json config = openJSON(filename);
        for ( auto color : config["colors"].items() ) {
            auto  values = color.value();

            try {
                colors[color.key()] = sf::Color ( values[0], values[1], values[2] );
            } catch ( std::exception exc ) {
                std::cerr << exc.what() << std::endl;
            }
        }


    }
    ColorScheme ( const ColorScheme&original ) = delete;
    ColorScheme& operator= ( const ColorScheme&original ) = delete;

    static ColorScheme& getInstance();
    static sf::Color getColor ( std::string name );
};
}

mw::ColorScheme& mw::ColorScheme::getInstance()
{
    static mw::ColorScheme CS; //singleton
    return CS;
}

sf::Color mw::ColorScheme::getColor ( std::string name )
{

    if ( mw::ColorScheme::getInstance().colors.count ( name ) )
        return mw::ColorScheme::getInstance().colors[name];
    std::cerr << "nie znaleziono koloru" << name <<std::endl;
    return sf::Color::Black; //jeśli niezdefiniowany
}
