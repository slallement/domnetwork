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
    serverAddress = sf::IpAddress::None;
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
    MenuServerList menu(window, socket_udp, connected, serverAddress, serverStatus);
    menu.run();
    return;
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
    while(serverAddress == sf::IpAddress::None){
        sf::sleep(sf::milliseconds(1000));
    }

    cout<<"Tentative de connexion a : "+serverAddress.toString()<<endl;

    sf::Socket::Status status = socket.connect(serverAddress, PORT_GAME_A);
    if (status != sf::Socket::Done)
    {
        // error...
        cout<<"Impossible de se connecter ..."<<endl;
        serverAddress = sf::IpAddress::None;
        waitServer();
        return;
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

    while (window.isOpen() && !exitCurrentGame && connected)
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
    socket.disconnect();
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
        }else if(code == SYNCHRONIZE_GBOARD){
            packet>>gboard;
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
