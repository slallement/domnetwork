#ifndef MENU_H
#define MENU_H

#include "Utils.h"
#include <vector>
#include <string>
#include <list>
#include <algorithm>
#include <SFML/Graphics.hpp>


class Menu;
struct Button
{
    sf::Text txt;
    void (Menu::*f)();
};

class Menu
{
    public:
        Menu(sf::RenderWindow & window);
        virtual void run();
        virtual ~Menu();
        void exit();
    protected:
        virtual void action();
        virtual void draw();
        void centerTextOnxAxis(sf::Text &txt, float y);
        void loadBackground();

        sf::RenderWindow & window;
        sf::Event event;
        std::vector<Button> option;
        bool end;
        sf::Vector2f clic;

    private:
};

#endif // MENU_H
