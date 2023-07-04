#pragma once
#include "Register.h"
#include "Counter.h"
#include "InstructionRegister.h"
#include "ALU.h"
#include "OpSet.h"
#include "Call.h"
#include "Connect.h"
#include "Empty.h"
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <iterator>
#include "RAM.h"
#include <sstream>
#include <functional>
namespace mw{
    
    struct status{
        std::vector<std::string> activeSignals;
        std::map<std::string, bool> flags;
    };
    class CPUconstructionError : public std::exception {
    public:
        std::string what() {
          return "Error while creating CPU";
    }  
    };
    class CPU {
    std::shared_ptr<mw::OpSet> opSet;
    
    std::shared_ptr<mw::RAM> ram;
    std::shared_ptr<mw::ALU> alu;
    std::shared_ptr<mw::Counter> L;
    std::shared_ptr<mw::InstructionRegister> I;
    std::map<std::string, std::shared_ptr<mw::Bus>> bus;
    
    std::map<std::string, std::shared_ptr<mw::IElement>> elements;
    
    std::map<std::string, std::shared_ptr<mw::ISignal>> sig_flat;
    std::map<std::string, std::shared_ptr<mw::ISignal>> sig_pulse;
    
    mw::bit_widths bits;
 

    void addConnection(std::map<std::string, std::shared_ptr<mw::ISignal>> &sig_map, std::string alias,
                                std::shared_ptr<mw::IElement> input,std::shared_ptr<mw::IElement> output){
        sig_map[alias] = std::make_shared<mw::Connect>(input, output);
                            
    }
    void addCall(std::map<std::string, std::shared_ptr<mw::ISignal>> &sig_map, std::string alias, 
                                        std::shared_ptr<mw::ICallable> target, std::string call_name){
        sig_map[alias] = std::make_shared<mw::Call>(target, call_name);
    }
    public:
        CPU(unsigned short int cb=3, unsigned short int db = 5) {
            if(cb>0 and db>0){
                bits.code=cb;
                bits.data=db;
            }
            else {
                throw mw::CPUconstructionError();
            }
            bus["magA"] = std::make_shared<mw::Bus>(bits.data);
            bus["magS"] = std::make_shared<mw::Bus>(bits.code+bits.data);
            opSet = std::make_shared<mw::OpSet>(cb);
            opSet->addOp("stp; czyt wys wei il; stop;");
            opSet->addOp("dod; czyt wys wei il; wyad wea; czyt wys weja dod weak;");
            opSet->addOp("ode; czyt wys wei il; wyad wea; czyt wys weja ode weak;");
            
            //Inicjalizowanie Pamięci operacyjnej
            ram = std::make_shared<mw::RAM>(bits);
            std::vector<std::shared_ptr<mw::Register>> registers = ram->getRegisters();
            for(int i = 0; i<registers.size()-2; i++){
                elements["#"+std::to_string(i)] = registers[i];
            }
            elements["A"] = registers[registers.size()-2]; //rejestr A przekazany jako przedostatni w wektorze
            elements["S"] = registers[registers.size()-1]; //rejestr S przekazany jako ostatni w wektorze
            
            //Inicjalizowanie JAL
            alu = std::make_shared<mw::ALU>(bits, bus["magS"]);
            elements["AK"] = alu->getAK();
            
            
            //Rejestr instrukcji zawsze musi sie pojawić w konstrukcji maszyny W.
            I = std::make_shared<mw::InstructionRegister>(bits);
            elements["I"] = I;
            //addElement<mw::InstructionRegister>("I");

            elements["L"] = std::make_shared<mw::Counter>(bits.data);

            addConnection(sig_flat, "wel", bus["magA"], elements["L"]);
            addConnection(sig_pulse, "wyl", elements["L"], bus["magA"]);
            addCall(sig_pulse, "il", elements["L"], "inc");
            
            
            addConnection(sig_flat, "wyad", elements["I"], bus["magA"]);
            addConnection(sig_pulse, "wei", bus["magS"], elements["I"]);
            
            addConnection(sig_flat, "wys", elements["S"], bus["magS"]);
            addConnection(sig_pulse, "wes", bus["magS"], elements["S"]);
            addConnection(sig_pulse, "wea", bus["magA"], elements["A"]);
            addConnection(sig_flat, "wyak", elements["AK"], bus["magS"]);
            addCall(sig_flat, "weja", alu, "weja");
            addCall(sig_flat, "dod", alu, "dod");
            addCall(sig_flat, "ode", alu, "ode");
            addCall(sig_flat, "przep", alu, "przep");
            addCall(sig_pulse, "weak", alu, "weak");
            addCall(sig_flat, "czyt", ram, "czyt");
            addCall(sig_pulse, "pisz", ram, "pisz");
            sig_flat["stop"] = std::make_shared<mw::Empty>(); //Specjalny pusty sygnał którego nasłuchuje UI
            elements["#0"]->setValue(33);
            elements["#1"]->setValue(5);
            elements["AK"]->setValue(20);
            
            std::cout << "Construction successfull!"<<std::endl;
        };
        void reset() {
            for(auto &reg : elements){
                reg.second->reset();
            }
        }
        status tick(){
            status s;
            std::cout << "Tick start"<<std::endl;
            alu->clear();
            for(auto b : bus){
                b.second->clear();
            }
            std::string op="";
            std::string line="";
            auto oc = I->getOpCode();
            line = opSet->getOpLine(oc.first, oc.second);
            //std::getline(std::cin, line, ';');
            std::stringstream ss(line);
            std::set<std::string> ops;
            std::cout << "Operations start"<<std::endl;
            while(ss >> op){
                ops.insert(op);
            }
            for(auto op : ops){
                if(sig_flat.find(op)!=sig_flat.end()){
                    s.activeSignals.push_back(op);
                    try{
                        std::cout << "Doing op:" << op<<std::endl;
                        sig_flat[op]->send();
                        std::cout << "Done." << op<<std::endl;
                    }catch(mw::InputError exc){
                        std::cout << "Wystapił błąd: " << exc.what() <<std::endl;
                    }
                }
            }
            std::cout << "Done with flats." <<std::endl;
            for(auto op : ops){
                if(sig_pulse.find(op)!=sig_pulse.end()){
                    s.activeSignals.push_back(op);
                    try{
                        std::cout << "Doing op:" << op<<std::endl;
                        sig_pulse[op]->send();
                        std::cout << "Done." << op<<std::endl;
                    }catch(mw::InputError exc){
                        std::cout << "Wystapił błąd: " << exc.what() <<std::endl;   
                    }catch(mw::ALUmissingOperation exc){
                         std::cout << "Wystapił błąd: " << exc.what() <<std::endl;   
                    }catch(mw::ALUmissingInput exc){
                         std::cout << "Wystapił błąd: " << exc.what() <<std::endl;   
                    }
                }
            } std::cout << "Done with Impulses. print elements:" <<std::endl;
            for(auto  el : elements){
                std::cout << el.first <<":" << el.second->getValue()<<" ";
            }
            std::cout <<std::endl;
            s.flags["Z"] = alu->NegativeInAK();
            s.flags["ZAK"] = alu->ZeroInAK();
            s.flags["op_code"] = oc.first;
            s.flags["cnt"] = oc.second;
            return s;
            
        }
        mw::bit_widths getBitWidths(){
            return bits;
        }
        std::map<std::string, std::shared_ptr<mw::IElement>>* getElements(){
            return &elements;
        }
        std::vector<std::string>  getSignals(){
            std::vector<std::string> temp;
            for(auto f : sig_flat){
                temp.push_back(f.first);
            }
            for(auto p : sig_pulse){
                temp.push_back(p.first);
            }
            return temp;
        }
    
    };
}
