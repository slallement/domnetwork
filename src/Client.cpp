#include "../include/Client.h"
#include "../include/Utils.h"

#include <iostream>
#include <vector>

using namespace std;

Client::Client(sf::RenderWindow& win):
    window(win), gboard(&players), exitCurrentGame(false),
    posClickedMouse(-1,-1), idPlayer(0),
    connected(false), start(false), playerConnected(1)
{
    window.setTitle("Client");
    idPlayer = 0; // initialisation to 0 important
    playerMax = 1;
    players.push_back(Player(1)); // the server
}

void Client::launch()
{
    start = false;
    connected = false;
    exitCurrentGame = false;

    gboard.init();
    cout<<"Attente d'un server"<<std::endl;
    serverAdress = sf::IpAddress::None;
    sf::Thread thread(&Client::waitServer,&(*this));
    thread.launch();

    //sf::UdpSocket socket_udp;
    socket_udp.setBlocking(false);
    if (socket_udp.bind(PORT_GAME_B) != sf::Socket::Done)
    {
        // error...
    }

    menuServerList();

    if(!window.isOpen()){
        start = true;
        thread.terminate();
        return;
    }

    socket_udp.unbind();

    menuWaitingRoom();

    thread.terminate();

    if(!exitCurrentGame){
        socket.setBlocking(true);
        // initialize the 'playground'
        sf::Packet packet;
        socket.receive(packet);
        if(packet>>gboard){
            // run the game
            cout<<"Carte recue"<<endl;
            run();
        }else{
            cout<<"erreur de reception du jeu ..."<<endl;
        }
    }
}

void Client::menuServerList()
{
    sf::Packet info_server;

    sf::Text buttonManualIp;
    buttonManualIp.setFont(*FontManager::getFont("ressources/Symtext.ttf"));
    buttonManualIp.setCharacterSize(24);
    buttonManualIp.setString("Manual ip     : ");
    buttonManualIp.setPosition(10.f,10.f);

    sf::Text infoInput;
    const float POSBAR_X = 300.f;
    const float POSBAR_Y = 10.f;
    infoInput.setFont(*FontManager::getFont("ressources/Symtext.ttf"));
    infoInput.setCharacterSize(24);
    infoInput.setString("");
    infoInput.setPosition(POSBAR_X,POSBAR_Y);
    string textInput("");

    sf::Text textBar;
    textBar.setFont(*FontManager::getFont("ressources/Symtext.ttf"));
    textBar.setCharacterSize(24);
    textBar.setString("|");
    textBar.setPosition(POSBAR_X,POSBAR_Y);

    sf::Text info;
    info.setFont(*FontManager::getFont("ressources/Symtext.ttf"));
    info.setCharacterSize(24);
    info.setString("Ip adresses:");
    info.setPosition(10.f,30.f);

    sf::Text info2;
    info2.setFont(*FontManager::getFont("ressources/Symtext.ttf"));
    info2.setCharacterSize(18);
    info2.setString("<empty>");
    info2.setPosition(10.f,30.f);
    info2.move(0,40.f);

    vector<string> listServText;
    vector<sf::Uint32> listServIp;
    vector<sf::Clock> listServTimeout;
    vector<sf::Text> listServButton;

    sf::IpAddress serv_ip;
    short unsigned int serv_port;

    sf::Clock clock;

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
            }
            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Escape)
                {
                    exitCurrentGame = true;
                    connected = true;
                }
            }
            if (event.type == sf::Event::MouseButtonPressed){
                // if you click on an ip :
                if (event.mouseButton.button == sf::Mouse::Left){
                    for(unsigned int i=0;i<listServButton.size();++i){
                        if (listServButton[i].getGlobalBounds().contains(
                            window.mapPixelToCoords(
                            (sf::Mouse::getPosition(window)))) ){
                            serverAdress = sf::IpAddress(listServIp[i]);
                        }
                    }
                }
            }
            // manual ip entered :
            if (event.type == sf::Event::TextEntered)
            {
                if (event.text.unicode < 128){
                    if(event.text.unicode == '\b'){
                        if(textInput.size()>0){
                            textInput = textInput.substr(0, textInput.size()-1);
                        }
                    }else{
                        if( ((event.text.unicode >= '0' &&
                            event.text.unicode <= '9') ) &&
                            textInput.size() < 15)
                        {
                            textInput += event.text.unicode;
                            if( ( (textInput.size()+1) % 4 == 0) && textInput.size() > 0
                               && textInput.size() < 15){
                                textInput.push_back('.');
                            }
                        }else{
                            if( event.text.unicode == '.'
                               && (textInput.size()+1) % 4 > 0){
                                unsigned int a = textInput.size()%4;
                                string temp = textInput.substr(textInput.size()-a,a);
                                textInput = textInput.substr(0, textInput.size()-a);
                                for(unsigned int i=0;i<3-a;++i){
                                    textInput.push_back('0');
                                }
                                textInput += temp;
                                if(textInput.size() < 15)
                                    textInput.push_back('.');
                            }
                        }
                    }
                }
                infoInput.setString(textInput);
                textBar.setPosition(POSBAR_X+infoInput.getGlobalBounds().width,POSBAR_Y);
            }
        }

        if (socket_udp.receive(info_server, serv_ip, serv_port) != sf::Socket::Done){
            // error...
        }else{
            //sf::Uint32 serverIp;
            if(info_server>>serverStatus){
                string s = "serv: "+ttos(serv_ip);
                s += " - ";
                if(serverStatus == SERVER_READY){
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

        window.clear();
        window.draw(info);
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

void Client::menuWaitingRoom()
{
    sf::Text info;
    info.setFont(*FontManager::getFont("ressources/Symtext.ttf"));
    info.setCharacterSize(24);
    info.setPosition(10.f,10.f);

    sf::Text info2;
    info2.setFont(*FontManager::getFont("ressources/Symtext.ttf"));
    info2.setCharacterSize(18);
    info2.setPosition(10.f,10.f);
    info2.move(0,40.f);

    sf::Text info3 = info;
    info3.setPosition(300.f,200.f);
    info3.setString("Player color :");

    sf::RectangleShape colorRect(sf::Vector2f(50.f,20.f));
    colorRect.setPosition(300.f+info3.getGlobalBounds().width,200.f);
    colorRect.setFillColor(Player::DEFAULT_COLOR[idPlayer]);

    // wait for other players--------------------------
    info.setString("waiting ...");
    while(!start && !exitCurrentGame && window.isOpen()){
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed){
                exitCurrentGame = true;
                window.close();
            }
            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Escape)
                {
                    exitCurrentGame = true;
                }
            }
        }

        /*if (socket_udp.receive(info_server, serv_ip, serv_port) != sf::Socket::Done){
            // error...
        }else{
        }*/
        info2.setString("Connectes: "+ttos((int)playerConnected)+" /"+ttos((int)playerMax));

        window.clear();

        window.draw(info);
        window.draw(info2);
        window.draw(info3);
        window.draw(colorRect);

        window.display();
    }
}

