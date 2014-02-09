#ifndef MAINMENU_H
#define MAINMENU_H

#include "Utils.h"
#include <vector>
#include <string>
#include <list>
#include <algorithm>
#include <SFML/Graphics.hpp>


class MainMenu;
struct Button
{
    sf::Text txt;
    void (MainMenu::*f)();
};

class MainMenu
{
    public:
        MainMenu(sf::RenderWindow & window);
        virtual void run();
        virtual ~MainMenu();

        void server();
        void client();
        void mode();
        void exit();
    protected:
        void centerTextOnxAxis(sf::Text &txt, float y);
        void loadBackground();

        sf::RenderWindow & window;
        sf::Event event;
        std::vector<Button> option;
        bool end;
        bool fullscreenMode;
        sf::Vector2f clic;

    private:
};

#endif // MAINMENU_H
