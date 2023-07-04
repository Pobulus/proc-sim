#pragma once
/** @file */ //doxygen
#include "IWidget.h"
#include <sstream>
namespace mw
{
class Console : public IWidget
{
    std::vector<sf::Text> lines;
    sf::Font font;
    sf::Color bg;
    float lastLineY=0;
    int lineCount = 5;
    sf::Mutex lines_mutex;

    const float lineSpace = 10;
    const float lineHeight =90;
public:
    Console ( sf::Vector2f windowSize, sf::FloatRect location, std::string fontname="monospace.otf" ) : IWidget ( windowSize, location )
    {
        font.loadFromFile ( fontname );
        bg = sf::Color::Black;
    }
    void addLine ( std::string text, sf::Color color = sf::Color::White ){
        std::stringstream ss(text);
        std::string sub;
        //Jeśli pojawi się znak nowej linii trzeba dodać 2 linijki
        while(std::getline(ss, sub)){
            addSingleLine(sf::String(sub), color);
        }
    }
    void addSingleLine ( sf::String text, sf::Color color = sf::Color::White )
    {
        sf::Text line;
        line.setCharacterSize ( lineHeight );
        line.setFillColor ( color );
        line.setFont ( font );
        line.setString ( text );
        line.setPosition ( {0, lastLineY} );
        
        auto gb = line.getGlobalBounds();

        lastLineY += lineHeight+lineSpace;

        currentScroll = 0;
        Bounds = {0, lastLineY-lineHeight* ( lineCount+0.5f ), 4200, lineHeight* ( lineCount+1 ) };
        lines_mutex.lock();
        lines.push_back ( line );
        lines_mutex.unlock();

    }
    bool scrolled( sf::Vector2f event, int ammount ){
        currentScroll+= ammount;
        float top = lastLineY-lineHeight* ( currentScroll+(lineCount+0.5f) );
        float height = lineHeight* ( lineCount+1 );
        if( top > 0 and top <= lastLineY-height+(lineHeight*0.5f))
            Bounds = {0, top, 4200,  height};
        else
            currentScroll -= ammount;
        return true;
    }


    void setBGColor ( sf::Color clr )
    {
        bg = clr;
    }
    void setTextColor ( sf::Color clr )
    {
        for ( auto &l : lines )
            l.setFillColor ( clr );
    }
    void display()
    {
        resetZoom();
        Texture.clear ( bg );
        lines_mutex.lock();
        for ( auto &l : lines ) {
            Texture.draw ( l );
        }
        lines_mutex.unlock();
        Texture.display();
    }
    void clear()
    {
        Bounds = {0,0,0,0};
        lines_mutex.lock();
        lines.clear();
        lines_mutex.unlock();
        lastLineY = 0;
    }

};
}
