#pragma once
/** @file */ //doxygen
#include "IWidget.h"
namespace mw
{
class ButtonBar : public IWidget
{
    sf::Mutex button_mutex;
public:
    ButtonBar ( sf::Vector2f windowSize, sf::FloatRect location ) : IWidget ( windowSize, location )
    {
    }
    void addMargin ( float f )
    {
        boundsAddMargin ( f );
    }
    void addButton ( float x, float y, float w, float h, actionCode cc, sf::String text, std::string fontname, sf::Color color )
    {
        std::shared_ptr<Button> btn = std::make_shared<Button> ( x,y,w,h, cc, text, fontname );
        btn->setColor ( color );
        button_mutex.lock();
        this->objects.clickables.push_back ( btn );
        this->objects.drawables.push_back ( btn );
        boundsExpand ( {x,y}, {w,h} );
        button_mutex.unlock();

    }
    void display()
    {
        this->Texture.clear ( mw::ColorScheme::getColor ( "btn-bar-background" ) );
        button_mutex.lock();
        for ( auto d : objects.drawables ) {
            d->draw ( &this->Texture );
        }
        button_mutex.unlock();
        this->Texture.display();
    };

};
}
