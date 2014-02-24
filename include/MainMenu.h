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
        void centerTextOnxAxis(sf::Text &txt, float y);
        void recomputePos();
        void loadBackground();

        bool fullscreenMode;

    private:
};

#endif // MAINMENU_H
