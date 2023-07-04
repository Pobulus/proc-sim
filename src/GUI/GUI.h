#pragma once

/** @file */ //doxygen

#include "../UI.h"
#include "MaszynaFrameConfigurator.h"
#include "Console.h"
#include "FileView.h"
#include "AssemblerView.h"


using namespace nlohmann;
namespace mw
{

/**
Klasa GUI odpowiada za wyświetlanie graficznego interfejsu użytkownika.
Pozwala na sterowanie pracą maszyny przy pomocy klawiatury lub myszki
Jej zadaniem jest wyświetlanie i sterowanie widgetami
*/
class GUI : public UI
{
    int animationFrame=0;
    bool lostFocus = false;
    std::shared_ptr<MaszynaFrame> maszynaFrame;
    std::shared_ptr<TitleBar> title;
    std::shared_ptr<Console> console;
    std::shared_ptr<AssemblerView> asmview;
    sf::Cursor hand, arrow, arrow_wait;
    sf::RenderWindow window;
    std::vector<std::shared_ptr<mw::IClickable>> clickables;
    std::vector<std::shared_ptr<mw::IScrollable>> scrollables;
    std::vector<std::shared_ptr<mw::IWidget>> widgets;
    sf::Font mainFont;
	sf::Image logo;
    sf::Thread ren_thread;
    float maszynaFrameSplitter=200.0;
    bool stopped;

