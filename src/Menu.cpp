#include "../include/Menu.h"

#include <iostream>

Menu::Menu(sf::RenderWindow & win):
    window(win), end(false), fullscreenMode(false)
{
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    window.create(sf::VideoMode(800,600), "Dom Network", sf::Style::Default, settings);
    window.setFramerateLimit(60);
}

Menu::~Menu()
{
    //dtor
}

void Menu::centerTextOnxAxis(sf::Text &txt, float y)
{
    txt.setPosition((window.getSize().x-txt.getGlobalBounds().width)/2.f,y);
}

void Menu::action()
{

}

void Menu::draw()
{

}

void Menu::run()
{
    sf::Clock elapsed;
    sf::Clock clock;
    float dt;
    while (window.isOpen() && !end)
    {
        dt = clock.restart().asSeconds();

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed){
                if (event.key.code == sf::Keyboard::Escape){
                    end = true;
                }
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    clic = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                }
            }else if(event.type == sf::Event::MouseButtonReleased) {
                for(unsigned int i=0; i<option.size();++i){
                    if(option[i].txt.getGlobalBounds().contains(
                            window.mapPixelToCoords(
                            (sf::Mouse::getPosition(window))) )
                    && option[i].txt.getGlobalBounds().contains(
                           clic) )
                    {
                        if(option[i].f != NULL){
                            (this->*option[i].f)();
                        }
                    }
                }
            }

        }
        action();
        window.clear();

        for(unsigned int i=0; i<option.size();++i){
            if(option[i].txt.getGlobalBounds().contains(
                        window.mapPixelToCoords(
                        (sf::Mouse::getPosition(window)))) )
            {
                option[i].txt.setColor(sf::Color(200,200,255,200));
            }else{
                option[i].txt.setColor(sf::Color(255,255,255,255));
            }
            window.draw(option[i].txt);
        }
        draw();

        window.display();
    }
}
