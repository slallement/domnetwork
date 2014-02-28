#include "../include/ClientMenuServerList.h"
#include "../include/Client.h"
#include <string>
using namespace std;

MenuServerList::MenuServerList(sf::RenderWindow& win, sf::UdpSocket& sock_udp,
                                bool& n_connected, sf::IpAddress& ip_serv,
                                sf::Uint8& servStatus):
    Menu(win), socket_udp(sock_udp), connected(n_connected),
    serverAddress(ip_serv), serverStatus(servStatus)
{


    textManualIp.setFont(*FontManager::getFont("ressources/Symtext.ttf"));
    textManualIp.setCharacterSize(24);
    textManualIp.setString("Manual ip  : ");
    textManualIp.setPosition(10.f,FIRST_LINE_Y);

    buttonManualIp.setFont(*FontManager::getFont("ressources/Symtext.ttf"));
    buttonManualIp.setCharacterSize(24);
    buttonManualIp.setString("Try connection");
    buttonManualIp.setPosition(window.getSize().x
                               -buttonManualIp.getGlobalBounds().width-20.f, FIRST_LINE_Y);

    POSBAR_X = textManualIp.getGlobalBounds().width
                            + textManualIp.getGlobalBounds().left + 20.f;

    infoInput.setFont(*FontManager::getFont("ressources/Symtext.ttf"));
    infoInput.setCharacterSize(24);
    infoInput.setString("");
    infoInput.setPosition(POSBAR_X,POSBAR_Y);
    string textInput("");
    sf::IpAddress directIp;

    textBar.setFont(*FontManager::getFont("ressources/Symtext.ttf"));
    textBar.setCharacterSize(24);
    textBar.setString("|");
    textBar.setPosition(POSBAR_X,POSBAR_Y);

    const float SECOND_LINE_Y = FIRST_LINE_Y+30.f;
    info.setFont(*FontManager::getFont("ressources/Symtext.ttf"));
    info.setCharacterSize(24);
    info.setString("Ip adresses ---------------------------");
    info.setPosition(10.f,SECOND_LINE_Y);

    info2.setFont(*FontManager::getFont("ressources/Symtext.ttf"));
    info2.setCharacterSize(18);
    info2.setString("<empty>");
    info2.setPosition(10.f,SECOND_LINE_Y);
    info2.move(0,SECOND_LINE_Y);


    window.setFramerateLimit(60);
}

MenuServerList::~MenuServerList()
{
}

void MenuServerList::run()
{
    bool exitCurrentGame = false;
    string textInput("");
    while(!connected && !exitCurrentGame && window.isOpen()){
        if(clock.getElapsedTime().asMilliseconds() > 1000){
            clock.restart();
        }
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed){
                window.close();
                connected = true;
            }else
            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Escape)
                {
                    exitCurrentGame = true;
                    connected = true;
                }
            }else
            if (event.type == sf::Event::MouseButtonPressed){
                // if you click on an ip :
                if (event.mouseButton.button == sf::Mouse::Left){
                    for(unsigned int i=0;i<listServButton.size();++i){
                        if (listServButton[i].getGlobalBounds().contains(
                            window.mapPixelToCoords(
                            (sf::Mouse::getPosition(window)))) )
                        {
                            serverAddress = sf::IpAddress(listServIp[i]);
                        }
                    }

                    if(buttonManualIp.getGlobalBounds().contains(
                            window.mapPixelToCoords(
                            (sf::Mouse::getPosition(window))))
                            && directIp != sf::IpAddress::None)
                    {
                        serverAddress = directIp;
                    }
                }
            }else
            // manual ip entered :
            if (event.type == sf::Event::TextEntered)
            {
                if (event.text.unicode < 128){
                    if(event.text.unicode == '\b'){
                        if(textInput.size()>0){
                            textInput = textInput.substr(0, textInput.size()-1);
                        }
                    }else
                    if(textInput.size() < 15){
                        if( ((event.text.unicode >= '0' &&
                            event.text.unicode <= '9') ) )
                        {
                            textInput += event.text.unicode;
                            if( ( (textInput.size()+1) % 4 == 0)
                                && textInput.size() > 0
                                && textInput.size() < 11){
                                textInput.push_back('.');
                            }
                        }else{
                            if( event.text.unicode == '.'
                                && (textInput.size()+1) % 4 > 0 )
                            {
                                unsigned int a = textInput.size()%4;
                                string temp = textInput.substr(textInput.size()-a,a);
                                textInput = textInput.substr(0, textInput.size()-a);
                                for(unsigned int i=0;i<3-a;++i){
                                    textInput.push_back('0');
                                }
                                textInput += temp;
                                if(textInput.size() <= 11)
                                    textInput.push_back('.');
                            }
                        }
                    }

                    infoInput.setString(textInput);
                    if(textInput.size() >= 12)
                        directIp = sf::IpAddress(textInput);
                    textBar.setPosition(POSBAR_X+infoInput.getGlobalBounds().width,POSBAR_Y);
                }
            }
        }

        if (socket_udp.receive(info_server, serv_ip, serv_port) != sf::Socket::Done){
            // error...
        }else{
            //sf::Uint32 serverIp;
            if(info_server>>serverStatus){
                string s = "serv: "+ttos(serv_ip);
                s += " - ";
                if(serverStatus == Client::SERVER_READY){
                    s += "ready !";
                }else{
                    s += "not ready";
                }
                //s += " - ";
                //s += "ip: "+sf::IpAddress(serverIp).toString();
                s += "\n";
                    //info2.setString(s);
                // serch if the server info are in the lists
                bool founded = false;
                for(unsigned int i=0;i<listServIp.size();++i){
                    if(listServIp[i] == serv_ip.toInteger()){
                        founded = true;
                        listServText[i]=s;
                        listServButton[i].setString(listServText[i]);
                        listServTimeout[i].restart();
                        break;
                    }
                }
                // correct the lists if not founded
                if(!founded){
                    listServIp.push_back(serv_ip.toInteger());
                    listServText.push_back(s);
                    listServButton.push_back(info2);
                    listServButton.back().setString(listServText.back());
                    listServTimeout.push_back(sf::Clock());
                }

            }
        }
        for(unsigned int i=0;i<listServIp.size();++i){
            if(listServTimeout[i].getElapsedTime().asSeconds() > 1.f){
                serv_ip = sf::IpAddress(listServIp[i]);
                string s = "serv: "+serv_ip.toString()+" - < Disconnected >";
                listServText[i]=s;
                listServButton[i].setString(listServText[i]);
            }
            if (listServButton[i].getGlobalBounds().contains(
                window.mapPixelToCoords(
                (sf::Mouse::getPosition(window)))) ){
                listServButton[i].setColor(sf::Color(255,255,200,128));
            }else{
                listServButton[i].setColor(sf::Color(255,255,255,255));
            }
        }
        if(buttonManualIp.getGlobalBounds().contains(window.mapPixelToCoords(
                (sf::Mouse::getPosition(window))))
                && directIp != sf::IpAddress::None)
        {
            buttonManualIp.setColor(sf::Color(200,200,255,200));
        }else{
            buttonManualIp.setColor(sf::Color(255,255,255,255));
        }

        window.clear();
        window.draw(info);
        window.draw(textManualIp);
        window.draw(buttonManualIp);
        window.draw(infoInput);

        if(clock.getElapsedTime().asMilliseconds() > 500){
            window.draw(textBar);
        }
        if(listServIp.size()<=0){
            window.draw(info2);
        }else{
            for(unsigned int i=0;i<listServIp.size();++i){
                window.draw(listServButton[i]);
            }
        }
        window.display();
    }
}

