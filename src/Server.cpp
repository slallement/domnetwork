#include "../include/Server.h"

#include <iostream>
#include "../include/Utils.h"

using namespace std;

Server::Server(sf::RenderWindow& win):
    Client(win)
{
    window.setTitle("Server");
    idPlayer = 1;
    addPlayer();
    addPlayer();
    playerConnected = 1;
    serverStatus = SERVER_NOT_READY;
}

void Server::addPlayer()
{
    playerMax++;
    players.push_back(Player(playerMax));
}

void Server::launch()
{
    start = false;
    connected = false;
    exitCurrentGame = false;

    gboard.init();
    cout<<"Attente d'une connexion"<<std::endl;

    // first it will show the waiting room for the server
    // the thread is used to make connection server-clients
    // the menu also send information about the server

    sf::Thread thread(&Server::waitClient,&(*this));
    thread.launch();

    socket_udp.setBlocking(false);
    menuWaitingRoom();

    // after the menu, the server won't get new connections
    thread.terminate();

    if(!window.isOpen())
        return;

    // when the menu is closed/terminated the game will start
    if(connected){
        serverStatus = SERVER_START_GAME;
        // first we send an information to all client
        // to make them wait for receiving the gameboard
        for (vector<sf::TcpSocket*>::iterator it = sockets.begin();
                it != sockets.end(); ++it)
        {
            sf::TcpSocket& client = **it;
            sf::Packet packet;
            packet<<playerConnected<<playerMax<<serverStatus;
            if (client.send(packet) == sf::Socket::Done)
            {
                cout<<"Envoie sequence de demarrage force a "<<client.getRemoteAddress().toString()<<endl;
            }
        }

        // then we send the gameboard
        sf::Packet packet;
        packet<<gboard;
        unsigned int id = 0; // used to identify differents clients
        for (vector<sf::TcpSocket*>::iterator it = sockets.begin();
                    it != sockets.end(); ++it)
        {
            sf::TcpSocket& client = **it;
            if (client.send(packet) == sf::Socket::Done)
            {
                cout<<"Carte envoye a : #"<<int(id+2)<<endl;
            }
            client.setBlocking(false);
            ++id;
        }

        // Eventually the game starts
        timeElapsed.restart();
        run();
    }else{
        // if we can't make the server
        // the game run without other players ...
        cout<<"Solo game"<<endl;
        timeElapsed.restart();
        run();
    }

    // disconnect all clients when end
    for (vector<sf::TcpSocket*>::iterator it = sockets.begin();
            it != sockets.end();
            ++it)
    {
        sf::TcpSocket& client = **it;
        client.disconnect();
    }
}

