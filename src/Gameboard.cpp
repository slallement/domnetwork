#include "../include/Gameboard.h"
#include "../include/utils.h"
#include <cstdlib>
#include <iostream>
#include <cmath>

sf::Clock Gameboard::elapsed;

Gameboard::Gameboard()
{
}

Gameboard::Gameboard(std::vector<Player>* playerList)
{
    players = playerList;
}

void Gameboard::reset()
{
    cells.clear();
}

void Gameboard::randomize()
{
    const int NB = 25;
    const float mindist = 2.f*10.f;
    const float border = 5.f;
    const int minCapacity = 25;
    const int maxCapacity = 2000;
    bool ok;
    int security = 0; // avoid infinite loop
    for(int i=0;i<NB;i++){
        ok = true;

        const float WIDTH = 800.f;
        const float HEIGHT = 600.f;
        float r = (float)rand() / (float) RAND_MAX * 90.f+20.f;
        float x = (float)rand() / (float) RAND_MAX * (WIDTH-2.f*(r+border)) + r + border;
        float y = (float)rand() / (float) RAND_MAX * (HEIGHT-2.f*(r+border)) + r + border;
        int c = rand() % (maxCapacity-minCapacity-(int)(r/2.5f))+minCapacity;
        for(unsigned int j=0;j<cells.size();j++){
            float dx = cells[j].pos.x-x;
            float dy = cells[j].pos.y-y;
            float sr = cells[j].radius + r + mindist;
            if( dx*dx+dy*dy < sr*sr )
            {
                i--;
                ok = false;
                security++;
                break;
            }
        }
        if(ok){
            cells.push_back(Cell(r,x,y,c));
            float f = ((float)rand()/RAND_MAX-0.5f)/6.f;
            freqRot.push_back( (f!=0.f?f:0.01f) );
        }
        if(security > NB*10.f){
            break;
        }
    }
}

void Gameboard::initPlayers()
{
    for(unsigned int i=0;i<cells.size();++i){
        cells[i].idOwner = 0;
    }
    for(unsigned int i=1;i<(*players).size()+1;++i){
        int c = rand() % cells.size();
        if(cells[c].idOwner == 0){
            cells[c].idOwner = i;
            cells[c].capacity += 100;
            cells[c].units += 100;
        }else
            i--;
    }
}

void Gameboard::init(){
    randomize();
    initPlayers();
}

Gameboard::~Gameboard()
{
    //dtor
}

void Gameboard::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    sf::Vector2f ratio(Utils::dot(
                                  target.getDefaultView().getSize(),
                                  sf::Vector2f(1.f/800.f,1.f/600.f)) );
    states.transform.scale(ratio);
    //target.draw(m_sprite, states);;
    for(std::deque<Link>::const_iterator it=links.begin();it!=links.end();++it){
       sf::Vertex line[] =
        {
            sf::Vertex(it->getOrgPos()),
            sf::Vertex(it->getDestPos())
        };
        line[0].color = (*players)[it->org->getOwner()-1].getColor();
        //line[1].color = (*players)[it->org.getOwner()].getColor();
        target.draw(line, 2, sf::Lines,states);
        target.draw(*it,states);
    }

    for(unsigned int i=0;i<cells.size();++i){
        sf::CircleShape cell(cells[i].radius, 8);
        cell.setOrigin(cells[i].radius,cells[i].radius);
        cell.setPosition(cells[i].pos);
        cell.setOutlineColor(sf::Color(255,255,255));
        cell.setOutlineThickness(2);
        if(cells[i].idOwner == 0){
            cell.setFillColor(Player::DEFAULT_COLOR[0]);
        }else{
            cell.setFillColor((*players)[cells[i].idOwner-1].getColor());
        }
        cell.setRotation(elapsed.getElapsedTime().asSeconds()*2.f*PI*RAD_TO_DEG
                         *freqRot[i]);

        target.draw(cell, states);

        sf::Text text;
        text.setColor(sf::Color::Black);
        text.setFont(*FontManager::getFont("ressources/Symtext.ttf"));
        text.setCharacterSize(4+cells[i].radius/8);
        text.setString(""+ttos(cells[i].units)+" / "+ttos(cells[i].capacity));
        text.setOrigin(text.getLocalBounds().left+text.getGlobalBounds().width/2.f,
                       text.getGlobalBounds().height/2.f);
        text.setPosition(cells[i].pos);

        target.draw(text,states);//,text.getTransform());
    }
}

