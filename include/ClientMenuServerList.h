#ifndef MENU_SERVER_LIST_H
#define MENU_SERVER_LIST_H

#include <SFML/Network.hpp>
#include "Menu.h"

class MenuServerList: public Menu
{
    public:
        MenuServerList(sf::RenderWindow& win, sf::UdpSocket& sock, bool& co,
                        sf::IpAddress& serv_add, sf::Uint8& servStatus);
        ~MenuServerList();
        //virtual void draw();
        //void action();
        virtual void run();
    protected:
        sf::Text textManualIp;
        sf::Text buttonManualIp;
        sf::Text infoInput;
        sf::Text textBar;
        sf::Text info;
        sf::Text info2;
        std::vector<sf::Uint32> listServIp;
        std::vector<sf::Clock> listServTimeout;
        std::vector<sf::Text> listServButton;

        std::vector<std::string> listServText;
        sf::IpAddress directIp;
        sf::IpAddress serv_ip;
        sf::Packet info_server;
        sf::Clock clock;
        short unsigned int serv_port;
        sf::UdpSocket& socket_udp;
        bool& connected;
        sf::IpAddress& serverAddress;
        sf::Uint8& serverStatus;

        static const float FIRST_LINE_Y = 10.f;
        static const float POSBAR_Y = 10.f;// = FIRST_LINE_Y;
        float POSBAR_X;
};

#endif
