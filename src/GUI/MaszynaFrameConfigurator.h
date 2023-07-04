#pragma  once
/** @file */ //doxygen
#include "MaszynaFrame.h"
#include "Label.h"
#include "Link.h"
#include "List.h"


#include "../Exceptions.h"
namespace mw
{
mw::MaszynaFrame::Configurator::Configurator()
{

}
std::string mw::MaszynaFrame::Configurator::loadFromFile ( std::string filename, MaszynaFrame &MF )
{
    //niejawny singleton, nie możemy
    static mw::MaszynaFrame::Configurator config;
    std::map<std::string, std::pair<float, float>> sectMov;
    MF.mutex.lock();
    MF.stopped  =  false;
    config.mf = &MF; //tymczasowo zapisz w prywatnej zmiennej wskaźnik do konfigurowanej ramki maszyny
    config.clear();



    json empty = {};
    //najpierw skonfiguruj maszynę
    std::string res;
    try {
        res =MF.maszyna->configFromFile ( filename );
    } catch ( mw::CPUconstructionError exc ) {
        mw::PopUpWarning ( exc.what() ).display();
        MF.mutex.unlock(); //zwolnij semafor przed rzuceniem wyjątku
        throw mw::configCPUFatalError();
    } catch(json::exception exc){
        mw::PopUpWarning ( exc.what() ).display();
        MF.mutex.unlock();
        throw mw::configCPUFatalError();
    }

    try {
        config.loadFromJSON ( filename, empty );

    } catch ( json::exception exc ) {
        std::cout << exc.what() <<std::endl;
    } catch ( std::exception exc ) {
        std::cout << exc.what() <<std::endl;
    }


    MF.currentConfig = filename;

    //dodaj odstęp od krawdędzi
    MF.boundsAddMargin ( 100.f );

    MF.mutex.unlock();

    return res;
}



void checkRequired ( json &list, const std::vector<std::string> &required )
{
    for ( auto req : required ) {
        if ( !list.contains ( req ) ) {
            throw mw::configMissingAtribute ( req, list.dump() );
        }
    }
}
int mw::MaszynaFrame::Configurator::loadFromJSON ( const std::string &filename, json &sectionMoves )
{
    json config = openJSON(filename);
    usedConfigFiles.push_back ( filename );

    if ( config.contains ( "include" ) ) {
        auto include = config["include"];
        if ( !include.contains ( "file" ) ) {
            throw mw::configMissingAtribute ( "File" );
        }

        // Jeśli już dołączono taki plik, nastąpiło zapętlenie załączeń
        if ( std::find ( usedConfigFiles.begin(), usedConfigFiles.end(), include["file"] ) !=usedConfigFiles.end() ) {
            throw mw::configRecursiveInclude ( include["file"] );
        }
        loadFromJSON ( include["file"], include["sections"] );
    }
    try{
      addObjects ( config["objects"], sectionMoves, 0, 0 );
    }catch(mw::configMissingAtribute exc){
      mw::PopUpWarning(exc.what()).display();
    }
    std::cout << filename<<": loaded successfully"<<std::endl;
    return 0;
}
void mw::MaszynaFrame::Configurator::addObjects ( json &objList,json &sectionMoves, float sectX, float sectY )
{
    std::vector<json> links;

    for ( auto &obj : objList ) {
        // std::cout << obj["type"]<<":"<<obj["name"] <<std::endl;


        if ( obj["type"]=="SECTION" ) {
            std::string name = obj["name"];

            float X = sectX;
            float Y = sectY;
            if ( obj.contains ( "pos" ) ) {
                X += ( float ) obj["pos"][0];
                Y += ( float ) obj["pos"][1];
            }
            //Wprowadź przesunięcie z pliku importującego, jeśli występuje
            if ( sectionMoves[name]!=nullptr ) {
                X += ( float ) sectionMoves[name][0];
                Y += ( float ) sectionMoves[name][1];
            }
            addObjects ( obj["objects"], sectionMoves, X, Y );
        } else if ( obj["type"]=="BLOCK" ) {
            addBlock ( obj, sectX, sectY );
        } else if ( obj["type"]=="LABEL" ) {
            addLabel ( obj, sectX, sectY );
        } else if ( obj["type"]=="LINK" ) {
            links.push_back ( obj );
        } else if ( obj["type"]=="LIST" ) {
            addList ( obj, sectX, sectY );
        } else if ( obj["type"]=="FIELD" ) {
            addField ( obj, sectX, sectY );
        } else if ( obj["type"]=="NODE" ) {
            addFreeNode ( obj, sectX, sectY );
        } else {
            std::cerr << "Unknown object type!\n"<<obj<<std::endl;
        }

    }
    //Połącz węzły dopiero po utworzeniu wszystkich pozostałych obiektów w sekcji
    for ( auto &link : links ) {
        addLink ( link, sectX, sectY );
    }
}


void MaszynaFrame::Configurator::addList ( json list, float sectX, float sectY )
{
    mw::checkRequired ( list, {"pos", "length", "hook", "selector", "size", "name"} );

    auto output = std::make_shared<mw::List> (
                      ( float ) list["pos"][0]+sectX,
                      ( float ) list["pos"][1]+sectY,
                      ( int ) list["length"] );

    output->hookCells ( list["hook"], mf->maszyna->getElements(), ( int ) list["size"][0], ( int ) list["size"][1] );
    output->setupAddress ( ( * ( mf->maszyna->getElements() ) ) [list["selector"]] );
    output->setFont ( *mf->font );
    for ( std::string signal : list["signals"] ) {
        mf->objects.lightables[signal].push_back ( output );
    }

    mf->objects.drawables.push_back ( output );
    mf->objects.clickables.push_back ( output );
    mf->objects.scrollables.push_back ( output );

}

void MaszynaFrame::Configurator::addLabel ( json label, float sectX, float sectY )
{
    mw::checkRequired ( label, {"pos", "name"} );

    std::string text = "";
    if ( label.contains ( "text" ) ) {
        text = label["text"];
    }
    //Utwórz blok o zadanych parametrach
    auto output = std::make_shared<mw::Label> (
                      ( float ) ( label["pos"][0] )+sectX,
                      ( float ) ( label["pos"][1] )+sectY, text );


    output->setFont ( *mf->font );
    if(label["font"]!=nullptr){ //opcjonalnie można zmienić czcionkę tekstu
      sf::Font font;
      if(font.loadFromFile(label["font"]))
        std::cout << "font set to " << label["font"]<<std::endl;
        output->setFont(font);
    }


    std::string name = label["name"];
    for ( auto node : label["nodes"].items() ) {
        nodes[name+"@"+node.key()] = output->getNode ( ( float ) node.value() [0], ( float ) node.value() [1] );
    }
    //Dodaj do sygnałów
    for ( std::string signal : label["signals"] ) {
        mf->objects.lightables[signal].push_back ( output );
    }

    //poszerz widok o wymiary etykiety
    mf->boundsExpand ( {( float ) ( label["pos"][0] )+sectX,( float ) ( label["pos"][1] )+sectY},  output->getSize());
    mf->objects.drawables.push_back ( output );

}


void MaszynaFrame::Configurator::addBlock ( json block, float sectX, float sectY )
{

    //Position i size są wymagane, jeśli ich brak -> rzuć wyjątek
    mw::checkRequired ( block, {"pos", "size", "name"} );

    //Utwórz blok o zadanych parametrach
    auto output = std::make_shared<mw::Block> ( ( float ) ( block["pos"][0] )+sectX, ( float ) ( block["pos"][1] )+sectY, block["size"][0], block["size"][1] );

    //poszerz granice maszyny o wymiary bloku
    mf->boundsExpand ( { ( float ) ( block["pos"][0] )+sectX, ( float ) ( block["pos"][1] )+sectY}, {block["size"][0], block["size"][1]} );

    if ( block["color"]!=nullptr ) {
        auto color = block["color"];
        output->setColor ( sf::Color ( color[0], color[1], color[2] ) );
    }
    std::string name = block["name"];
    for ( auto node : block["nodes"].items() ) {
        nodes[name+"#"+node.key()] = output->getNode ( node.value() [0], node.value() [1] );
    }
    //Dodaj do sygnałów
    for ( std::string signal : block["signals"] ) {
        mf->objects.lightables[signal].push_back ( output );
    }
    mf->objects.drawables.push_back ( output );
}

void MaszynaFrame::Configurator::addField ( json field, float sectX, float sectY )
{
    //Position i size są wymagane, jeśli ich brak -> rzuć wyjątek

    mw::checkRequired ( field, {"pos"} );

    auto output = std::make_shared<mw::Field> ( ( float ) ( field["pos"][0] )+sectX, ( float ) ( field["pos"][1] )+sectY );
    output->setFont ( *mf->font );
    std::string name = field["name"];
    if ( !field.contains ( "hook" ) ) {
        std::cerr << "WARNING: UNHOOKED FIELD"<<name<<std::endl;
    } else {
        std::string hook = field["hook"];
        if ( mf->maszyna->getElements()->count ( hook ) ) {
            output->setupHook ( ( * ( mf->maszyna->getElements() ) ) [hook] );
        } else {
            throw mw::configUnknownElement ( hook );
        }
    }
    mf->objects.clickables.push_back ( output );
    mf->objects.scrollables.push_back ( output );
    mf->objects.drawables.push_back ( output );
}
void MaszynaFrame::Configurator::addLink ( json link, float sectX, float sectY )
{
    mw::checkRequired ( link, {"join"} );
    bool hor = false;
    if ( link["horizontal"] ) {
        hor = true;
    }

    auto join = link["join"];
    std::string node1=join[0], node2 = join[1];
    //Sprawdż czy rządane węzły istnieją
    //Jeśli nie istnieją rzuć wyjątek
    if ( ! nodes.count ( node1 ) ) {
        throw mw::configUnknownNode ( node1 );
    }
    if ( ! nodes.count ( node2 ) ) {
        throw mw::configUnknownNode ( node2 );
    }

    auto output =  std::make_shared<mw::Link> ( nodes[node1], nodes[node2], hor, link["style"] );
    for ( std::string signal : link["signals"] ) {
        mf->objects.lightables[signal].push_back ( output );
    }
    mf->objects.drawables.push_back ( output );
}

void MaszynaFrame::Configurator::addFreeNode ( json fn, float sectX, float sectY )
{
    //Position i size są wymagane, jeśli ich brak -> rzuć wyjątek
    mw::checkRequired ( fn, {"pos","name"} );

    std::string fn_name = fn["name"];

    nodes["$"+fn_name] = { ( float ) ( fn["pos"][0] )+sectX, ( float ) ( fn["pos"][1] )+sectY};
}

void MaszynaFrame::Configurator::clear()
{
    nodes.clear();
    mf->objects = {};

    mf->maszyna->configClear();

    mf->Bounds = {0.f,0.f,0.f,0.f};
    usedConfigFiles.clear();
}



}
