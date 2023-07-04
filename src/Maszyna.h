#pragma once
/** @file */ //doxygen
#include "Counter.h"
#include "InstructionRegister.h"
#include "ALU.h"
#include "Bus.h"
#include "Call.h"
#include "Connect.h"
#include "Empty.h"
#include <iterator>
#include "RAM.h"

#include "JSON.h"

namespace mw
{


struct status {
    std::vector<std::string> activeSignals;

};
/**
 * Klasa Maszyna zbiera w sobie wszystkie elementy, sygnały i połączenia między nimi. Wystawia zewnętrzne metody do sterowania pracą maszyny. 
 */
class Maszyna
{
    mw::OpSet* opSet;
    std::vector<std::string> usedConfigFiles;
    status s;
    //zapisane jako mapa, ponieważ można podpiąć więcej niż 1 moduł pamięci, a dostęp jest potrzebny tylko przy ładowaniu programu
    std::map<std::string, std::shared_ptr<mw::RAM>> memory;
    //ram powinien utrzymać się w pamięci przez call, ale nie mam pewności

    std::shared_ptr<mw::InstructionRegister> I;

    std::map<std::string, std::shared_ptr<mw::IElement>> elements; //rejestry(zawartość pamięci), liczniki
    std::vector<std::shared_ptr<mw::IClearable>> clearables;
    static const int signal_phases = 4;
    // 4 poziomy sygnałów,flat to 0, a pulse 3

    std::map<std::string, std::shared_ptr<mw::ISignal>> signals[signal_phases];

    mw::bit_widths bits;
    /**
     * Dekoduje szerokość bitową podaną w pliku konfiguracyjnym
     */
    short int getWidth ( json width )
    {
        if ( width.type() == json::value_t::null ) {
            //jeśli nie podano to domyślnie: pełna szerokość
            return bits.code+bits.data;
        } else if ( width.type() == json::value_t::string ) {
            //jeśli string to odpowiadający alias
            if ( width == "code" )
                return bits.code;
            else if ( width == "data" )
                return bits.data;
            else if ( width == "full" )
                return bits.data+bits.code;
            else
                throw mw::configUnknownWidthAlias(width);
        } else if ( width.type() == json::value_t::number_integer ) {
            return width;
        } else return 0;

    }
    /**
     * Dodaje element pamięci na podstawie obiektu json
     */
    void addRAM ( json conf )
    {
        auto ram = std::make_shared<mw::RAM> ( getWidth ( conf["width"] ), getWidth ( conf["addr-width"] ) );
        std::vector<std::shared_ptr<mw::Register>> registers = ram->getRegisters();
        std::string name = conf["name"];
        for ( int i = 0; i<registers.size()-2; i++ ) {
            elements[name+"#"+std::to_string ( i )] = registers[i];
        }
        elements[name+"#A"] = registers[registers.size()-2]; //rejestr A przekazany jako przedostatni w wektorze
        elements[name+"#S"] = registers[registers.size()-1]; //rejestr S przekazany jako ostatni w wektorze
        addCalls ( conf["calls"], ram );
        memory[name] =ram;
    }
    /**
     * Dodaje rejestr na podstawie obiektu json
     */
    void addRegister ( json conf )
    {
        auto reg = std::make_shared<mw::Register> ( getWidth ( conf["width"] ) );
        elements[conf["name"]] = reg;
    }
    /**
     * Dodaje magisstralę na podstawie obiektu json
     */
    void addBus ( json conf )
    {
        auto bus = std::make_shared<mw::Bus> ( getWidth ( conf["width"] ) );
        elements[conf["name"]] = bus;
        clearables.push_back ( bus );
    }
    /**
     * Dodaje jednostkę arytmetyczno-logiczną na podstawie obiektu json
     */
    void addALU ( json conf )
    {
        auto alu = std::make_shared<mw::ALU> ( getWidth ( conf["width"] ) );
        clearables.push_back ( alu );
        addCalls ( conf["calls"],alu );
        elements[conf["name"]] = alu;
    }
    /**
     * Dodaje wywołania na podstawie obiektu json, dla podanego wywoływalnego obiektu
     */
    void addCalls ( json conf, std::shared_ptr<mw::ICallable> target )
    {
        for(auto calls : conf.items()) {
            int phase;
            if(calls.key() == "pulse") {
                phase = signal_phases-1;
            } else if(calls.key() == "flat") {
                phase = 0;
            } else {
                phase = std::stoi(calls.key());
                if(phase >= signal_phases) {
                    std::cerr << "ERROR phase exceeded  signal_phases"<<std::endl;
                }
            }
            for ( auto call : calls.value().items()) {
                signals[phase][call.key()] = std::make_shared<mw::Call> ( target, ( int ) call.value() );
            }
        }
    }
    /**
     * Dodaje licznik na podstawie obiektu json
     */
    void addCounter ( json conf )
    {
        auto counter = std::make_shared<mw::Counter> ( getWidth ( conf["width"] ) );
        elements[conf["name"]] = counter;
        addCalls ( conf["calls"], counter );
    }
    /**
     * Dodaje rejestr instrukcji (może być tylko jeden)
     */
    void addInstructionRegister()
    {
        I = std::make_shared<mw::InstructionRegister> ( opSet->getBits() );
        elements["I"] = I;
    }


