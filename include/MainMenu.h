#ifndef MAINMENU_H
#define MAINMENU_H

#include "Menu.h"
#include "Utils.h"
#include <vector>
#include <string>
#include <list>
#include <algorithm>
#include <SFML/Graphics.hpp>


/*class MainMenu;
struct Button
{
    sf::Text txt;
    void (MainMenu::*f)();

};*/

class MainMenu : public Menu
{
    public:
        MainMenu(sf::RenderWindow & window);
        virtual ~MainMenu();

        void server();
        void client();
        void mode();
        //void exit();
    protected:
        virtual void draw();
        void centerTextOnxAxis(sf::Text &txt, float y);
        void recomputePos();
        void loadBackground();

        bool fullscreenMode;
        sf::Text title;

    private:
};

#endif // MAINMENU_H
