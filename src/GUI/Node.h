#pragma once
#include <SFML/Graphics.hpp>

namespace mw
{
class Node
{
    sf::Vector2f offset;
public:
    Node ( float x, float y ) : offset ( x,y ) {}
};
}
