#pragma once
/** @file */ //doxygen
#include "PopUpFileInput.h"
#include "PopUpOperationInput.h"
#include "../Assembler.h"


namespace mw
{

class AssemblerView : public IWidget
{
    sf::Color bg;
    int autoreload = 500;
    const int autoreload_delay = 500;
    sf::Mutex lines_mutex;
    std::shared_ptr<ButtonBar> barProg, barOp;
    std::shared_ptr<ButtonBar> current;

    std::shared_ptr<FileView> fview;
    sf::Vector2f size_of_asmview;
public:
    AssemblerView ( sf::Vector2f windowSize, sf::FloatRect location, std::string fontname="monospace.otf" ) : IWidget ( windowSize, location )
    {
        size_of_asmview = {windowSize.x*location.width, windowSize.y*location.height};
        Bounds = {0,0,size_of_asmview.x, size_of_asmview.y};
        fview = std::make_shared<FileView> ( size_of_asmview, sf::FloatRect ( 0,0.1,1,0.9 ), fontname );
        objects.scrollables.push_back(fview);

        bg = sf::Color::Black;//nie powinno być widoczne

        //po utworzeniu pusty plik, nie można asemblować ani dodać rozkazu
        //trzeba wczytać plik programu lub rozkazu
        current = std::make_shared<ButtonBar> ( size_of_asmview, sf::FloatRect ( 0,0,1,0.1 ) );
        current->addButton ( 0,  0,100,100,open_prog, "h", "PWSmallIcons.ttf", sf::Color ( 150,150,205 ) );
        current->addButton ( 100,  0,100,100,open_inst, "x", "PWSmallIcons.ttf", sf::Color ( 200,200,255 ) );
        
        barProg = std::make_shared<ButtonBar> ( size_of_asmview, sf::FloatRect ( 0,0,1,0.1 ) );
        barOp = std::make_shared<ButtonBar> ( size_of_asmview, sf::FloatRect ( 0,0,1,0.1 ) );
        barProg->addButton ( 0,  0,100,100,open_prog, "h", "PWSmallIcons.ttf", sf::Color ( 150,205,150 ) );
        barProg->addButton ( 100,  0,100,100,open_inst, "x", "PWSmallIcons.ttf", sf::Color ( 200,200,255 ) );
        barProg->addButton ( 250,  0,100,100,open_editor, "W", "PWSmallIcons.ttf", sf::Color ( 200,200,200 ) );
        barProg->addButton ( 400,0,100,100,assemble, "Y", "PWSmallIcons.ttf", sf::Color ( 255,0,133 ) );

        barOp->addButton ( 0,  0,100,100,open_prog, "h", "PWSmallIcons.ttf", sf::Color ( 150,150,205  ) );
        barOp->addButton ( 100,  0,100,100,open_inst, "x", "PWSmallIcons.ttf", sf::Color ( 200,255,200 ) );
        barOp->addButton ( 250,  0,100,100,open_editor, "W", "PWSmallIcons.ttf", sf::Color ( 200,200,200 ) );
        barOp->addButton ( 400,0,100,100,  add_inst, ";", "PWSmallIcons.ttf", sf::Color ( 255,255,148 ) );

        current->addMargin ( 30 );
        barProg->addMargin ( 30 );
        barOp->addMargin ( 30 );
        refresh();

    }
    void refresh()
    {
        fview->setBGColor ( mw::ColorScheme::getColor ( "file-background" ) );
        fview->setTextColor ( mw::ColorScheme::getColor ( "file-foreground" ) );
        fview->setHighlightColor ( mw::ColorScheme::getColor ( "file-highlight" ) );
    }
    void hookAddressRegister(std::shared_ptr<mw::IElement> A){
        fview->hookAddressRegister(A);
    }
    void unhookAddressRegister(){
        fview->unhookAddressRegister();
    }
    actionCode clicked ( sf::Vector2f event )
    {
        auto mf = Frame.getGlobalBounds();
        sf::Vector2f viewPos = Texture.mapPixelToCoords ( {
            ( int ) ( event.x-mf.left ),
            ( int ) ( event.y-mf.top ) } );
        auto cc = current->check ( viewPos );

        switch ( cc ) {
        case assemble:
            if(!fview->progAssemble())
                cc =  handled;
            break;
        case open_editor:
            fview->openEditor();
            break;
        case add_inst:
            if(!fview->addOperation())
                cc=handled;
            break;
        case open_prog:
        {
            std::string filename = mw::PopUpFileInput ( ".prg" ).getFilename();
            if(filename != ""){
                fview->loadProgram ( filename );
                current = barProg;
            }else
                cc = handled;

        }   break;
        case open_inst:
        {
            std::string filename = mw::PopUpOperationInput ().getFilename();
            if(filename!=""){
                fview->loadOperation ( filename );
                current = barOp;
            }else // nie otwarto pliku, więc nie przekzauj wyżej kodu akcji
                cc = handled;

        }break;
        default:
            break;
        }
        refreshSize(); //Zakualizuj wymiary jeśli mieniono current
        return cc;
    }


    void setHighlightColor ( sf::Color clr )
    {
        fview->setHighlightColor ( clr );
    }
    void setBGColor ( sf::Color clr )
    {
        bg=clr;
        fview->setBGColor ( clr );
    }
    void setTextColor ( sf::Color clr )
    {
        fview->setTextColor ( clr );
    }
    void refreshSize( )
    {
        current->resize ( size_of_asmview.x, size_of_asmview.y);
        fview->resize ( size_of_asmview.x, size_of_asmview.y);
    }
    void resize ( float w, float y )
    {
        size_of_asmview = {this->location.width*w, this->location.height*y};
        refreshSize( );
        this->area = Frame.getGlobalBounds();

    }

    void reload(){
        fview->reload();
    }
    void display()
    {
//         resize();
        //wczytuj ponownie otwarty plik, żeby uwidocznić zmiany wykonane w zewnętrznym edytorze
//         if( (autoreload--) == 0){
//             autoreload = autoreload_delay;
//             fview->reload();
//         }
        Texture.clear ( bg );
        fview->display();
        fview->draw ( &this->Texture );
        current->display();
        current->draw ( &this->Texture );
        this->Texture.display();
    }




};
}
