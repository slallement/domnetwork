#include "../include/Player.h"

sf::Color Player::DEFAULT_COLOR[] =
    {sf::Color(128, 128, 128),
    sf::Color(0, 255, 0),
    sf::Color(255, 0, 0),
    sf::Color(0, 0, 255),
    sf::Color(255, 0, 255),
    sf::Color(255, 255, 0),
    sf::Color(0, 255, 255),
    sf::Color(128, 255, 255),
    sf::Color(255,128,255)
    };

Player::Player(int Id, sf::Color Color):
    id(Id), color(Color)
{

}

Player::Player(int Id):
    id(Id)
{
    if(Id > 9){
        color = sf::Color(250,250,250);
    }else{
        color = DEFAULT_COLOR[Id];
    }
}

Player::~Player()
{
    //dtor
}
