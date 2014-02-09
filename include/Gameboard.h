#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <deque>
#include "Link.h"
#include "Player.h"
#include <SFML/Network.hpp>

sf::Packet& operator<<(sf::Packet& packet, const Gameboard& gameboard);
sf::Packet& operator>>(sf::Packet& packet, Gameboard& gameboard);

class Gameboard: public sf::Drawable
{
    public:
        Gameboard();
        Gameboard(std::vector<Player>* playerList);
        virtual ~Gameboard();
        void reset();
        void randomize();
        void initPlayers();
        void init();
        void update(float dt);
        void link(int idOrg, int idDest, float strength=0.5f);
        void changeLinkState(int idOrg, int idDest);
        int getCloserCell(float x, float y);
        int getCloserCell(sf::Vector2f coord);
        int getOwner(int idCell);
        void free(int idPlayer, int idCell);
        friend sf::Packet& operator>>(sf::Packet& packet, Gameboard& gameboard);
        friend sf::Packet& operator<<(sf::Packet& packet, const Gameboard& gameboard);

    protected:
        std::vector<Player>* players;
        std::vector<Cell> cells;
        std::deque<Link> links;
        static sf::Clock elapsed;
        std::vector<float> freqRot;
    private:
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

};


#endif // GAMEBOARD_H