void Client::waitServer(){

    // accept a new connection

    /*cout<<"Quelle adresse ?"<<endl;
    std::string adress_input;
    cin>>adress_input;
    if(adress_input == "")
        return;

    sf::IpAddress adress(adress_input);*/
    while(serverAdress == sf::IpAddress::None){

    }

    cout<<"Tentative de connexion a : "+serverAdress.toString()<<endl;

    sf::Socket::Status status = socket.connect(serverAdress, PORT_GAME_A);
    if (status != sf::Socket::Done)
    {
        // error...
    }else{
        cout<<"connecte !"<<endl;
        connected = true;
    }

    cout<<"Attente des autres joueurs..."<<endl;

    socket.setBlocking(false);
    sf::Packet packetInfo;
    start = false;
    while(!start && !exitCurrentGame){
        if(socket.receive(packetInfo) == sf::Socket::Done){
            if(packetInfo>>playerConnected>>playerMax>>serverStatus){
                if(idPlayer == 0){
                    idPlayer = playerConnected;
                    cout<<"Your ID : #"<<(int)idPlayer<<endl;
                }
            }
        }
        /*if(playerConnected == playerMax){
            cout<<"Tout le monde est pret !"<<endl;
        }*/
        if(serverStatus == SERVER_START_GAME){
            start = true;
            break;
        }
    }
    if(start && !exitCurrentGame){
        for(int i=1;i<(int)playerMax;i++){
            players.push_back(Player(1+i));
        }
    }
}

Client::~Client()
{
    //dtor
}

void Client::run()
{
    sf::Clock clock;
    socket.setBlocking(false);

    while (window.isOpen() && !exitCurrentGame)
    {
        const float dt = clock.restart().asSeconds();
        sf::Event event;
        while (window.pollEvent(event))
        {
            manageEvents(event);
        }
        manageNetwork();

        gboard.update(dt);

        window.clear();
        window.draw(gboard);
        window.display();
    }
}

