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

void GameEntity::onRegisterScriptBindings(sp::ScriptBindingClass& script_binding_class)
{
    script_binding_class.bind("getPosition", &sp::Node::getPosition2D);
    script_binding_class.bind("getRotation", &sp::Node::getRotation2D);
}
