#include "gameEntity.h"

GameEntity::GameEntity(sp::P<sp::Node> parent)
: sp::Node(parent)
{
}

bool GameEntity::canHurt(sp::P<GameEntity> other)
{
    if (!other)
        return false;
    return team != other->team;
}
