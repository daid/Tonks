#include "aiTank.h"
#include "turret.h"
#include "explosion.h"


AITank::AITank(sp::P<sp::Node> parent)
: TankBase(parent)
{
}

TankBase::Input AITank::getInput()
{
    script.update();

    Input result = input;
    input = Input();
    return result;
}

void AITank::setAI(sp::string script_name)
{
    script.setup(this, "ai/" + script_name + ".lua");
}

void AITank::takeDamage(int amount, sp::P<GameEntity> from)
{
    hp -= amount;
    if (hp <= 0)
    {
        new Explosion(getParent(), getPosition2D());
        delete this;
    }
}

void AITank::move(sp::Vector2d amount)
{
    input.move_request = amount;
}

void AITank::rotate(float amount)
{
    input.turret_rotate_request = amount;
}

void AITank::fire()
{
    input.fire = true;
}

double AITank::getTurretRotation()
{
    return turret->getGlobalRotation2D();
}

void AITank::onRegisterScriptBindings(sp::ScriptBindingClass& script_binding_class)
{
    GameEntity::onRegisterScriptBindings(script_binding_class);
    script_binding_class.bind("move", &AITank::move);
    script_binding_class.bind("fire", &AITank::fire);
    script_binding_class.bind("rotate", &AITank::rotate);
    script_binding_class.bind("getTurretRotation", &AITank::getTurretRotation);
}
