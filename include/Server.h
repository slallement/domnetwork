#ifndef SERVER_H
#define SERVER_H

#include <SFML/Network.hpp>
#include "Game.h"
#include "Client.h"

class Server: public Client
{
    public:
        //Server();
        Server(sf::RenderWindow& win);
        virtual ~Server();
        void manageEvents(sf::Event event);
        void launch();
        virtual void actionLink(int orgX, int orgY, int destX, int destY);
        virtual void actionFree(int mouseX, int mouseY);
        virtual void actionFree(sf::Uint8 nidPlayer, int mouseX, int mouseY);
        virtual void actionChangeLink(int orgX, int orgY, int destX, int destY);
        void menuWaitingRoom();
        void waitClient();
        void waitAll();
        virtual void manageNetwork();
        void addPlayer();
    protected:
        void resyncClients();
        sf::TcpListener listener;
        sf::SocketSelector selector;
        std::vector<sf::TcpSocket*> sockets;
        sf::Clock timeElapsed; // timer between each resynchronizations
        static const sf::Int32 T_RESYNC = 1000; // time of resync
    private:
};

#endif // SERVER_H
