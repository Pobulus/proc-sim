#pragma once
/** @file */ //doxygen
#include "ButtonBar.h"
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
namespace mw
{
    
class IPopUp
{
protected:
    sf::RenderWindow window;
public:
    IPopUp ( unsigned int w, unsigned int h, std::string titlebar )
    {
        window.create ( sf::VideoMode ( w, h ), titlebar );
        window.setActive ( false );
    }

    virtual ~IPopUp() {}
    virtual void handleEvent(sf::Event event) = 0;
    void display() {
        window.setActive ( true );

        while ( window.isOpen() ) {
            sf::Event event;
            if(!window.hasFocus()) {
                window.close();
                return;
            }
            while ( window.pollEvent ( event ) ) {
                handleEvent(event);
            }

        }
        return;
    }
};
}


