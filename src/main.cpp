
#include <iostream>
#include <ctime>
#include <cstdlib>

#include "../include/MainMenu.h"

int main()
{
    srand(static_cast <unsigned int> (time(0)));
    sf::RenderWindow window(sf::VideoMode(400, 300), "DomNetwork",sf::Style::None);
    std::cout<<"--- Begining Dom Network ---"<<std::endl;
    MainMenu menu(window);
    menu.run();
    /*#ifdef GAME_SERVER
        std::cout<<"lancement du server"<<std::endl
        <<"----"<<std::endl;
        Server s;
        s.launch();
    #else

        std::cout<<"lancement du client"<<std::endl
        <<"-----"<<std::endl;
        Client c;
        c.launch();
    #endif*/
    std::cout<<"--- Ending ---"<<std::endl;

    return 0;
}
