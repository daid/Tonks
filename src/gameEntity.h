#ifndef GAME_ENTITY_H
#define GAME_ENTITY_H

#include <sp2/scene/node.h>

class GameEntity : public sp::Node
{
public:
    GameEntity(sp::P<sp::Node> parent);
    
    bool canHurt(sp::P<GameEntity> other);
    virtual void takeDamage(int amount, sp::P<GameEntity> from) {}
    
    int team = -1;
};

#endif//GAME_ENTITY_H