void Server::menuWaitingRoom()
{
    serverStatus = SERVER_READY;
    // information that the sever will send to make others able to connect: statut
    sf::Packet info_server;
        info_server<<serverStatus;//<< sf::IpAddress::getLocalAddress().toInteger();

    sf::RectangleShape rect(sf::Vector2f(800,600));
        rect.setFillColor(sf::Color(0,0,0,180));

    sf::Text info;
        info.setFont(*FontManager::getFont("ressources/Symtext.ttf"));
        info.setCharacterSize(24);
        info.setString("Waiting for players ... ");
        info.setOrigin(info.getLocalBounds().left+info.getGlobalBounds().width/2.f,
                       info.getGlobalBounds().height/2.f);
        info.setPosition(window.getDefaultView().getSize()/2.f);
        info.setColor(sf::Color::White);

    sf::Text info2 = info;
        info2.setString("");
        info2.setCharacterSize(12);
        info2.setPosition(200.f,20.f);

    sf::Text buttonAdd = info;
        buttonAdd.setString("Add player");
        buttonAdd.setPosition(500.f,20.f);

    sf::Text buttonRemove = info;
        buttonRemove.setString("Remove player");
        buttonRemove.setPosition(500.f,40.f);

    sf::Text button_t;
        button_t.setFont(*FontManager::getFont("ressources/Symtext.ttf"));
        button_t.setCharacterSize(24);
        button_t.setString("Start !");
        button_t.setOrigin(button_t.getLocalBounds().left+button_t.getGlobalBounds().width/2.f,
                       button_t.getGlobalBounds().height/2.f);
        button_t.setPosition(window.getDefaultView().getSize()/2.f);
        button_t.move(0,2.f*24.f);
        button_t.setColor(sf::Color::White);

    while(!start && !connected && window.isOpen() && !exitCurrentGame){

            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed){
                    window.close();
                    start = true;
                }
                if (event.type == sf::Event::KeyPressed)
                {
                    if (event.key.code == sf::Keyboard::Escape)
                    {
                        exitCurrentGame = true;
                        start = true;
                    }
                }
                if (event.type == sf::Event::MouseButtonPressed){
                    if (event.mouseButton.button == sf::Mouse::Left){
                        // the "ready" button (force the game to start)
                        if (button_t.getGlobalBounds().contains(
                            window.mapPixelToCoords(
                            (sf::Mouse::getPosition(window)))))
                        {
                            start = true;
                            connected = true;
                        }else if (buttonAdd.getGlobalBounds().contains(
                            window.mapPixelToCoords(
                            (sf::Mouse::getPosition(window)))))
                        {
                            //add
                            if(playerMax < 8){
                                addPlayer();
                                gboard.initPlayers();
                            }
                        }
                        else if (buttonRemove.getGlobalBounds().contains(
                            window.mapPixelToCoords(
                            (sf::Mouse::getPosition(window)))))
                        {
                            //remove plaeyer
                            if(playerMax >= 2){
                                playerMax--;
                                players.pop_back();
                                gboard.initPlayers();
                            }

                        }
                    }
                }
            }

            // send information on all user of the local network
            if (socket_udp.send(info_server, sf::IpAddress::Broadcast, PORT_GAME_B) != sf::Socket::Done){
                //error
                //start = true;
            }
            // if mouse under the "ready button"
            if (button_t.getGlobalBounds().contains(window.mapPixelToCoords(
                sf::Mouse::getPosition(window)))) {
                button_t.setColor(sf::Color(200,200,255,200));
            }else{
                button_t.setColor(sf::Color(255,255,255,255));
            }
            if (buttonAdd.getGlobalBounds().contains(window.mapPixelToCoords(
                sf::Mouse::getPosition(window)))) {
                buttonAdd.setColor(sf::Color(200,200,255,200));
            }else{
                buttonAdd.setColor(sf::Color(255,255,255,255));
            }
            if (buttonRemove.getGlobalBounds().contains(window.mapPixelToCoords(
                sf::Mouse::getPosition(window)))) {
                buttonRemove.setColor(sf::Color(200,200,255,200));
            }else{
                buttonRemove.setColor(sf::Color(255,255,255,255));
            }
            info2.setString("Waiting : "+ttos((int)playerConnected)
                            +" / "+ttos((int)playerMax));

            window.clear();
            window.draw(gboard);
            window.draw(rect);
            window.draw(info);

            window.draw(button_t);
            window.draw(buttonAdd);
            window.draw(buttonRemove);
            window.draw(info2);

            window.display();
    } // end of waiting room
}

void Server::waitClient(){
    serverStatus = SERVER_READY;
    // bind the listener to a port
    if (listener.listen(PORT_GAME_A) != sf::Socket::Done){
        return;
    }

    selector.add(listener);

    while(!start) {
        if (selector.wait( sf::seconds(0.1f))) {
            if (selector.isReady(listener)) {
                // check new connexions
                sf::TcpSocket* socketTemp = new sf::TcpSocket();
                if (listener.accept(*socketTemp) == sf::Socket::Done){
                    // Add the new client to the clients list
                    sockets.push_back(socketTemp);
                    selector.add(*socketTemp);
                    socketTemp->setBlocking(false);
                    playerConnected++;
                    cout<<"Nouveau client connecte"<<endl;
                    cout<<" "<<(int)playerConnected<<" / "<<(int)playerMax<<" "<<endl;
                } else {
                    delete socketTemp;
                }
             }
        }
            /*  send info to user already connected:
                the nb of users (to make the client know which idPlayer it
                will be) and the max player */
            for (vector<sf::TcpSocket*>::iterator it = sockets.begin();
                it != sockets.end(); ++it)
            {
                sf::TcpSocket& client = **it;
                if ( true )// selector.isReady(client))
                {
                    sf::Packet packet;
                    packet<<playerConnected<<playerMax<<serverStatus;
                    if (client.send(packet) == sf::Socket::Done)
                    {
                        //cout<<"envoie info a "<<client.getRemoteAddress().toString()<<endl;
                    }
                }
            }

        //}

    }

}

