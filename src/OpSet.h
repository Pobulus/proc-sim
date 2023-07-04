#pragma once
/** @file */ //doxygen
#include <iostream>
#include <fstream>
#include <algorithm>
#include <regex>
#include "opLine.h"
#include "opCond.h"
#include "opCommon.h"


namespace mw
{
/**
 * Usuwa komentarze w stylu //*komentarz*
 * ze sturmienia plików i zwraca strumień wolny od komentarzy
 */
std::stringstream removeComments ( std::stringstream &buf )
{
    std::stringstream output;
    std::regex comment ( "([^\\/]*)\\/\\/.*" );
    std::string str;
    std::smatch scom;
    while ( getline ( buf, str ) ) {

        if ( !std::regex_search ( str, scom, comment ) ) {

            output << str;
        } else {

            output << scom.str ( 1 );
        }
        output << "\n";//dodaj ponownie newline
    }
    return output;
}

/**
 * Konwertuje wielkie litery w stringu na małe
 */
std::string toLower ( std::string str )
{
    std::string out;
    for ( char character : str ) {
        if ( character >= 'A' and character <='Z' ) {
            character = character- ( 'A'-'a' );
        }
        out += character;
    }
    return out;
}
/**
 * Sturktura przechowująca szerkość bitów dla zapisu kodu rozkazu i adresu danych
 */
struct bit_widths {
    unsigned short int code;
    unsigned short int data;
};
/**
 * Struktura przechowująca kod rozkazu, liczbę argumentów oraz plik w którym został zdefiniowany
 */
struct instruction {
    int code;
    int attributes;
    std::string fileOfOrigin;
};
/**
 * Klasa przechowująca zestaw rozkazów. <Singleton>
 *  
 */
class OpSet
{
    std::set<std::string> signal_names;

    std::map<std::string, instruction> instructions;
    mw::bit_widths bits;
    short int max_op_count=0, count=0;

    std::regex reg_opname, reg_opline, reg_cond, reg_cond_begin, reg_args, reg_comm, reg_line;
    std::shared_ptr<mw::opCommon> root;
    std::shared_ptr<mw::opNode> current_node=nullptr;


    std::shared_ptr<mw::opNode> parseLines ( std::stringstream &buff )
    {
        std::string tmp;
        std::shared_ptr<mw::opNode> output=nullptr;
        std::shared_ptr<mw::opLine> last;

        while ( std::getline ( buff, tmp, ';' ) ) {
            // zignoruj komentarze, ROZKAZ i Argumenty

            if ( std::regex_search ( tmp, reg_opline ) ) {
                if ( std::regex_search ( tmp, reg_cond_begin ) ) {

                    break;//zakończ wczytywanie linii jeśli trafisz na warunek
                } else {

                    if ( output ==nullptr ) {

                        last = std::make_shared<mw::opLine> ( tmp );
                        output = last;
                    } else {
                        auto x = std::make_shared<mw::opLine> ( tmp );
                        last->addNode ( x );
                        last = x;
                    }

                }
            }
        }
        std::string content = buff.str();

        std::smatch condition;
        if ( std::regex_search ( content, condition, reg_cond ) ) {

            auto cond_node = std::make_shared<mw::opCond> ( condition.str ( 1 ) );

            std::stringstream truestream ( condition.str ( 4 ) );
            cond_node->addNodeTrue ( parseLines ( truestream ) );

            std::stringstream falsestream ( condition.str ( 5 ) );
            cond_node->addNodeFalse ( parseLines ( falsestream ) );

            if ( output == nullptr ) {
                output = cond_node;
            } else {
                last->addNode ( cond_node );
            }
        }

        return output;
    }

    OpSet()
    {
        // inicjalizacja wyrażeń regularnych
        reg_opname =  std::regex ( "ROZKAZ\\s+([\\w]+)\\s*", std::regex_constants::ECMAScript|std::regex::icase );
        reg_comm =  std::regex ( "\\/\\/.*", std::regex_constants::ECMAScript );
        reg_args =  std::regex ( "Argumenty\\s*([0-9])+", std::regex_constants::ECMAScript|std::regex::icase );

        //JEZELI\s*([\w]+)\s*TO\s*(@[\w]+)\s*GDY NIE\s*(@[\w]*)\s*;[\S\s]*\2([\w\s@;]*)KONIEC;[\S\s]*\3([\s\w;@]*);

        reg_cond =  std::regex ( "JEZELI\\s*([\\w]+)\\s*TO\\s*(@[\\w]+)\\s*GDY NIE\\s*(@[\\w]*)\\s*;[\\S\\s]*\\2([\\w\\s@;]*)KONIEC;[\\S\\s]*\\3([\\s\\w;@]*);", std::regex_constants::ECMAScript|std::regex::icase  );

        reg_cond_begin   =  std::regex ( "JEZELI", std::regex_constants::ECMAScript );
        reg_opline = std::regex ( "^(?!\\s*ROZKAZ|\\s*\\/\\/|\\s*Argumenty.*)\\s*\\w[\\w\\s]*",std::regex_constants::ECMAScript|std::regex::icase  );

    }
public:
    void addSignal(std::string sig){
        signal_names.insert(sig);
    }
    void setBits ( mw::bit_widths bw )
    {
        bits = bw;
        max_op_count = 1<<bits.code;
        root = std::make_shared<mw::opCommon> ( max_op_count );
    }
    mw::bit_widths getBits()
    {
        return bits;
    }
    OpSet ( const OpSet&original ) = delete;
    OpSet& operator= ( const OpSet&original ) = delete;
    static OpSet& getInstance();