    void renderingThread()   //odpowiedzialny za wyświetlanie interfejsu
    {
        window.setActive ( true );
        while ( window.isOpen() ) {
//             window.clear ( sf::Color(2, 84,100) );
            window.clear ( mw::ColorScheme::getColor ( "main-background" ) );

            for ( auto &w : widgets ) {
                w->display();
                w->draw ( &window );
            }
            window.display();
        }
    }
    void inputConfigFilename()
    {
        mw::PopUpFileInput fin ( ".conf" );
        std::string filename = fin.getFilename();
        if ( filename !="" ) {
            loadFromFile ( filename );
        }
    }
    void inputColorSchemeFilename()
    {
        mw::PopUpFileInput fin ( ".style" );
        std::string filename = fin.getFilename();
        if ( filename !="" ) {
            mw::ColorScheme::getInstance().loadFromFile ( filename );
            maszynaFrame->configReload(); // wczytaj konfigurację maszyny na nowo żeby zaktualizować wszystkie elementy
            asmview->unhookAddressRegister();
            asmview->refresh();
            title->setBGColor ( mw::ColorScheme::getColor ( "title-background" ) );
            title->setTextColor ( mw::ColorScheme::getColor ( "title-foreground" ) );
            console->addLine ( "Loaded color scheme "+filename );
            mw::SFX::get().Done();
        }
    }
    void handleAction ( actionCode code )
    {
        switch ( code ) {
        case quit:
            mw::SFX::get().Exit();
            window.close();
            break;
        case config:
            inputConfigFilename();
            asmview->reload();
            break;
        case color:
            inputColorSchemeFilename();
            break;
        case reload:
            maszynaFrame->configReload();
            asmview->reload();
            console->addLine ( "Config Reloaded.", sf::Color ( 0,115, 255 ) );
            break;
        case open_editor:
            console->addLine ( "Opened external editor", sf::Color ( 115,0,255) );
            break;
        case do_tick:
            mw::SFX::get().Tick();
            maszynaFrame->doTick();
            break;
        case do_op:
            mw::SFX::get().Op();
            maszynaFrame->doOperation();
            break;
        case do_prog:
            try {
                if(maszynaFrame->doProgram()){
                    mw::SFX::get().Done();
                    console->addLine ( "Program Finished.", sf::Color::Green );
                }else{
                    mw::PopUpWarning ( "Machine is stopped" ).display();
                }
            } catch ( mw::CPUprogramTimeout exc ) {
                mw::PopUpWarning ( "CPU timed out\nPossible infinite loop" ).display();
            }
            break;
        case reset:
            maszynaFrame->doReset();
            console->addLine ( "Reset" );
            break;
        case open_prog:
            console->addLine ( "loaded program", sf::Color ( 115,0,255 ) );
            break;
        case open_inst:
            console->addLine ( "loaded instruction", sf::Color ( 115,0,255 ) );
            break;
        case assemble:
            //asemblacja wykonana już wczesniej
            console->addLine ( "assemble", sf::Color ( 115,0,255 ) );
            {
                auto memories = maszyna->getMemoryLabels();
                std::string memory;
                if(memories.size()>1) {
                    memory =  mw::PopUpSelectString(memories, "Load to which memory?").getString();
                } else if(memories.size()==1) {
                    memory = memories[0];
                } else {
                    console->addLine ( "Error: CPU has no  memory!", sf::Color::Red );
                    break;
                }
                try{
	                asmview->hookAddressRegister(maszyna->loadProgram ( memory ));
	            }catch(mw::RAMnotEnoughMemory exc){
	            	mw::PopUpWarning(exc.what()).display();
	            	break;
	            }
	            console->addLine ( "Program loaded" );
            }
            handleAction(reset); //zresetuj maszynę żeby była gotowa do pracy
            break;
        case add_inst:
            console->addLine("operation added to OpSet", sf::Color ( 225,225, 0 ));
            break;
        default:
            break;
        }
    }
    void resizeWidgets ( unsigned int w, unsigned int h )
    {
        for ( auto wid : widgets ) {
            wid->resize ( w, h );
        }
    }

public:
    GUI ( mw::Maszyna& cpu ) : UI ( cpu ), ren_thread ( &GUI::renderingThread, this )
    {
        mw::SFX::get().Greet();
        mw::Editor::open("readme.md"); // otwórz edytor i wyświetl użytkownikowi plik pomocy
        mw::ColorScheme::getInstance().loadFromFile ( "style/classic" );

        window.create ( sf::VideoMode ( 1000, 700 ), "MaszynaW" );
                arrow.loadFromSystem(sf::Cursor::Arrow);
		if(logo.loadFromFile("logo.png")){
			//Ustaw ikonę jeśli logo dostepne
			window.setIcon(logo.getSize().x, logo.getSize().y, logo.getPixelsPtr());
		}
        if(!hand.loadFromSystem(sf::Cursor::Hand))
            hand.loadFromSystem(sf::Cursor::Arrow);
        if(!arrow_wait.loadFromSystem(sf::Cursor::ArrowWait))
            if(!arrow_wait.loadFromSystem(sf::Cursor::Wait))   //fallback
                arrow_wait.loadFromSystem(sf::Cursor::Arrow);
        window.setMouseCursor(arrow_wait);
        window.setActive ( false );
        mainFont.loadFromFile ( "orange_kid.otf" );

        asmview = std::make_shared<AssemblerView> ( sf::Vector2f ( 1000, 700 ), sf::FloatRect ( 0.01,0.11,0.24,0.88 ) );

        title = std::make_shared<TitleBar> ( sf::Vector2f ( 1000, 700 ), sf::FloatRect ( 0.01,0.01,0.24,0.09 ), "MaszynaW" );
        console = std::make_shared<Console> ( sf::Vector2f ( 1000, 700 ), sf::FloatRect ( 0.26,0.81,0.73,0.18 ) );


        std::shared_ptr<ButtonBar> topBar = std::make_shared<ButtonBar> ( sf::Vector2f ( 1000, 700 ), sf::FloatRect ( 0.26,0.01,0.73,0.09 ) );

        //Ta czcionka to ikony dingbat
//         topBar->addButton(0,  0,100,100,load, ";", "PWSmallIcons.ttf", sf::Color(255,255,148));
        topBar->addButton ( 150,0,100,100,reset, "s", "PWSmallIcons.ttf", sf::Color ( 27,156,133 ) );
        topBar->addButton ( 300,0,100,100,do_tick, "L", "PWSmallIcons.ttf", sf::Color ( 249,155,125 ) );
        topBar->addButton ( 400,0,100,100,do_op, "G", "PWSmallIcons.ttf", sf::Color ( 231,97,97 ) );
        topBar->addButton ( 500,0,100,100,do_prog, "I", "PWSmallIcons.ttf", sf::Color ( 176,71,89 ) );
        topBar->addButton ( 700,0,100,100,reload, "<", "PWSmallIcons.ttf", sf::Color ( 0,120,250 ) );
        topBar->addButton ( 800,0,100,100,config, "f", "PWSmallIcons.ttf", sf::Color ( 120,120,250 ) );
        topBar->addButton ( 900,0,100,100,color, L"¿", "PWSmallIcons.ttf", sf::Color ( 169,114,173 ) );

        topBar->addButton ( 1100,0,100,100,quit, "`", "PWSmallIcons.ttf", sf::Color::Red );
        topBar->addMargin ( 30 );

        maszynaFrame = std::make_shared<MaszynaFrame> ( sf::Vector2f ( 1000, 700 ), sf::FloatRect ( 0.26,0.11,0.73,0.68 ) );
        maszynaFrame->setMaszyna ( &cpu );
        maszynaFrame->setFont ( mainFont );
        clickables.push_back ( maszynaFrame );
        clickables.push_back ( topBar );
        clickables.push_back ( asmview );

        widgets.push_back ( maszynaFrame );
        widgets.push_back ( topBar );
        widgets.push_back ( title );
        widgets.push_back ( console );
        widgets.push_back ( asmview );

        scrollables.push_back(console);
        scrollables.push_back(maszynaFrame);
        scrollables.push_back(asmview);
        console->addLine("Maszyna W initialised.", sf::Color::Green);
    }

