#pragma once
/** @file */ //doxygen
#include "../OpSet.h"
#include "PopUpSelectString.h"
#include <filesystem>
namespace mw
{
class PopUpOperationInput : public PopUpFileInput
{
    void addButton(int x, int y, int index){
        auto color = sf::Color ( 150,150,250 );
        std::string opname = mw::OpSet::getInstance().checkLoaded(strings[index]);
        std::string button_text = strings[index];
        if(opname != ""){
            color = sf::Color(250,250,150);
            button_text+="["+opname+"]";
        }
        widget->addButton ( x*300, y*100, 290, 90, static_cast<mw::actionCode> ( index+2 ), button_text, "monospace.otf", color);
    }


public:
    PopUpOperationInput ( ) : PopUpFileInput ( "Wybierz plik rozkazu")
    {
        findFilesWithExtension ( ".rzk" );      
        addButtons();
        widget->addMargin ( 60 );
    }

};
}
