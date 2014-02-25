#include "../include/MainMenu.h"

#include <iostream>
#include "../include/Server.h"
#include "../include/Client.h"

MainMenu::MainMenu(sf::RenderWindow & win):
    Menu(win), fullscreenMode(false)
{
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    window.create(sf::VideoMode(800,600), "Dom Network", sf::Style::Default, settings);

    /*sf::Image icon;
    if(icon.loadFromFile("ressources/icon.png"))
        window.setIcon(256,256,icon.getPixelsPtr());*/
    window.setFramerateLimit(60);

    title = sf::Text("DomNetwork",*FontManager::getFont("ressources/Symtext.ttf"),46);
    title.setColor(sf::Color(100,100,255));
    centerTextOnxAxis(title, 50.f);

    sf::Text t = sf::Text("Server",*FontManager::getFont("ressources/Symtext.ttf"),32);
    option.resize(4);
    option[0].txt = t;
    option[0].f = static_cast<void(Menu::*)()>(&MainMenu::server);

    t.setString("Client");
    option[1].txt = t;
    option[1].f = static_cast<void(Menu::*)()>(&MainMenu::client);

    t.setString("Mode");
    option[2].txt = t;
    option[2].txt.setString("Mode : windowed");
    option[2].f = static_cast<void(Menu::*)()>(&MainMenu::mode);

    t.setString("Exit");
    option[3].txt = t;
    option[3].f = &Menu::exit;

    recomputePos();
}

MainMenu::~MainMenu()
{
    //dtor
}

void MainMenu::recomputePos()
{
    for(unsigned int i=0;i<4; ++i){
        centerTextOnxAxis(option[i].txt,150.f+i*100.f);
    }
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

void MainMenu::draw()
{
    window.draw(title);
}

void MainMenu::mode()
{
    /* list video mode
    std::vector<sf::VideoMode> Modes = sf::VideoMode::getFullscreenModes();
    for (unsigned i=0; i<Modes.size(); i++)
        std::cout << Modes[i].width << 'x' << Modes[i].height << ' ' << Modes[i].bitsPerPixel << std::endl;*/

    //window = sf::RenderWindow(window(sf::VideoMode(1600, 900), "Dodger King",sf::Style::Fullscreen);
    //sf::VideoMode videomode = sf::VideoMode(1680, 1050);

    if(!fullscreenMode){
        sf::VideoMode videomode = sf::VideoMode(1600, 900);
        if(videomode.isValid()){
            window.create(videomode, "Dom Network",sf::Style::Fullscreen);
            option[2].txt.setString("Mode : fullscreen");
        }
        fullscreenMode = true;
    }else{
        sf::VideoMode videomode = sf::VideoMode(800, 600);
        window.create(videomode, "Dom network");
        option[2].txt.setString("Mode : windowed");
        fullscreenMode = false;
    }
    recomputePos();
}


