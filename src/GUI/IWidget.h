#pragma once
/** @file */ //doxygen
#include "IScrollable.h"
#include "Button.h"

namespace mw
{
       std::string removeChar ( std::string str, char c )
{
    str.erase ( std::remove ( str.begin(), str.end(), c ), str.end() );
    return str;
}
std::string normalizeLength ( std::string in, int len )
{
    int count = len-in.length();
    while ( ( count-- ) > 0 ) {
        in += " ";
    }
    return in;
}
std::string addLineBreaks ( std::string in, int len )
{
    int count = len-in.length();
    auto beg = in.begin();
    auto end = in.end();
    auto i = beg+len;
    while ( i < end ) {
        while ( i > beg ) {
            if ( *i==' ' || *i==',' ) {
                *i = '\n';

                break;
            }
            i--;
        }
        i+=len;
        beg+= len;
    }
    return in;
}


struct Objects {
    std::vector<std::shared_ptr<mw::IDrawable>> drawables;
    std::map<std::string, std::vector<std::shared_ptr<mw::ILightable>>> lightables;
    std::vector<std::shared_ptr<mw::IClickable>> clickables;
    std::vector<std::shared_ptr<mw::IScrollable>> scrollables;
};


class IWidget : public IDrawable, public IScrollable
{
protected:
    sf::FloatRect Bounds;
    sf::RenderTexture Texture;
    sf::Sprite Frame;
    sf::View View;
    sf::FloatRect location; //in fraction of window size
    mw::Objects objects;
    sf::Font* font;
    sf::Vector2f lastSize;
public:
    IWidget ( sf::Vector2f windowSize, sf::FloatRect location ) : location ( location )
    {

        sf::Vector2f size = {location.width*windowSize.x, location.height*windowSize.y};
        Texture.create ( size.x, size.y );
        View.setCenter ( size.x/2, size.y/2 );
        View.setSize ( size.x, size.y );
        Texture.setView ( View );
        Texture.display();
        Frame.setTexture ( Texture.getTexture() );
        Frame.setPosition ( {location.left*windowSize.x, location.top*windowSize.y} );
    }
    virtual ~IWidget() {}
    /** Ustawia czcionkę widgetu
      Należy to zrobić przed dodaniem obiektów z tekstem
    */
    void setFont ( sf::Font &f )
    {
        font = &f;
    }
    /**
    * Poszerza obszar który ma być dopasowany jako widoczny w widgecie przy zmianie rozmiaru
    * Przyjmuje 2 Vector2f: wektor pozycji i wektor rozmiarów obiektu
    */
    void boundsExpand ( sf::Vector2f P, sf::Vector2f S )
    {
        if(P.x < Bounds.left){
          Bounds.width +=Bounds.left - P.x;
          Bounds.left = P.x;
        }
        if(P.y < Bounds.top){
          Bounds.height +=Bounds.top - P.y;
          Bounds.top = P.y;
        }
        Bounds.width= std::max ( Bounds.width, P.x+S.x-Bounds.left );
        Bounds.height= std::max ( Bounds.height, P.y+S.y-Bounds.top );
    }


    /**
    Przywraca przybliżenie do wartości ustawionej automatycznie przy zmianie rozmiaru okna
    */
    void resetZoom()
    {
        resize ( lastSize.x, lastSize.y );
    }
    virtual void resize ( float w, float h )
    {
        lastSize = {w, h};
        View.setSize ( w*location.width, h*location.height );

        //wyśrodkuj w okienku
        float scale = std::max ( Bounds.width/ ( w*location.width ), Bounds.height/ ( h*location.height ) );
        View.zoom ( scale ); //przeskaluj do szerokości
        // View.setCenter ( Bounds.left+ ( Bounds.width/2 ), Bounds.top+ ( Bounds.height/2 ) );
        View.setCenter (  ( Bounds.left+Bounds.width/2 ),  ( Bounds.top+Bounds.height/2 ) );

        Texture.setView ( View );

        Texture.display();

        this->area = Frame.getGlobalBounds();

    }
    virtual void display() =0;
    virtual void draw ( sf::RenderTarget * RT )
    {
        RT->draw ( Frame );
    }
    actionCode clicked ( sf::Vector2f event )
    {
        auto mf = Frame.getGlobalBounds();
        sf::Vector2f viewPos = Texture.mapPixelToCoords ( {
            ( int ) ( event.x-mf.left ),
            ( int ) ( event.y-mf.top ) } );
        for ( auto clk : objects.clickables ) {
            auto res = clk->check ( viewPos );

            if ( res != empty and res != handled ) {
                return res;
            }
        }
        return empty;
    }
    void boundsAddMargin ( float m )
    {
        Bounds.height += m;
        Bounds.width  += m;
        Bounds.left   -= m/2;
        Bounds.top    -= m/2;
    }

    void  moveView ( sf::Vector2f disp )
    {
        this->View.move ( disp );
        this->Texture.setView ( this->View );
    }
    void showBounds(){
      objects.drawables.push_back(std::make_shared<mw::Block>(Bounds.left, Bounds.top, Bounds.width, Bounds.height));
    }
    void  zoomView ( float zoom )
    {
        this->View.zoom ( zoom );
        this->Texture.setView ( this->View );
    }

    virtual bool scrolled ( sf::Vector2f event, int ammount ) {
        auto mf = Frame.getGlobalBounds();
        sf::Vector2f viewPos = Texture.mapPixelToCoords ( {
            ( int ) ( event.x-mf.left ),
            ( int ) ( event.y-mf.top ) } );
        for ( auto scrl : objects.scrollables) {
            if(scrl->scroll( viewPos, ammount ))
                return true;
        }
        return false;

    };
};
}
