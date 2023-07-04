#pragma once
/** @file */ //doxygen
#include "Editor.h"
#include "PopUpFileInput.h"
#include "../Assembler.h"
#include <unistd.h>

namespace mw
{

//metody operujące na stringach do poprawnienia wyświetlania

enum filetype{
    none,
    program,
    operation
};
class FileView : public IWidget
{
    std::shared_ptr<mw::IElement> addressRegister=nullptr;
    sf::Color highlightColor = sf::Color::Blue;
    float lineHeight =100;
    int firstError = 0;
    mw::Assembler assembler;
    bool progError = false;
    filetype currentFiletype=none;
    std::vector<sf::Text> lines;
    std::string filename;
    sf::Font font;
    sf::Color bg, fg;
    float lastLineY=0;
    int lineCount = 20;
    sf::Mutex lines_mutex;

    const float lineSpace = 10;
public:
    FileView ( sf::Vector2f windowSize, sf::FloatRect location, std::string fontname="monospace.otf" ) : IWidget ( windowSize, location )
    {
        font.loadFromFile ( fontname );
        bg = sf::Color::White;
        fg = sf::Color::Black;
        //Informacja dla użytkownika
        addLine ( "Use the buttons above");
        addLine ( "to open a program");
        addLine ( "or operation file");


    }
    void reload(){
        switch(currentFiletype){
            case program:{
                  auto ar_keep = addressRegister; //przy ponownym wczytaniu tego samego pliku zachowaj rejestr adresu
                  loadProgram(filename);
                  addressRegister = ar_keep;
                }break;
            case operation:
                loadOperation(filename);
                break;
            default:break;
        }
    }
    std::string getFilename()
    {
        return filename;
    }
    void addLine( sf::String text){
    	addLine(text, fg);
    }
    void addLine ( sf::String text, sf::Color clr )
    {
        sf::Text line;
        line.setCharacterSize ( 80 );
        line.setFillColor ( clr );
        line.setFont ( font );
        line.setString ( text );
        line.setPosition ( {0, lastLineY} );

        auto gb = line.getGlobalBounds();
        lastLineY += lineHeight;
        Bounds = {gb.getPosition().x-15, lineHeight*currentScroll, 1500, lineHeight* ( lineCount+1 ) };
        lines_mutex.lock();
        lines.push_back ( line );
        lines_mutex.unlock();

    }

        bool scrolled( sf::Vector2f event, int ammount ){

        currentScroll-= ammount;
        float top = currentScroll*lineHeight;
        float height = lineHeight* ( lineCount+1 );
        if( top > 0 and top <= lastLineY-height)
            Bounds = {-15, top, 1500,  height};
        else
            currentScroll += ammount;
        
        return true;
    }

    void openEditor(){
        mw::Editor::open(filename, "featherpad", firstError );
    }

    void loadOperation ( std::string file_name )
    {
        addressRegister = nullptr;
        firstError = 0;
        progError = false;
        currentScroll = 0;
        if ( file_name=="" )
            return;
        clear();
        currentFiletype = operation;
        std::regex whitespace ( "\\s*" );
        filename = file_name;
        std::vector<std::string> opLines;
        try{
            opLines = mw::OpSet::getInstance().parseToLines ( file_name );
        }catch(mw::fstreamException exc){
            mw::PopUpWarning(exc.what()).display();
        }
        int lc = 0;

        for ( auto line : opLines ) {

            if ( !std::regex_match ( line, whitespace ) ) {
                sf::Color lineColor = fg;
                if(line.find("Error:")!=std::string::npos){
                    lineColor = sf::Color::Red;
                    if(!progError)firstError = lc+1;
                    progError = true;
                }
                std::string toPrint = removeChar ( line, '\n' )+";";
                std::stringstream ss ( addLineBreaks ( toPrint, 25 ) );
                std::string sub;
                getline ( ss, sub );
                addLine ( normalizeLength ( std::to_string ( lc++ ), 3 )+sub, lineColor );
                while ( getline ( ss, sub ) ) {
                    addLine ( "   "+sub, lineColor);
                }


            }
        }


    }

    bool addOperation()
    {
        if(progError){
            mw::PopUpWarning("Unable to add operation\nPlease fix errors first").display();
            return false;
        }
        try {
            mw::OpSet::getInstance().addOp ( filename );
        } catch ( mw::OpSetException exc ) {
            mw::PopUpWarning ( exc.what() ).display();
        }
        return true;
    }
    void hookAddressRegister(std::shared_ptr<mw::IElement> A){

        addressRegister = A;
    }
    void unhookAddressRegister(){
        addressRegister = nullptr;
    }
    bool progAssemble()
    {
        if(progError){
            mw::PopUpWarning("Unable to assemble\nPlease fix errors first").display();
            return false;
        }
        assembler.assemble ( filename );
        return true;
    }
    void loadProgram ( std::string file_name )
    {
        addressRegister = nullptr;
        firstError = 0;
        progError =false;

        currentScroll = 0;

        if ( file_name=="" )
            return;
        currentFiletype = program;
        clear();
        filename = file_name;
        auto lines = assembler.parseToLines ( filename );
        int lc = 0;
        for ( auto &line : lines ) {
            std::string toPrint = normalizeLength(std::to_string ( lc++ ), 4);
            toPrint+=normalizeLength ( line.label, 8 );
            toPrint+=normalizeLength(line.operation, 8)+line.argument;

			if(line.label == "Error"){
                if(!progError){
                    firstError = lc;
                    progError =true;
                }

				auto lineColor = sf::Color::Red;
                //wyjątek zajmuje kilka linii
                std::stringstream ss ( toPrint  );
                std::string sub;
                while ( getline ( ss, sub ) ) {
                    addLine ( sub, lineColor );
                }

            }else{
                addLine ( toPrint,fg );
            }



        }
    }
    void setHighlightColor(sf::Color clr){
      highlightColor = clr;
    }
    void setBGColor ( sf::Color clr )
    {
        bg = clr;
    }
    void setTextColor ( sf::Color clr )
    {
        fg = clr;
        for ( auto &l : lines )
            l.setFillColor ( clr );
    }
    sf::RectangleShape highlight(){
      sf::RectangleShape output;
      if(addressRegister != nullptr){
        output.setSize({1500,lineHeight});
        int index = addressRegister->getValue();
        if(index >= 0 and index <  lines.size()){
          auto lPos = lines[index].getPosition();
          output.setPosition(lPos);
          output.setFillColor(highlightColor);

        }
      }
      return output;
    }

    void display()
    {
        resetZoom();
        Texture.clear ( bg );

        lines_mutex.lock();
        Texture.draw(highlight());
        for ( auto &l : lines ) {

            Texture.draw ( l );
        }
        lines_mutex.unlock();
        Texture.display();
    }
    void clear()
    {
        Bounds = {0,0,0,0};
        lines_mutex.lock();
        lines.clear();
        lines_mutex.unlock();
        lastLineY = 0;
    }
    actionCode clicked ( sf::Vector2f event ){
      return empty;
    }
};
}
