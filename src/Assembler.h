#pragma once
/** @file */ //doxygen
#include "OpSet.h"

namespace mw
{
struct codeLine {
    std::string label;
    std::string operation;
    std::string argument;
};
/**
 * Klasa Assemblera. \n 
 * Metoda assemble() służy do kompilowania kodu z plików .prg do kodu maszynowego, zapisanego w pliku 'comp'. \n 
 * Metoda parseToLines() zwraca wektor struktur typu codeLine dla łatwego wyświetlania efektu asemblacji
 */
class Assembler
{
    mw::OpSet* opSet;
    std::regex number, character, empty;

public:
    Assembler()
    {
        opSet = &mw::OpSet::getInstance();

        number = std::regex("\\s*[0-9-][0-9]*\\s*");
        character= std::regex("\\s*\'(.)\'\\s*");
        empty = std::regex("\\s*");
    }
    int getInstructionCode ( std::string &name )
    {
        return opSet->getInstruction ( toLower ( name ) ).code << opSet->getBits().data; //przesuń bitowo o bity danych
    }

    std::vector<codeLine> parseToLines ( std::string filename )
    {
        using namespace std;
        string input_file_name=filename;
        ifstream  inputfile ( input_file_name );

        stringstream input;
        input << inputfile.rdbuf();
        input = removeComments(input);
        vector<codeLine> result;
        vector<std::string> labels;

        if ( input ) {
            int lc = 0;
            codeLine line;
            string token;
            while ( input >> token ) {
                int pos = token.find ( ":" ); //pozycja dwukropka
                if ( pos!=string::npos ) { //token to etykieta

                    line.label = token;
                    labels.push_back(token.substr ( 0, pos ));
                } else {
                    mw::instruction inst;
                    try {
                        inst = opSet->getInstruction ( toLower ( token ) );
                        line.operation = token;
                    } catch ( mw::OpSetNotDefined exc ) {
                        std::cout << exc.what() <<std::endl; //nierozpoznana instrukcja -> wpisz błąd w tej linijce
                        /**w przypadku napotkania błędu, w wracanej linijce etykieta ustawiana jest na 'Error'*/
                        line.label = "Error";
                        std::string restOfThisLine;
                        getline( input, restOfThisLine);
                        line.operation = exc.what()+"\n            "+token+restOfThisLine;
                        inst = {0,0};
                    }
                    string attr;
                    if ( inst.attributes == 0 ) {
                        attr = "";
                    } else {
                        if ( ! ( input >> attr ) ) {
                            cerr << "Attribute error on "<<lc<<endl;
                            attr = "0";
                        }
                    }

                    line.argument = attr;
                    result.push_back ( line );
                    line.label="";

                    ++lc; //inkrementuj licznik linii
                    for ( int i = 0; i< inst.attributes-1; i++ ) {
                        if ( ! ( input >> attr ) ) {
                            cerr << "Attribute error on "<<lc<<endl;
                            attr = "0";
                        }
                        line.operation = "rst";
                        line.argument = attr;
                        result.push_back ( line );
                        line.label="";

                        // jeśli więcej niż jeden argument to kolejne przechowywane jako stałe
                        ++lc;
                    }

                }
            }
            //sprawdź czy argumenty są valid

            for(auto &l : result){
                //jeśli nie jest pusty, liczbą lub znakiem
                if(l.argument != "" and !std::regex_match(l.argument, number) and !regex_match(l.argument, character)){
                    // i nie jest znaną etykietą
                    if(std::find(labels.begin(), labels.end(), l.argument) == labels.end()){
                        l.label = "Error";
                        std::string errorMessage = "\nUnknown Label "+l.argument+"\n            "+l.operation+"   "+l.argument;
                        l.operation = errorMessage;
                        l.argument = "";
                        //wpisz jako błąd
                    }
                }

            }


        }else{
            return {{"Error", filename+"\n", "couldn't be opened"}};
        }

        return result;

    }
    void assemble ( std::string filename )
    {
        using namespace std;
        string input_file_name=filename;
        string output_file_name="comp";
        // ifstream  input ( input_file_name );

        ifstream  inputfile ( input_file_name );

        stringstream input;
        input << inputfile.rdbuf();
        input = removeComments(input);

        ofstream  output ( output_file_name );
        vector<pair<string, string>> lines; // linia = rozkaz i atrybut, jeśli atrybutów jest więcej to traktowane są jako rst
        map<string, int> labels;

        if ( input ) {
            int lc = 0;

            string token;
            while ( input >> token ) {
                int pos = token.find ( ":" ); //pozycja dwukropka
                if ( pos!=string::npos ) { //token to etykieta

                    labels[token.substr ( 0, pos )]=lc;
                } else {
                    mw::instruction inst;
                    try {
                        inst = opSet->getInstruction ( toLower ( token ) );
                    } catch ( mw::OpSetNotDefined exc ) {
                        std::cout << exc.what() <<std::endl; //nierozpoznana instrukcja -> koniec asemblacji
                        return;
                    }
                    string attr;
                    if ( inst.attributes == 0 ) {
                        attr = "0";
                    } else {
                        if ( ! ( input >> attr ) ) {
                            cerr << "Attribute error on "<<lc<<endl;
                            attr = "0";
                        }
                    }
                    lines.push_back ( {token, attr} );

                    ++lc; //inkrementuj licznik linii
                    for ( int i = 0; i< inst.attributes-1; i++ ) {
                        if ( ! ( input >> attr ) ) {
                            cerr << "Attribute error on "<<lc<<endl;
                            attr = "0";
                        }
                        lines.push_back ( {"rst", attr} );
                        // jeśli więcej niż jeden argument to kolejne przechowywane jako stałe
                        ++lc;
                    }

                }
            }



            std::smatch schar;
            for ( auto &line : lines ) {
                if ( labels.count ( line.second ) ) { // wstaw numery linii w miejse etykiet
                    line.second = to_string ( labels[line.second] );
                }else if(regex_search(line.second, schar, character)){
                    //konwersja ze znaku char w '' na jego wartość w int
                    line.second = std::to_string((int)schar.str(1)[0]);
                }
            }
            if ( output ) { // wypisz do pliku obliczone wartości
                for ( auto line : lines ) {

                    output << ( getInstructionCode ( line.first ) )+stoi ( line.second ) << " ";
                }
                output << endl;
            }



        }

    }

};
}
