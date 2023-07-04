#pragma once
/** @file */ //doxygen
#include "UI.h"

class CLI : public UI
{

    void showElements()
    {
        for ( auto el : *elements ) {
            std::cout << el.first << ":"<< el.second->pull() <<", ";
        }
    }
public:
    CLI ( mw::Maszyna& cpu ) : UI ( cpu )
    {

    }

    bool update()
    {
        bool halt = false;
        std::string choice;
        std::cin >> choice;
        if ( choice == "t" ) { //wykonajtakt
            mw::status temp = maszyna->tick();
            for ( auto s : temp.activeSignals ) {
                std::cout << s<< " ";
                if ( s == "stop" ) halt = true;
            }
            std::cout <<std::endl;
            showElements();
            std::cout << "Z:"<< mw::Flags::getInstance().get("Z")<<std::endl;
            std::cout << "zak:"<< mw::Flags::getInstance().get("zak")<<std::endl;

            std::cout <<std::endl;
        } else if ( choice == "s" ) { //Ustaw wartość
            std::string label;
            int value;
            std::cin >> label >> value;
            std::cout << "SET" << std::endl;
            ( *elements ) [label]->push ( value );
            showElements();
        } else if ( choice == "u" ) { //pokaż elementy
            showElements();
        }
        return !halt;

    }
    int run()
    {
        while ( update() );
        return 0;
    }
};
