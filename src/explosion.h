#ifndef EXPLOSION_H
#define EXPLOSION_H

#include <sp2/scene/node.h>

class Explosion : public sp::Node
{
public:
    Explosion(sp::P<sp::Node> parent, sp::Vector2d position);
    
    virtual void onFixedUpdate();
private:
    int idx = 1;
};

#endif//EXPLOSION_H
