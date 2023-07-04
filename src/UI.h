#pragma once
/** @file */ //doxygen
#include "Maszyna.h"
/**
 * Interfejs interfejsu użytkownika.
 * Wymaga zdefiniowania metod:
 * - loadFromFile() która ładuje konfigurację z podanego pliku,
 * - run() która wykonuje wewnętrzną logikę sterującą maszyną.
 * Jeśli nie występują błędy obie metody powinny zwrócić 0.
 */
class UI
{
protected:
    mw::Maszyna* maszyna;
    mw::OpSet* opSet;
    std::map<std::string, std::shared_ptr<mw::IElement>>* elements;
    mw::bit_widths bits;

public:
    UI ( mw::Maszyna& cpu ) : maszyna ( &cpu )
    {
        opSet = &mw::OpSet::getInstance();
        bits = opSet->getBits();
        //bits = UI::maszyna->getBitWidths();

        elements = cpu.getElements();
    }
    virtual ~UI()
    {
        maszyna = nullptr;
        elements->clear();
    }
    virtual int run() =0;
    virtual int loadFromFile ( const std::string &filename ) =0;


};
