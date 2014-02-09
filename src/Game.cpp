#include "../include/Game.h"
#include "../include/Gameboard.h"
#include <iostream>

Game::Game():
    window(sf::VideoMode(800, 600), "SFML works!"), gboard(&players),
    idPlayer(1), posClickedMouse(-1,-1)
{
    idPlayer = 1;
    players.push_back(Player(1,sf::Color(0,255,0) ));
    players.push_back(Player(2,sf::Color::Red));
}

void Game::run()
{
    gboard.init();
    float dt = 1.f/60.f;


    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            manageEvents(event);
        }

        gboard.update(dt);

        window.clear();
        window.draw(gboard);
        window.display();
    }

}

inline void Game::manageEvents(sf::Event event)
{
    if (event.type == sf::Event::Closed)
        window.close();
    if (event.type == sf::Event::KeyPressed)
    {
        if (event.key.code == sf::Keyboard::Escape)
        {
            window.close();
        }
        if (event.key.code == sf::Keyboard::Space)
        {
            gboard.randomize();
        }
    }
    if (event.type == sf::Event::MouseButtonPressed){
        if (event.mouseButton.button == sf::Mouse::Left){
            posClickedMouse.x = event.mouseButton.x;
            posClickedMouse.y = event.mouseButton.y;
        }else if(event.mouseButton.button == sf::Mouse::Right){
            posClickedMouse.x = event.mouseButton.x;
            posClickedMouse.y = event.mouseButton.y;

        }
    }
    if (event.type == sf::Event::MouseButtonReleased){
        if (event.mouseButton.button == sf::Mouse::Left){
            proceed(posClickedMouse.x,posClickedMouse.y,
                    event.mouseButton.x,event.mouseButton.y);
        }else if(event.mouseButton.button == sf::Mouse::Right){
            if( gboard.getCloserCell(posClickedMouse.x,posClickedMouse.y)
               == gboard.getCloserCell(event.mouseButton.x,event.mouseButton.y))
            {
                gboard.free(idPlayer,gboard.getCloserCell(posClickedMouse.x,
                                                posClickedMouse.y));
            }else{
                changeLinkState(posClickedMouse.x,posClickedMouse.y,
                            event.mouseButton.x,event.mouseButton.y);
               }
        }
    }
}

void Game::proceed(int orgX, int orgY, int destX, int destY)
{
    int idOrg = gboard.getCloserCell(orgX,orgY);
    if(idOrg >= 0 && gboard.getOwner(idOrg) == idPlayer){
        int idDest = gboard.getCloserCell(destX,destY);
        gboard.link(idOrg,idDest);
    }
}

void Game::changeLinkState(int orgX, int orgY, int destX, int destY)
{
    int idOrg = gboard.getCloserCell(orgX,orgY);
    if(idOrg >= 0 && gboard.getOwner(idOrg) == idPlayer){
        int idDest = gboard.getCloserCell(destX,destY);
        if(idDest >= 0){
            gboard.changeLinkState(idOrg,idDest);
        }
    }
}

Game::~Game()
{
    //dtor
}
