#ifndef CLIENT_H
#define CLIENT_H

#include <SFML/Network.hpp>
#include "Game.h"

class Client
{
    public:
        //Client();
        Client(sf::RenderWindow& win);
        virtual ~Client();
        void manageEvents(sf::Event event);
        void launch();
        virtual void run();
        virtual void actionLink(int orgX, int orgY, int destX, int destY);
        virtual void actionFree(int mouseX, int mouseY);
        virtual void actionFree(sf::Uint8 nidPlayer, int mouseX, int mouseY);
        virtual void actionChangeLink(int orgX, int orgY, int destX, int destY);
        virtual void manageNetwork();
        void networkActions(sf::Packet& p, sf::Uint8 code);
        void waitServer();
        enum CODE{LINK,FREE,CHANGE,SYNCHRONIZE_GBOARD};
        enum STATE{SERVER_READY,SERVER_NOT_READY,SERVER_START_GAME};
        virtual void menuWaitingRoom();
        void menuServerList();
    protected:
        sf::RenderWindow& window;
        Gameboard gboard;
        std::vector<Player> players;
        bool exitCurrentGame;
        sf::Vector2f posClickedMouse;
        sf::Uint8 idPlayer;
        // network variables:
        sf::TcpSocket socket;
        sf::UdpSocket socket_udp;
        sf::IpAddress serverAdress;
        bool connected;
        bool start;
        sf::Uint8 serverStatus;
        sf::Uint8 playerConnected;
        sf::Uint8 playerMax;
        static const unsigned short int PORT_GAME_A = 53000; // tcp
        static const unsigned short int PORT_GAME_B = 54000; // udp

    private:
};

#endif // CLIENT_H
