#include <SFML/Audio.hpp>
#include <SFML/System.hpp>
/** @file */ //doxygen
#pragma once
namespace mw{
class SFX {
    sf::SoundBuffer greet, tick, op, error, exit, done;
    sf::Sound player;
    SFX(){
        greet.loadFromFile("sounds/greet.ogg");
        exit.loadFromFile("sounds/exit.ogg");
        error.loadFromFile("sounds/error.ogg");
        tick.loadFromFile("sounds/tick.ogg");
        op.loadFromFile("sounds/op.ogg");
        done.loadFromFile("sounds/done.ogg"); 
    }
    
public:
    static SFX &get();
    void Error(){
        player.setBuffer(error);
        play();
    }
    void Greet(){
        player.setBuffer(greet);
        play();
    }
    void Tick(){
        player.setBuffer(tick);
        play();
    }
    void Op(){
        player.setBuffer(op);
        play();
    }
    void Done(){
        player.setBuffer(done);
        play();
    }
    void Exit(){
        player.setBuffer(exit);
        play();
    }
    void play(){
        player.play();
    }
};

SFX& SFX::get(){
    static SFX singleton = SFX();
    return singleton;
}
}