    /**
     * Dodaje elementy wymienione w obiekcie JSON
     */
    void addElements ( json conf )
    {
        for ( auto element: conf ) {
            if ( element["type"]=="REGISTER" ) {
                addRegister ( element );
            } else if ( element["type"]=="COUNTER" ) {
                addCounter ( element );
            } else if ( element["type"]=="BUS" ) {
                addBus ( element );
            } else if ( element["type"]=="ALU" ) {
                addALU ( element );
            } else if ( element["type"]=="RAM" ) {
                addRAM ( element );
            }
        }
    }
    /**
     * Dodaje pojedyncze połączenie na podstawie obiektu json, dla podanej fazy taktu
     */
    void addConnection(json conn, int phase) {
        auto inp = elements[conn["join"][0]];
        auto out = elements[conn["join"][1]];

        signals[phase][conn["signal"]] = std::make_shared<mw::Connect> ( inp, out );
    }
    /**
     * Dodaje połączenia na podstawie obiektu json
     */
    void addConnections ( json conf )
    {
        for(auto connections : conf.items()) {
            //połączenia wprowadź po utworzeniu wszystkich elementów
            int phase;
            if(connections.key() == "pulse") {
                phase = signal_phases-1;
            } else if(connections.key() == "flat") {
                phase = 0;
            } else {
                phase = std::stoi(connections.key());
            }
            for ( auto conn : connections.value()) {
                addConnection(conn, phase);
            }
        }
    }

    /**
     * Dokonue konfiguracji na podstawie obiektu json
     */
    void configFromJSON ( json conf )
    {
        //jeśli załączony jest inny plik, wczytaj go najpierw
        if ( conf["include"]!=nullptr ) {
            json included = mw::openJSON(conf["include"]["file"]);
            //wywołanie rekurencyjne
            if ( std::find ( usedConfigFiles.begin(), usedConfigFiles.end(), conf["include"]["file"] ) !=usedConfigFiles.end() ) {
                throw mw::configRecursiveInclude ( conf["include"]["file"] );
            }
            configFromJSON ( included );
            usedConfigFiles.push_back(conf["include"]["file"]);
        }
        //załaduj instrukcje wymienione na liście
        for ( std::string filename : conf["machine"]["instructions"] ) {
            opSet->addOp ( filename );
        }
        addElements ( conf["machine"]["elements"] );
        addConnections ( conf["machine"]["connections"] );
    }
public:
    Maszyna()
    {
        opSet = &mw::OpSet::getInstance();
    };
    /**
     * Całkowicie czyści konfigurację maszyny
     */
    void configClear()
    {
        for(int i = 0; i<signal_phases; i++) {
            signals[i].clear();
        }
        clearables.clear();
        elements.clear();
        memory.clear();
        usedConfigFiles.clear();
    }
    /**
     * Konfiguruje maszynę zgodnie z plikiem o podanej nazwie
     */
    std::string configFromFile ( std::string filename )
    {
        /**
        *Konfigurując trzeba wczytać szerokość bitów z najwyższego pliku
        * a następnie konstruować maszynę od dołu w sensie
        * najpierw reukurencyjnie wczytujemy załączone pliki
        **/
        std::cout << "Machine Loading file: "<<filename<<std::endl;
        auto config = mw::openJSON(filename);
        auto jbits = config["machine"]["bits"];
        if ( jbits == nullptr ) {
            throw mw::CPUconstructionError();
        }

        bits.code = ( int ) jbits["code"];
        bits.data= ( int ) jbits["data"];
        std::cout << "start recursion"<<std::endl;
        opSet->reset();
        opSet->clear();
        opSet->setBits ( bits );
        addInstructionRegister();

        configFromJSON ( config );
        signals[signal_phases-1]["stop"] = std::make_shared<mw::Empty>(); //Specjalny pusty sygnał którego nasłuchuje UI
        //przekaż do opSet dostępne sygnały
        for(int i = 0; i< signal_phases; i++) {
            for(auto sig : signals[i]) {
                opSet->addSignal(sig.first);
            }
        }
        std::cout << "Maszyna skonfigurowana"<<std::endl;

        return config["machine"]["name"];

    }

