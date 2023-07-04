#pragma once
/** @file */ //doxygen
#include "PopUpWarning.h"
#include "../Maszyna.h"

namespace mw
{

class MaszynaFrame : public IWidget
{


    class Configurator
    {
        sf::Font* font;
        MaszynaFrame * mf;
        std::vector<std::string> usedConfigFiles;
        std::map<std::string, sf::Vector2f> nodes;
        void checkRequired ( json &list, const std::vector<std::string> &required );
        void addBlock ( json block, float sectX=0, float sectY=0 );
        void addLabel ( json label, float sectX=0, float sectY=0 );
        void addFreeNode ( json fn,    float sectX=0, float sectY=0 );
        void addField ( json field, float sectX=0, float sectY=0 );
        void addLink ( json link,  float sectX=0, float sectY=0 );
        void addList ( json list,  float sectX=0, float sectY=0 );
        void addObjects ( json &objList,json &sectionMoves, float sectX, float sectY );
        int loadFromJSON ( const std::string &filename, json &sectionMoves );
        void clear();
    public:
        Configurator ( );
        static std::string loadFromFile ( std::string filename, MaszynaFrame &MF );
    };

    std::string currentConfig; //obecna konfiguracja ramki maszyny przechowywana wewnątrz jej obiektu






protected:
    bool stopped;
    bool drag;
    sf::Vector2f dragViewStart;
    sf::Vector2i dragMouseStart;


    mw::Maszyna* maszyna;
    sf::Mutex mutex;

    void lightsOut()
    {
        for ( auto v : objects.lightables ) {
            for ( auto l : v.second ) {
                l->lightOff();
            }
        }
    }

    void lightUp ( std::string sig )
    {
        for ( auto l : objects.lightables[sig] ) {
            l->lightOn();
        }
    }

public:
    MaszynaFrame ( sf::Vector2f windowSize, sf::FloatRect location ) : IWidget ( windowSize, location )
    {
        stopped = false;
        drag = false;
        maszyna = new mw::Maszyna();
    }

    void doTick()
    {
        if ( !stopped ) {
            try {
                auto stat = maszyna->tick();
                update ( stat );
            } catch(mw::RuntimeException exc) {
                mw::PopUpWarning ( exc.what() ).display();
                stopped = true;
                update({{"stop"}});
            }
        } else {
            mw::PopUpWarning ( "Machine is stopped" ).display();
        }
    }
    void doOperation()
    {
        if ( !stopped ) {
            try {
                auto stat = maszyna->op();
                update ( stat );
            } catch(mw::RuntimeException exc) {
                mw::PopUpWarning ( exc.what() ).display();
                stopped = true;
                update({{"stop"}});
            }
        } else {
            mw::PopUpWarning ( "Machine is stopped" ).display();
        }
    }
    bool doProgram()
    {
        int timeout = 1000;
        if(!stopped) {
            while ( !stopped ) {
                doOperation();
                if ( ! ( timeout-- ) ) { //jeśli wykonano więcej operacji niż limit zezwala
                    throw mw::CPUprogramTimeout();
                }
            }
            return true; // Program zakończony powodzeniem
        } else {
            mw::PopUpWarning ( "Machine is stopped" ).display();
            return false; // Program nie wykonał się ponieważ maszyna jest zatrzymana
        }


    }
    void doReset()
    {
        mutex.lock();
        maszyna->reset();
        mutex.unlock();
        stopped = false;
        lightsOut();
        for(auto  scrl : objects.scrollables) {
            scrl->setScroll(0);
        }
    }

    void configReload()
    {
        try {
            loadFromFile ( currentConfig );
        } catch ( mw::configCPUFatalError exc ) {
            mw::PopUpWarning(exc.what()).display();
        }
    }

    void update ( mw::status stat ) //Ustawia podświetlane elementy itp
    {
        lightsOut();
        for ( auto s : stat.activeSignals ) {
            lightUp ( s );
            if ( s=="stop" ) {
                stopped = true;

            }
        }
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
        if(ammount > 0) {
            zoomView(0.8);
        } else {
            zoomView(1.25);
        }
        return true;

    };

    actionCode clicked ( sf::Vector2f event )
    {
        auto mf = Frame.getGlobalBounds();
        sf::Vector2f viewPos = Texture.mapPixelToCoords ( {
            ( int ) ( event.x-mf.left ),
            ( int ) ( event.y-mf.top ) } );

        for ( auto clk : objects.clickables ) {
            auto res = clk->check ( viewPos );

            if ( res != empty) {
                return res;
            }
        }
        //rozpocznij przeciąganie, zapisz początek przeciągania
        drag = true;
        dragViewStart = View.getCenter();
        dragMouseStart = sf::Mouse::getPosition();
        return empty;
    }


    void setMaszyna ( mw::Maszyna* M )
    {
        maszyna = M;
    }
    std::string loadFromFile ( const std::string &filename )
    {
        return Configurator::loadFromFile ( filename, *this );
    }

    virtual void display()   //Uaktualnia teksturę
    {
        mutex.lock();
        if(sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            if(drag) {
                sf::Vector2i displacement = sf::Mouse::getPosition() - dragMouseStart;
                View.setCenter({dragViewStart.x - displacement.x, dragViewStart.y - displacement.y});
                this->Texture.setView ( this->View );
            }
        } else {
            //puszczono przycisk, koniec przeciągania
            drag = false;
        }
        this->Texture.clear ( mw::ColorScheme::getColor ( "maszyna-background" ) );
        for ( auto object : objects.drawables ) {
            object->draw ( &this->Texture );
        }
        mutex.unlock();
        this->Texture.display();

    }

};
}
