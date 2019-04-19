#include "tower.h"
#include "turret.h"
#include "main.h"
#include "explosion.h"

#include <sp2/random.h>


Tower::Tower(sp::P<sp::Node> parent)
: GameEntity(parent)
{
    render_data.type = sp::RenderData::Type::Normal;
    render_data.shader = sp::Shader::get("internal:basic.shader");
    render_data.order = RenderOrder::objects;
    
    turret = new Turret(this);
}

void Tower::setAI(sp::string script_name)
{
    script.setup(this, "ai/" + script_name + ".lua");
}

void Tower::takeDamage(int amount, sp::P<GameEntity> from)
{
    hp -= amount;
    if (hp <= 0)
    {
        new Explosion(getParent(), getPosition2D());
        delete this;
    }
}

void Tower::onFixedUpdate()
{
    script.update();
    turret->setRotation(turret_angle);
}

void Tower::rotate(float amount)
{
    turret_angle += amount;
}

void Tower::fire()
{
    turret->fire();
}

double Tower::getTurretRotation()
{
    return turret->getGlobalRotation2D();
}

void Tower::onRegisterScriptBindings(sp::ScriptBindingClass& script_binding_class)
{
    GameEntity::onRegisterScriptBindings(script_binding_class);
    script_binding_class.bind("fire", &Tower::fire);
    script_binding_class.bind("rotate", &Tower::rotate);
    script_binding_class.bind("getTurretRotation", &Tower::getTurretRotation);
}
