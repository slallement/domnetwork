#include "../include/MainMenu.h"

#include <iostream>
#include "../include/Server.h"
#include "../include/Client.h"

MainMenu::MainMenu(sf::RenderWindow & win):
    window(win), end(false), fullscreenMode(false)
{
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    window.create(sf::VideoMode(800,600), "Dom Network", sf::Style::Default, settings);

    /*sf::Image icon;
    if(icon.loadFromFile("ressources/icon.png"))
        window.setIcon(256,256,icon.getPixelsPtr());*/
    window.setFramerateLimit(60);

    sf::Text t = sf::Text("Server",*FontManager::getFont("ressources/Symtext.ttf"),32);
    centerTextOnxAxis(t,100.f);
    option.resize(4);
    option[0].txt = t;
    option[0].f = &MainMenu::server;

    t.setString("Client");
    centerTextOnxAxis(t,200.f);
    option[1].txt = t;
    option[1].f = &MainMenu::client;

    t.setString("Mode");
    option[2].txt = t;
    option[2].txt.setString("Mode : windowed");
    centerTextOnxAxis(option[2].txt,300.f);
    option[2].f = &MainMenu::mode;

    t.setString("Exit");
    centerTextOnxAxis(t,400.f);
    option[3].txt = t;
    option[3].f = &MainMenu::exit;
}

MainMenu::~MainMenu()
{
    //dtor
}

void MainMenu::centerTextOnxAxis(sf::Text &txt, float y)
{
    txt.setPosition((window.getSize().x-txt.getGlobalBounds().width)/2.f,y);
}

void MainMenu::server()
{
    Server s(window);
    s.launch();
}

void MainMenu::client()
{
    Client s(window);
    s.launch();
}

void MainMenu::mode()
{
    /* list video mode
    std::vector<sf::VideoMode> Modes = sf::VideoMode::getFullscreenModes();
    for (unsigned i=0; i<Modes.size(); i++)
        std::cout << Modes[i].width << 'x' << Modes[i].height << ' ' << Modes[i].bitsPerPixel << std::endl;*/

    //window = sf::RenderWindow(window(sf::VideoMode(1600, 900), "Dodger King",sf::Style::Fullscreen);
    sf::VideoMode videomode = sf::VideoMode(1680, 1050);
    if(!fullscreenMode){
        if(videomode.isValid()){
            window.create(videomode, "Dom Network",sf::Style::Fullscreen);
            option[2].txt.setString("Mode : fullscreen");
            centerTextOnxAxis(option[2].txt,400.f);
        }
        fullscreenMode = true;
    }else{
        window.create(videomode, "Dodger King");
        option[2].txt.setString("Mode : windowed");
        centerTextOnxAxis(option[2].txt,400.f);
        fullscreenMode = false;
    }
}

void MainMenu::exit()
{
    end = true;
}

void MainMenu::run()
{
    sf::Clock elapsed;
    sf::Clock clock;float dt;
    while (window.isOpen() && !end)
    {
        dt = clock.restart().asSeconds();

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed){
                if (event.key.code == sf::Keyboard::Escape){
                    exit();
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

        window.display();
    }
}