Server::~Server()
{
    for (vector<sf::TcpSocket*>::iterator it = sockets.begin();
            it != sockets.end();
            ++it)
    {
        delete *it;
    }
}

void Server::manageNetwork()
/* loop for each client
synchronize the current game
and send back all information received from one client to others */
{
    if(!connected)
        return;

    unsigned int i=0;
    for (vector<sf::TcpSocket*>::iterator it = sockets.begin();
            it != sockets.end();
            ++it)
    {
        sf::TcpSocket& client = **it;
        if (selector.isReady(client)){
            sf::Packet packet;

            // receive the data from a client
            if(client.receive(packet)==sf::Socket::Disconnected){
                cout<<"Deconnexion du client #"<<(i+2)<<" ! "<<endl;
                it=sockets.erase(it);
                break;
                //connected = false;
            }else{
                sf::Uint8 code;
                if(packet>>code){
                    // make the modification
                    networkActions(packet,code);

                    // send the packet to others
                    for (unsigned int j=0;j<sockets.size();j++){
                        if(j!=i){
                            sockets[j]->send(packet);
                        }
                    }

                }else{

                }
            }
        }
        i++;
    }

    // resynchronize the clients with de server each T_RESYNC sec
    if(timeElapsed.getElapsedTime().asMilliseconds() > T_RESYNC )
    {
        resyncClients();
        timeElapsed.restart();
    }
}

void Server::resyncClients()
{
    sf::Packet packet;
    packet<<(sf::Uint8)Client::SYNCHRONIZE_GBOARD<<gboard;
    for (vector<sf::TcpSocket*>::iterator it = sockets.begin();
            it != sockets.end();
            ++it)
    {
        sf::TcpSocket& client = **it;
        client.send(packet);
    }
}

void Server::actionLink(int orgX, int orgY, int destX, int destY)
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
        //socket.send(packet);
        for (vector<sf::TcpSocket*>::iterator it = sockets.begin();
                    it != sockets.end(); ++it)
        {
            (**it).send(packet);
        }
    }
}

void Server::actionChangeLink(int orgX, int orgY, int destX, int destY)
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
            //socket.send(packet);
            for (vector<sf::TcpSocket*>::iterator it = sockets.begin();
                    it != sockets.end(); ++it)
            {
                (**it).send(packet);
            }
        }
    }
}

void Server::actionFree(int mouseX, int mouseY)
{
    sf::Uint16 idCell = gboard.getCloserCell(mouseX,mouseY);
    gboard.free(idPlayer,idCell);
    sf::Packet packet;
    sf::Uint8 code = FREE;
    sf::Uint16 org = idCell;
    packet<<code<<idPlayer<<org;
    //socket.send(packet);
    for (vector<sf::TcpSocket*>::iterator it = sockets.begin();
                    it != sockets.end(); ++it)
    {
        (**it).send(packet);
    }
}

void Server::actionFree(sf::Uint8 nidPlayer, int mouseX, int mouseY)
{
    sf::Uint16 idCell = gboard.getCloserCell(mouseX,mouseY);
    gboard.free(nidPlayer,idCell);
    sf::Packet packet;
    sf::Uint8 code = FREE;
    sf::Uint16 org = idCell;
    packet<<code<<nidPlayer<<org;
    //socket.send(packet);
    for (vector<sf::TcpSocket*>::iterator it = sockets.begin();
                    it != sockets.end(); ++it)
    {
        (**it).send(packet);
    }
}

