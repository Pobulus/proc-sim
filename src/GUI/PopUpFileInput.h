#pragma once

/** @file */ //doxygen

#include "PopUpSelectString.h"
#include <filesystem>
namespace mw
{
class PopUpFileInput : public PopUpSelectString
{

protected:
    void findFilesWithExtension ( std::string extension )
    {
        namespace fs=std::filesystem;
        for ( auto &f : fs::directory_iterator ( fs::current_path() ) ) {
            if ( f.path().extension() == extension ) {
                strings.push_back ( f.path().filename() );
            }
        }


        extension.erase(0,1); // usuń .
        
        // przeszukaj też w folderze nazwanym jak rozszerzenie
        
        auto config = fs::current_path() /fs::path ( extension );
        if ( fs::is_directory ( config ) ) {
            for ( auto &f : fs::recursive_directory_iterator ( config ) ) {
            	strings.push_back ( fs::relative ( f.path(),  fs::current_path() ) );
            }

        }
    }

public:
    PopUpFileInput ( std::string extension ) : PopUpSelectString ( "Wybierz plik "+extension )
    {
        findFilesWithExtension ( extension );
        addButtons();
        widget->addMargin ( 30 );
    }
    std::string getFilename()
    {
        return getString();

    }
};
}
