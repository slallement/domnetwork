#include "../include/Link.h"
#include <iostream>
#include <cmath>
#include "../include/Utils.h"

const float Link::TSCALE = 100.f;
const float Link::ARROWSIZE = 15.f;

Link::~Link()
{
}

Link::Link(Cell* origin, Cell* destination, float Strenght) :
    org(origin), dest(destination), strenght(Strenght), time(1.f), state(0),
    arrow(ARROWSIZE, 3)
{
    if(origin == 0 || destination == 0) {
        std::cout<<"error in the link creation"<<std::endl;
    }
    org=origin;
    dest=destination;
    init();
}

void Link::init()
{
    setArrow();
}

inline void Link::setArrow()
{
    sf::Vector2f dir = org->pos-dest->pos;
    dir /= (float)sqrt(dir.x*dir.x+dir.y*dir.y);
    arrow.setPosition(dest->pos+(dest->radius+ARROWSIZE)*dir);
    arrow.setFillColor(sf::Color::White);
    arrow.setOrigin(ARROWSIZE,ARROWSIZE);
    arrow.setRotation((float)atan2(dir.x,dir.y)*-180.f/PI);
}

void Link::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(arrow);
}

void Link::update(float dt)
{
    time -= strenght*dt*TSCALE;
    if(time > 0.f) return;
    if(org->units > 0) {
        if(dest->units == 0) {
            org->units--;
            dest->units++;
            dest->idOwner = org->idOwner;
        } else if(dest->idOwner == org->idOwner) {
            if(dest->units < dest->capacity) {
                org->units--;
                dest->units++;
            }
        } else {
            org->units--;
            dest->units--;
        }
        time = 1.f;
    }
}

void Link::invert()
{
    Cell* temp = org;
    org = dest;
    dest = temp;
    setArrow();
}

void Link::nextState()
{
    state++;
    if(state>=COUNT)
        state=0;

    switch(state) {
        case 0:
            strenght = 0.5f;
            arrow.setFillColor(sf::Color(255,255,255));
            break;
        case 1:
            strenght = 1.f;
            arrow.setFillColor(sf::Color(128,128,254));

            break;
        default:
        ;
    }
}

sf::Vector2f Link::getOrgPos() const
{
    return org->pos;
}
sf::Vector2f Link::getDestPos() const
{
    return dest->pos;
}

float Link::getDestRadius() const
{
    return dest->radius;
}
