#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>

class Player
{
    public:
        //Player();
        Player(int Id, sf::Color Color);
        Player(int Id);
        virtual ~Player();
        sf::Color getColor() const {return color;}
        int getId() const {return id;}
        static sf::Color DEFAULT_COLOR[];
    protected:
        int id;
        sf::Color color;

    private:
};


#endif // PLAYER_H