    std::string verifySigNames(std::string line){
        std::string out;
        std::stringstream sigs(line);
        while(sigs >> out){
            if(!signal_names.count(out) and out[0] !='@' and out!="KONIEC"){
                return out;
            }
        }
        return  "";
    }
    /**
     * Zwrca wektor linii z pliku zawierającego definicję rozkazu
     * Jeśli w danej linijce znaleziono błąd, zaczyna się ona od "Error:"
     */
    std::vector<std::string> parseToLines ( std::string filename )
    {
        std::vector<std::string> output;
        std::ifstream file ( filename );
        if ( !file ) {
            throw mw::fstreamFileNotOpened(filename);

        }
        std::stringstream  buff, comentlessbuff;
        buff << file.rdbuf();
        comentlessbuff = removeComments ( buff );
        std::string tmp, name;
        while ( std::getline ( comentlessbuff, tmp, ';' ) ) {
            if(!std::regex_search(tmp, reg_args)
                and !std::regex_search(tmp, reg_opname)
                and !std::regex_search(tmp, reg_cond_begin) ){
            std::string verOut = verifySigNames(tmp) ;
                if(verOut != ""){
                    output.push_back ( "Error: no signal: "+verOut+" \n"+ tmp );
                }
                else {
                    output.push_back ( tmp );
                }
            } else {
                    output.push_back ( tmp );
                }
        }
        return output;
    }
    /**
     * Dodaje rozkaz zdefiniowany w pliku o podanej nazwie do listy rozkazów
     */
    void addOp ( std::string filename )
    {
        auto reg_end = std::sregex_iterator();
        std::ifstream file ( filename );
        if ( !file ) {

            std::cerr <<"file"<< filename<< " not found >:("<<std::endl;
            return;
        }
        std::stringstream  buff;
        buff << file.rdbuf();
        std::string tmp, name;
        auto opTree = parseLines ( buff );
        std::string content = buff.str();
        std::smatch sname, sargs;
        int args=1;
        if ( std::regex_search ( content, sargs, reg_args ) ) {
            args = std::stoi ( sargs.str ( 1 ) ); //liczba argumentów (domyślnie 1)
        }
        if ( !std::regex_search ( content, sname, reg_opname ) ) {
            throw mw::OpSetMissingName();
        }
        name = toLower ( sname.str ( 1 ) );
        root->setText ( opTree->get() );
        if ( instructions.count ( name ) ) { //jeśli już zdefiniowano rozkaz o takiej nazwie
            try{
                root->replaceNode ( opTree->next ( 0 ), instructions[name].code );
            }catch(mw::OpSetCannotReplace exc){
                instructions[name] = {count++, args, filename};
            }
            //podmień drzewo operacji we wspólnym węźle
            instructions[name].attributes = args; //zaktualizuj liczbę argumentów
            instructions[name].fileOfOrigin = filename; //podmień nazwę pliku źródłowego

        } else { // wprzeciwnym wypadku dodaj nowy
            // podaj następny, ponieważ zawatość pierwszego węzła powinna już być w root
            root->addNode ( opTree->next ( 0 ) );
            instructions[name] = {count++, args, filename};
        }


    }
    /**
     * Zwraca strukturę rozkazu o podanej nazwie
     */
    instruction getInstruction ( std::string  name )
    {
        if ( instructions.count ( name ) )
            return instructions[name];
        else throw  mw::OpSetNotDefined ( name );
    }
    // wolniesze niż getInstruction
    /**
     * Zwraca nazwę rozkazu o podanym kodzie
     */
    std::string  getOpName ( unsigned short int c )
    {
        for ( auto i : instructions ) {
            if ( i.second.code == c and i.first != "rpa" and i.first != "rst" ) {
                return i.first;
            }
        }
        return "";//nie znaleziono rozkazu o takiej nazwie
    }
    /**
     * Sprawdza czy drzewo rozkazu jest na weźle końcowym
     */
    bool isOpEnd()
    {
        return ( current_node == nullptr );
    }
    std::set<std::string> const getOpLine()
    {
        if ( current_node == nullptr ) {
            current_node = root;
        }
        return current_node->get();
    }
    /**
     * Przesuwa wskaźnik na następny węzeł drzewa rozkazów
     */
    void nextOpLine ( unsigned short int code )
    {
        try{
            current_node = current_node->next ( code );
        }catch(mw::OpSetNotDefined){
            current_node = root;
        }
    }
    /**
     * Przywraca wskaźnik węzła na poczatek
     */
    void reset()
    {
        current_node = nullptr;
    }
    /**
     * Usuwa skonfigurowane rozkazy
     */
    void clear(){
        count = 0;
        signal_names.clear();
        instructions.clear();

        instructions["rst"] = {0,1};
        instructions["rpa"] = {0,0};
    }
    // Zwraca nazwę rozkazu zdefiniowanego w podanym pliku, jeśli nie wczytano z tego pliku - zwraca pusty string
   std::string checkLoaded(std::string opfilename){
        for(auto inst : instructions){
            if(inst.second.fileOfOrigin == opfilename)
                return inst.first;
        }
        return "";
    }
};
}
mw::OpSet& mw::OpSet::getInstance()
{
    static mw::OpSet singleton;
    return singleton;
}