void Client::manageNetwork(){
    if(!connected)
        return;
    sf::Packet packet;
    if(socket.receive(packet)==sf::Socket::Disconnected){
        cout<<"Erreur de connexion !"<<endl;
        connected = false;
    }
    sf::Uint8 code;
    if(packet>>code){
        networkActions(packet,code);
    }
}

void Client::networkActions(sf::Packet& packet, sf::Uint8 code)
{
        if(code == LINK){
            sf::Uint16 org;
            sf::Uint16 dest;
            packet>>org>>dest;
            if(org>=0 && dest>=0){
                gboard.link(org,dest);
            }
        }else if(code == CHANGE){
            sf::Uint16 org;
            sf::Uint16 dest;
            packet>>org>>dest;
            if(org>=0 && dest>=0){
                gboard.changeLinkState(org,dest);
            }
        }else if(code == FREE){
            sf::Uint8 nidPlayer;
            sf::Uint16 org;
            packet>>nidPlayer>>org;
            gboard.free(nidPlayer, org);
        }
}

inline void Client::manageEvents(sf::Event event)
{
    if (event.type == sf::Event::Closed)
        window.close();

    if (event.type == sf::Event::KeyPressed)
    {
        if (event.key.code == sf::Keyboard::Escape)
        {
            exitCurrentGame = true;
        }
    }
    if (event.type == sf::Event::MouseButtonPressed){
        if (event.mouseButton.button == sf::Mouse::Left){
            posClickedMouse =
                window.mapPixelToCoords(sf::Mouse::getPosition(window));
        }else if(event.mouseButton.button == sf::Mouse::Right){
            posClickedMouse =
                window.mapPixelToCoords(sf::Mouse::getPosition(window));
        }
    }
    if(event.type == sf::Event::MouseButtonReleased){
        if(event.mouseButton.button == sf::Mouse::Left){
            actionLink(posClickedMouse.x,posClickedMouse.y,
                    window.mapPixelToCoords(sf::Mouse::getPosition(window)).x,
                    window.mapPixelToCoords(sf::Mouse::getPosition(window)).y);
        }else if(event.mouseButton.button == sf::Mouse::Right){
            if(gboard.getCloserCell(posClickedMouse.x,posClickedMouse.y)
                == gboard.getCloserCell(
                window.mapPixelToCoords(sf::Mouse::getPosition(window))) )
            {
                actionFree(idPlayer,posClickedMouse.x, posClickedMouse.y);

            }else{
                actionChangeLink(posClickedMouse.x,posClickedMouse.y,
                            window.mapPixelToCoords(sf::Mouse::getPosition(window)).x,
                            window.mapPixelToCoords(sf::Mouse::getPosition(window)).y);
            }
        }
    }
}

void Client::actionLink(int orgX, int orgY, int destX, int destY)
{
    int idOrg = gboard.getCloserCell(orgX,orgY);
    if(idOrg >= 0 && gboard.getOwner(idOrg) == idPlayer){
        int idDest = gboard.getCloserCell(destX,destY);
        gboard.link(idOrg,idDest);
        // send link
        sf::Packet packet;
        sf::Uint8 code = LINK;
        sf::Uint16 org = idOrg;
        sf::Uint16 dest = idDest;
        packet<<code<<org<<dest;
        socket.send(packet);

    }
}

void Client::actionChangeLink(int orgX, int orgY, int destX, int destY)
{
    int idOrg = gboard.getCloserCell(orgX,orgY);
    if(idOrg >= 0 && gboard.getOwner(idOrg) == idPlayer){
        int idDest = gboard.getCloserCell(destX,destY);
        if(idDest >= 0){
            gboard.changeLinkState(idOrg,idDest);

            // send link
            sf::Packet packet;
            sf::Uint8 code = CHANGE;
            sf::Uint16 org = idOrg;
            sf::Uint16 dest = idDest;
            packet<<code<<org<<dest;
            socket.send(packet);
        }
    }
}

void Client::actionFree(int mouseX, int mouseY)
{
    sf::Uint16 idCell = gboard.getCloserCell(mouseX,mouseY);
    gboard.free(idPlayer,idCell);
    sf::Packet packet;
    sf::Uint8 code = FREE;
    sf::Uint16 org = idCell;
    packet<<code<<idPlayer<<org;
    socket.send(packet);
}

void Client::actionFree(sf::Uint8 nidPlayer, int mouseX, int mouseY)
{
    sf::Uint16 idCell = gboard.getCloserCell(mouseX,mouseY);
    gboard.free(nidPlayer,idCell);
    sf::Packet packet;
    sf::Uint8 code = FREE;
    sf::Uint16 org = idCell;
    packet<<code<<nidPlayer<<org;
    socket.send(packet);
}
