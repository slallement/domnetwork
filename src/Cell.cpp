#include "../include/Cell.h"

const float Cell::TSCALE = 1.f/10.f;

Cell::Cell(float cellradius, sf::Vector2f position, float cellcapacity) :
    radius(cellradius), pos(position), capacity(cellcapacity),
    idOwner(0), units(0), time(1.f)
{
}

Cell::Cell(float cellradius,float px, float py, float cellcapacity) :
    radius(cellradius), pos(px,py), capacity(cellcapacity),
    idOwner(0), units(0), time(1.f)
{
}

Cell::~Cell()
{
    //dtor
}

void Cell::update(float dt)
{
    time -= dt*radius*TSCALE;
    if(time < 0.f){
        units++;
        if(units > capacity)
            units = capacity;
        time = 1.f;
    }
}

// ---

void Cell::setOwner(int ownerId)
{
    idOwner = ownerId;
}

int Cell::getOwner() const{ return idOwner; }
