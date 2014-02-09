#ifndef CELL_H
#define CELL_H

#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>

class Gameboard;
sf::Packet& operator>>(sf::Packet& packet, Gameboard& gameboard);
sf::Packet& operator<<(sf::Packet& packet, const Gameboard& gameboard);

class Cell
{
    public:
        Cell(){};
        Cell(float cellradius, sf::Vector2f position, float cellCapacity);
        Cell(float cellradius, float posx, float posy, float cellCapacity);
        void setOwner(int ownerId);
        int getOwner() const;
        void update(float dt);
        virtual ~Cell();
    protected:
        float radius;
        sf::Vector2f pos;
        sf::Uint16 capacity;

        sf::Uint8 idOwner;
        sf::Uint16 units;
        float time;
        static const float TSCALE;
    private:
        friend class Gameboard;
        friend class Link;

        friend sf::Packet& operator>>(sf::Packet& packet, Gameboard& gameboard);
        friend sf::Packet& operator<<(sf::Packet& packet, const Gameboard& gameboard);
};


#endif // CELL_H
