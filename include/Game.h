#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <vector>
#include "Player.h"
#include "Gameboard.h"

class Game
{
    public:
        Game();
        void run();
        virtual ~Game();
        void proceed(int orgX, int orgY, int destX, int destY);
        void changeLinkState(int orgX, int orgY, int destX, int destY);
        void manageEvents(sf::Event e);
    protected:
        sf::RenderWindow window;
        std::vector<Player> players;
        Gameboard gboard;
        int idPlayer;
        sf::Vector2f posClickedMouse;
    private:
};

#endif // GAME_H
