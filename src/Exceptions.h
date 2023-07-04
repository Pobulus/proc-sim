#pragma once
/** @file */ //doxygen
#include <exception>
#include <string>
#include <cstring>

namespace mw
{
class RuntimeException : public std::exception{
  std::string errorName;
public:

    RuntimeException ( std::string errorName ) : errorName ( errorName )
  {}
    virtual ~RuntimeException(){};
    virtual std::string what(){return errorName;}
};

class fstreamException : public std::exception
{
    std::string errorName;
public:
    fstreamException ( std::string errorName ) : errorName ( errorName )
    {}
    std::string what()
    {
        return "fstream exception:\n"+errorName;
    }
};

class fstreamFileNotOpened: public fstreamException
{
public:
    fstreamFileNotOpened(std::string filename = "") :fstreamException ( "File "+filename +" couldn't be opened")
    {}
};

class CPUexception : public RuntimeException
{
public:
    CPUexception ( std::string errorName ) : RuntimeException("CPU exception:\n"+errorName)
    {}
};

class CallableMissingCallId : public CPUexception
{
public:
  CallableMissingCallId ( short int call_id=0 ) : CPUexception("No call with ID: "+std::to_string(call_id))
  {}
};

class RAMnotEnoughMemory : public CPUexception
{
public:
  RAMnotEnoughMemory(int excess) : CPUexception("RAM memory exceeded by "+std::to_string(excess)+" words")
  {}
};

class CPUprogramTimeout: public CPUexception
{
public:
    CPUprogramTimeout() :CPUexception ( "CPU has reached  the limit of operations per program. Possible infinite loop" )
    {}
};



class CPUconstructionError : public CPUexception
{
public:
  CPUconstructionError() : CPUexception ( "Error while creating CPU" )
  {}
};

class OpSetException : public RuntimeException
{
public:
    OpSetException ( std::string errorName ) : RuntimeException ( "OpSet exception:\n"+errorName )
    {}
};


class OpSetFull : public OpSetException
{
public:
    OpSetFull() :OpSetException ( "Operation set is full\nincrease code bits\nor replace existing operation" )
    { }
};



class OpSetMissingName : public OpSetException
{
public:
    OpSetMissingName() :OpSetException ( "no Operation name specified" )
    { }

};
class OpPrimaryLineMismatch : public OpSetException
{
public:
    OpPrimaryLineMismatch() :OpSetException ( "First line must be the same for each operation" )
    { }

};
class OpSetCannotReplace: public OpSetException
{
public:
    OpSetCannotReplace( int code ) : OpSetException ( "Cannot replace"+std::to_string ( code )+"\n it's not defined in opCommon" )
    {}
};
class OpSetNotDefined: public OpSetException
{
public:
    OpSetNotDefined ( int code ) : OpSetException ( "No operation with code "+std::to_string ( code ) )
    {}
    OpSetNotDefined ( std::string name ) : OpSetException ( "No operation named "+name )
    {}
};
class OpSetEmpty : public OpSetException
{
public:
    OpSetEmpty ( ) : OpSetException ( "No operations are loaded" )
    {}
};
class ConfigException : public std::exception
{
    std::string errorName;
public:
    ConfigException ( std::string errorName ) : errorName ( errorName )
    {}
    std::string what()
    {
        return "Config exception:\n"+errorName;
    }
};

class configCPUFatalError : public ConfigException
{
public:
    configCPUFatalError() : ConfigException ( "Fatal error in configuration" )
    {}

};


class configMissingAtribute : public ConfigException
{

public:
    configMissingAtribute ( std::string atr_name, std::string origin="" ) :ConfigException ( "Missing attribute "+atr_name+((origin!="")?"in:\n"+origin:""))
    {}
};

class configUnknownWidthAlias : public ConfigException
{

public:
    configUnknownWidthAlias ( std::string alias ) :ConfigException ( "Unknown bit width alias: \""+alias+"\"")
    {}
};


class configUnknownNode : public ConfigException
{

public:
    configUnknownNode ( std::string node_name ) : ConfigException ( "Unknown node "+node_name )
    {}
};
class configUnknownElement : public ConfigException
{

public:
    configUnknownElement ( std::string el_name ) :  ConfigException ( "Unknown element "+el_name )
    {}

};
class configRecursiveInclude: public ConfigException
{

public:
    configRecursiveInclude ( std::string file_name ) :  ConfigException ( "Recursive Inclusion in file: " +file_name )
    {}
};



class BusInUse : public CPUexception
{
public:
  BusInUse():CPUexception("Bus already in use")
  {}
};

class BusEmpty : public CPUexception
{
public:
  BusEmpty():CPUexception("Bus is empty")
  {}
};
class ALUmissingOperation : public CPUexception
{
public:
  ALUmissingOperation():CPUexception("ALU operation not set")
  {}
};

class ALUmissingInput : public CPUexception
{
public:
  ALUmissingInput():CPUexception("ALU input is not connected")
  {}


};
class ALUoverwriteOperation : public CPUexception
{
public:
  ALUoverwriteOperation():CPUexception("ALU operation already set")
  {}


};


}