void Gameboard::link(int idOrg, int idDest, float strength)
{
    if(idOrg < 0) return;
    if(idDest < 0) return;
    if(idOrg >= (int)cells.size()) return;
    if(idDest >= (int)cells.size()) return;
    if(idOrg == idDest) return;

    for(std::deque<Link>::iterator it=links.begin();it<links.end();++it){
        // reverse the direction of the arrow
        // or erase it if in the same way
        if(&cells[idOrg] == it->dest && &cells[idDest] == it->org){
            it->invert();
            return;
        } else if(&cells[idOrg] == it->org && &cells[idDest] == it->dest){
            links.erase(it);
            return;
        }
    }

    links.push_back(Link(&cells[idOrg],&cells[idDest],strength));
}

void Gameboard::update(float dt)
{
    for(std::vector<Cell>::iterator it=cells.begin();it!=cells.end();it++){
        it->update(dt);
    }
    for(std::deque<Link>::iterator it=links.begin();it<links.end();++it){
        it->update(dt);
    }

}

int Gameboard::getCloserCell(float x, float y)
{
    int id = 0;
    for(std::vector<Cell>::iterator it=cells.begin();it!=cells.end();it++){
        float dx = it->pos.x-x;
        float dy = it->pos.y-y;
        float sr = it->radius;
        if( dx*dx+dy*dy < sr*sr )
        {
            return id;
        }
        id++;
    }
    return -1;
}

int Gameboard::getCloserCell(sf::Vector2f coord)
{
    return getCloserCell(coord.x, coord.y);
}

void Gameboard::free(int idPlayer, int idCell)
{
    if(idCell < 0) return;
    if(cells[idCell].getOwner() != idPlayer) return;
    for(std::deque<Link>::iterator it=links.begin();it<links.end();){
        if(it->org == &cells[idCell] || it->dest == &cells[idCell]){
            it = links.erase(it);
        }else{
            ++it;
        }
    }
}

void Gameboard::changeLinkState(int idOrg, int idDest)
{
    for(std::deque<Link>::iterator it=links.begin();it<links.end();++it){
        if(it->org == &cells[idOrg] && it->dest == &cells[idDest]){
            it->nextState();
            break;
        }
    }
}

int Gameboard::getOwner(int idCell)
{
    return cells.at(idCell).idOwner;
}

sf::Packet& operator>>(sf::Packet& packet, Gameboard& gb)
{
    sf::Uint8 sz = 0;
    packet >> sz;
    gb.cells.resize(sz,Cell());
    for(unsigned int i=0;i<gb.cells.size();i++){
        packet >> gb.cells[i].radius >> gb.cells[i].pos.x >> gb.cells[i].pos.y
        >> gb.cells[i].idOwner >> gb.cells[i].capacity >> gb.cells[i].units
        >> gb.cells[i].time;
    }
    return packet;
}

sf::Packet& operator<<(sf::Packet& packet, const Gameboard& gb)
{
    sf::Uint8 sz = gb.cells.size();
    packet << sz;

    for(unsigned int i=0;i<gb.cells.size();i++){
        packet << gb.cells[i].radius << gb.cells[i].pos.x << gb.cells[i].pos.y
        << gb.cells[i].idOwner  << gb.cells[i].capacity << gb.cells[i].units
        << gb.cells[i].time;
    }
    return packet;
}
