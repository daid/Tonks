#ifndef GAME_ENTITY_H
#define GAME_ENTITY_H

#include <sp2/scene/node.h>

class GameEntity : public sp::Node
{
public:
    GameEntity(sp::P<sp::Node> parent);
    
    bool canHurt(sp::P<GameEntity> other);
    
    int team = -1;
};

#endif//GAME_ENTITY_H
