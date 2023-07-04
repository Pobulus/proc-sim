#include "Maszyna.h"
#include "Assembler.h"
#include "CLI.h"
#include "GUI/GUI.h"
#include <X11/Xlib.h>
#include <nlohmann/json.hpp>
using namespace nlohmann;


int main ( int argc, char **argv )
{
    // Konieczne jest wywołanie tej metody żeby wielowątkowość działała na linuxie
    if ( !XInitThreads() ) {
        std::cerr << "Error: Couldn't start threads" <<std::endl;
        return 1;
    }
    std::cerr << "XInitThreads called!" <<std::endl;
    
    mw::Maszyna W;    
    mw::GUI ui ( W );
    
    if ( argc >1 )
        ui.loadFromFile ( argv[1] );
    else
        ui.loadFromFile ( "greeter.conf"  );

    ui.run();

    return 0;
}
