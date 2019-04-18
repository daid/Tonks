#include "turret.h"
#include "turretFlash.h"
#include "bullet.h"

#include <sp2/scene/scene.h>


Turret::Turret(sp::P<sp::Node> parent)
: GameEntity(parent)
{
    render_data.type = sp::RenderData::Type::Normal;
    render_data.shader = sp::Shader::get("internal:basic.shader");
    render_data.order = getParent()->render_data.order + 1;
    
    flash = new TurretFlash(this);
}

void Turret::fire()
{
    flash->show();
    Bullet* bullet = new Bullet(getScene()->getRoot());
    bullet->setPosition(getGlobalPoint2D(sp::Vector2d(0.8, 0)));
    bullet->setRotation(getGlobalRotation2D());
    bullet->team = team;
}
