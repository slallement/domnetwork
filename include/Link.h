#ifndef LINK_H
#define LINK_H

#include <SFML/Graphics.hpp>
#include "Cell.h"

class Link: public sf::Drawable
{
    public:
        Link(Cell* origin, Cell* destination, float Strenght = 1.f);
        virtual ~Link();
        sf::Vector2f getOrgPos() const;
        sf::Vector2f getDestPos() const;
        float getDestRadius() const;
        void init();
        void invert();

        void update(float dt);
        void nextState();
        enum States {NORMAL,FULL,COUNT};

    protected:
        void setArrow();

        Cell* org;
        Cell* dest;
        float strenght;
        float time;
        int state;
        static const float TSCALE;
        static const float ARROWSIZE;
        sf::CircleShape arrow;
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    private:
        friend class Gameboard;
};

#endif // LINK_H
