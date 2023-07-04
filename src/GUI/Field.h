#pragma once
/** @file */ //doxygen
#include "../IElement.h"
#include "PopUpNumberInput.h"
#include <bitset>

//Różne tryby wyświetlania BIN, HEX,DEC, DEC signed, KOD(DANE)
namespace mw
{
    
    
enum format_type {
    dec_unsigned,
    dec_signed,
    binary,
    hex,
    code,
    dec_and_code
};
class Field : public mw::IDrawable, public mw::IScrollable
{
    std::shared_ptr<IElement> hook;
    sf::Text value;
    format_type format = binary;
    std::string formatValue(int value){
        std::stringstream ss;
        auto bits = mw::OpSet::getInstance().getBits();
        switch(format){
            case dec_unsigned:
                ss << value;
                break;
            case dec_signed:
            {
                int full_bits = (bits.code+bits.data);
                bool sign_bit  = (value>>(full_bits-1));
                int max_value = (1<<full_bits);
                int signed_value = value-max_value*sign_bit;
                ss << signed_value;
            }
                break;
            case hex:
                ss << std::hex << value;
                break;
            case binary:
                ss << std::bitset<8>(value); //musi mieć stałą wartość w szablonie
                break;
            case code:
            {
                unsigned short int opCode = value>>bits.data;
                int address = value%(1<<bits.data);
                ss << mw::OpSet::getInstance().getOpName(opCode)<<"("<<address<<")";
                
            }
            break;
            case dec_and_code:
            {
                 ss << value << " ";
                unsigned short int opCode = value>>bits.data;
                int address = value%(1<<bits.data);
                ss << mw::OpSet::getInstance().getOpName(opCode)<<"("<<address<<")";
                
            }
            break;
                
        }
        
        return ss.str();
    }
public:
    Field ( float x, float y )
    {
        value.setPosition ( {x,y} );
        value.setFillColor ( mw::ColorScheme::getColor ( "outline-normal" ) );
        value.setCharacterSize ( 60 ); //renderuj jako większy rozmiar znaków, żeby mniej się rozmywał
        value.setScale ( 0.5,0.5 ); //przeskaluj do pierwotnego rozmiaru, ale mniej rozmyte
        

    }
    void setupHook ( std::shared_ptr<IElement>& el )
    {
        if ( el == nullptr ) {
            std::cerr << "warning, hooked to nullptr!"<<std::endl;
        }
        hook = el;
    }
    void draw ( sf::RenderTarget * RT )
    {
        value.setString ( formatValue ( hook->getValue() ) );
        RT->draw ( value );
    }
    actionCode clicked ( sf::Vector2f event )
    {
        mw::PopUpNumberInput popUp ( "Enter value to input, confirm  with Return", hook->getValue() );
        setValue ( popUp.getValue() );
        return handled;
    }
     virtual bool scrolled(sf::Vector2f event, int ammount ){
        format = (format_type)(format+ammount);
        if(format < 0)
             format = dec_unsigned;
        else if(format > dec_and_code)
            format = dec_and_code;
        return true;
    }
    void setFont ( const sf::Font &F )
    {
        value.setFont ( F );
        
        //ustaw pole wykrywania klinkięcia, trzeba to zrobić dopiero po ustawieniu czcionki 
        format = binary;
        value.setString ( formatValue ( 0 ) ); 
        //najszerza możliwa wartość, żeby można było ustawić odpowiednio szerokie pole wykrywania klinknięcia
        this->area = value.getGlobalBounds();
        areaAddMargin ( 30 ); //poszerz pole w którym wykrywane jest kliknięcie, żeby łatwiej było traifć myszą
        format = dec_unsigned; //domyślny format
        
    }
    void setValue ( int val )
    {
        hook->setValue( val );
    }
    void setFormat(format_type F){
        format = F;
    }
};
}
