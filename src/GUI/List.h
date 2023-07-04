#pragma once
/** @file */ //doxygen
#include "Field.h"
#include <cmath>
#include <queue> //do sortowania pól
namespace mw
{

class List : public ILightable, public IScrollable
{
    std::vector<mw::Field> fields;
    std::vector<mw::Block> blocks;
    std::vector<std::shared_ptr<IElement>> cells;
    std::shared_ptr<IElement> address;
    int previousAddress = 0;
    const int display_length;
public:
    List ( float x, float y, int length ) : display_length ( length )
    {
        currentScroll = 0;
        for ( int i = 0; i<length; i++ ) {
            fields.push_back ( {x,y+ ( i*30.f ) } );
            blocks.push_back ( {x,y+ ( i*30.f )+10, 150, 25} );
        }
        this->area = {x,y,150, 30.f*length};
    }
    void setupAddress ( std::shared_ptr<IElement> &ad )
    {
        address =ad;
    }
    void hookCells ( std::string element_prefix,std::map<std::string, std::shared_ptr<mw::IElement>>* elements, int from, int to )
    {
        std::cout << "Hooking Cells:\n";
        if(to < 0){ // jeśli podano liczbę mniejszą niż 0 to należy wziąć wszystkiekomórki pamięci
          int count=0;
          for(auto el : *elements){
            if(el.first.find ( element_prefix+"#" ) !=std::string::npos )count++;
          }
          //policz komórki pamięci z tym prefiksem i odejmi 2 na rejestr A i S
          to = count-2;
        }
        for ( int i = from; i < to; i++ ) {
            std::string name = element_prefix+"#"+std::to_string ( i );
            std::cout << name;
            cells.push_back ( ( *elements ) [name] );
        }

    }
    void setFont ( const sf::Font &F )
    {
        for ( auto &field : fields ) { //Wydrukuj
            field.setFont ( F );
            field.setFormat(dec_and_code); // w tym formacie mają się wyświeltać pola na liście
        }

    }

    void updateScroll()
    {
        int inRegister = address->getValue();
        //Jeśli zmieniła się wartość w rejestrze, przescrolluj do niej
        if(inRegister != previousAddress){
            previousAddress = inRegister;

            int offset = previousAddress - currentScroll;
            if(offset < 0 or offset >= display_length){
                currentScroll = previousAddress;
            }
        }
        currentScroll= std::min ( currentScroll, ( int ) cells.size()-display_length );

    }
    ~List() {}
    virtual void draw ( sf::RenderTarget * RT )
    {
        updateScroll();


        lightOff();
        for ( auto &B : blocks ) { //Wydrukuj ramki
            B.draw ( RT );
        }
        for ( int i = 0; i<display_length; i++ ) { //załaduj do pól odpowiednie elementy, wskazane przez rejestr w address
//         std::cout << i+currentScroll<<std::endl;
            fields[i].setupHook ( cells[currentScroll+i] );
            if(currentScroll+i==previousAddress){
                blocks[i].lightOn(); // zaznaczony blok nad pozostałymi i podświeltony
                blocks[i].draw( RT );
            }
        }

        //Wydrukuj zaznaczoną ramkę na górze



        for ( auto &F : fields ) { //Wydrukuj wartości liczbowe
            F.draw ( RT );
        }
    }
    virtual bool scrolled(sf::Vector2f event, int ammount ){
        int top_limit= cells.size()-display_length;
        currentScroll -= ammount;
        if(currentScroll < 0)
            currentScroll=0;
        else if(currentScroll > top_limit)
            currentScroll=top_limit;
        return true;
    }
    virtual actionCode clicked(sf::Vector2f event ){
        
        for(auto f : fields){
            if(f.check(event))
                break; //jeśli któryś trafiony - przerwij pętlę
                // Żeby uniknąć problemów  z nakładającymi sie elementami i podwójnym wywołaniem
        }
        return handled;
    }
    virtual void lightOn()
    {

    }
    virtual void lightOff()
    {
        for ( auto &block: blocks ) {
            block.lightOff();
        }
    }

};
}