    int run()
    {
        ren_thread.launch();
        while ( window.isOpen() ) {
            sf::Event event;


            while ( window.pollEvent ( event ) ) {
                if ( event.type==sf::Event::Closed ) {
                    window.close();
                } else if ( event.type==sf::Event::Resized ) {
                    //wyśrodkuj w okienku
                    resizeWidgets ( event.size.width, event.size.height );

                } else if ( event.type==sf::Event::GainedFocus) {
                    if(lostFocus) {
                        lostFocus=false;
                        asmview->reload();
                        //wczytaj ponownie plik bo najpewniej został zmieniony
                    }
                } else if ( event.type==sf::Event::LostFocus) {
                    lostFocus = true;
                } else if ( event.type==sf::Event::MouseWheelMoved ) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition ( window );
                    auto mousePosInWin = window.mapPixelToCoords ( mousePos );
                    for ( auto clk : scrollables ) {
                        clk->scroll ( mousePosInWin, event.mouseWheel.delta  );
                    }
                } else if ( event.type==sf::Event::MouseButtonPressed ) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition ( window );
                    auto mousePosInWin = window.mapPixelToCoords ( mousePos );
                    window.setMouseCursor(hand);
                    for ( auto clk : clickables ) {
                        handleAction ( clk->check ( mousePosInWin ) );
                    }
                    window.setMouseCursor(arrow);
                } else if ( event.type==sf::Event::KeyPressed ) {
                    if ( event.key.code == sf::Keyboard::Right ) {
                        maszynaFrame->moveView ( sf::Vector2f ( 10,0 ) );
                    } else if ( event.key.code == sf::Keyboard::Left ) {
                        maszynaFrame->moveView ( sf::Vector2f ( -10,0 ) );
                    } else if ( event.key.code == sf::Keyboard::Down ) {
                        maszynaFrame->moveView ( sf::Vector2f ( 0,10 ) );
                    } else if ( event.key.code == sf::Keyboard::Up ) {
                        maszynaFrame->moveView ( sf::Vector2f ( 0,-10 ) );
                    } else if ( event.key.code == sf::Keyboard::Equal ) {
                        maszynaFrame->zoomView ( 0.8f );
                    } else if ( event.key.code == sf::Keyboard::Dash ) {
                        maszynaFrame->zoomView ( 1.25f );

                    }
                } else if ( event.type==sf::Event::KeyReleased ) {
                    if ( event.key.code == sf::Keyboard::O ) {
                        handleAction ( do_op );
                    } else if ( event.key.code == sf::Keyboard::Num0) {
                      //przywróć normalne przybliżenie
                      maszynaFrame->resetZoom();
                    } else if ( event.key.code == sf::Keyboard::B) {
                        maszynaFrame->showBounds();
                    } else if ( event.key.code == sf::Keyboard::I ) {
                        handleAction ( do_tick );
                    } else if ( event.key.code == sf::Keyboard::P ) {
                        handleAction ( do_prog );
                    } else if ( event.key.code == sf::Keyboard::C ) {
                        console->clear();
                    } else if ( event.key.code == sf::Keyboard::L ) {
                        handleAction ( assemble );
                    } else if ( event.key.code == sf::Keyboard::R ) {
                        handleAction ( reset );
                    } else if ( event.key.code == sf::Keyboard::F6 ) {
                        handleAction ( config );
                    } else if ( event.key.code == sf::Keyboard::F5 ) {
                        handleAction ( reload );
                    }
                }
            }

        }
        ren_thread.wait();
        return 0;
    }

    virtual int loadFromFile ( const std::string &filename )
    {

        window.setMouseCursor(arrow_wait);
        asmview->unhookAddressRegister();
        std::string name;
        try {
             name = maszynaFrame->loadFromFile ( filename ) ;
        } catch ( mw::configCPUFatalError exc ) {

            console->addLine ( filename+":"+exc.what(), sf::Color::Red );
            console->addLine ( "restoring previous config...", sf::Color ( 255, 155, 0 ) );
            maszynaFrame->configReload(); // wczytaj wcześniejszą konfigurację
            name = "!restoredPrevious";
        }
        title->setText (name);
        resizeWidgets ( window.getSize().x, window.getSize().y ); //najpewniej zmienia się rozmiar przyajmniej kilku z nich
        mw::SFX::get().Done();
        console->addLine ( filename+" loaded successfully.", sf::Color::Green );
        window.setMouseCursor(arrow);
        return 0;

    }

};

}
