#pragma once
/** @file */ //doxygen
#include <regex>
#include "PopUpWarning.h"
//TODO kompilacja warunkowa z wersją dla Windowsa
namespace mw
{
class Editor
{
    std::regex safetycheck;
    Editor()
    {
        //sprawdza czy podany string jest nazwą pliku i nie zawiera
        //niebezpiecznych znaków które mogłyby wywołać niporządany proces w konsoli
        safetycheck = std::regex("\\w[\\w.\\/]+[.][\\w]+");
    }
public:
    Editor ( const Editor&original ) = delete;
    Editor& operator= ( const OpSet&original ) = delete;
    static void open(std::string filename, std::string editorname, int linenum);

};
}
void mw::Editor::open(std::string filename, std::string editorname = "featherpad", int linenum=0)
{
    static mw::Editor singleton;

        //wywołaj edytor tekstowy w tle
        if(std::regex_match(filename, singleton.safetycheck)){
            std::cout << "opening external editor: "+editorname+" "+filename<<std::endl;

            std::string command = editorname+ +" +"+std::to_string(linenum)+" "+ filename+ " &"; //& otwiera proces w tle
            //wersja dla Windowsa
            //std::string command = "start \B "singleton.editor+ " "+ filename; //& otwiera proces w tle
            system(command.c_str()); //polecenie dla linuxa,
            return;
        }
        mw::PopUpWarning("invalid filename").display();


}