    /**
     * Zeruje wszystkie rejestry, ale zachowuje zawartość pamięci
     */
    void reset()
    {
        for ( auto &reg : elements ) {

            if ( reg.first.find ( "#" ) ==std::string::npos ) { //jeśli nie jest elementem pamięci
                reg.second->reset();
            }
        }

        for(auto mem :  memory) {
            mem.second->reset();
        }
        opSet->reset();
    }
    /**
     * Wykonuje jeden rozkaz. Zwraca aktywne sygnały
     */
    status op()
    {
        // wykonaj rozkaz
        status out;
        do {
            out = tick();
        } while ( !opSet->isOpEnd() );
        return out;

    }
    /**
     * Wykonuje jeden takt (linijkę rozkazu). Zwraca aktywne sygnały
     */
    status tick()
    {
        //wykonaj takt
        std::cout << "Tick start"<<std::endl;
        s.activeSignals.clear();

        for ( auto cl : clearables ) {
            cl->clear();
        }
        std::string op="";
        std::set<std::string> ops;
        try {
            ops = opSet->getOpLine();
        } catch ( mw::OpSetNotDefined exc ) {
            std::cout << exc.what();
        }
        for(int i = 0; i< signal_phases; i++) {
            std::cout << "signal phase "<<i <<std::endl;
            for ( auto op : ops ) {
                if ( signals[i].find ( op ) !=signals[i].end() ) {
                    s.activeSignals.push_back ( op );
                    std::cout << "Doing op: " << op<<std::endl;
                    signals[i][op]->send();
                    std::cout <<  op << " Done."<<std::endl;
                }
            }
        }

        opSet->nextOpLine ( I->getOpCode() );

        return s;

    }
    /**
     * Zwraca nazwy dostępnych elementów pamięci
     */
    std::vector<std::string> getMemoryLabels() {
        std::vector<std::string> memnames;
        for(auto mem : memory) {
            memnames.push_back(mem.first);
        }
        return memnames;
    }
    /**
     * Zwraca wskaźnik na mapę elementów składowych maszyny
     */
    std::map<std::string, std::shared_ptr<mw::IElement>>* getElements()
    {
        return &elements;
    }
    /**
     * Zwraca nazwy zdefiniowanych sygnałów
     */
    std::vector<std::string>  getSignals()
    {
        std::vector<std::string> temp;
        for(int i = 0; i <   signal_phases; i++) {
            for ( auto s : signals[i]) {
                temp.push_back ( s.first );
            }
        }
        return temp;
    }

    /**
     * Ładuje program z pliku 'comp' do elementu pamięci o podanej nazwie
     */
    std::shared_ptr<mw::IElement> loadProgram ( std::string name )
    {
        int com;
        std::vector<int> list;
        std::ifstream compiled;
        compiled.open ( "comp" );
        while ( compiled >> com ) {
            list.push_back ( com );
        }
        return memory[name]->loadData ( list ); //zwraca adres rejestru adresowego pamięci
    }

};
}
