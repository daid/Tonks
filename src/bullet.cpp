#include "bullet.h"
#include "explosion.h"
#include "main.h"

#include <sp2/collision/2d/circle.h>


Bullet::Bullet(sp::P<sp::Node> parent)
: GameEntity(parent)
{
    render_data.type = sp::RenderData::Type::Normal;
    render_data.shader = sp::Shader::get("internal:basic.shader");
    render_data.order = RenderOrder::projectile;
    
    sp::collision::Circle2D shape(0.2);
    shape.type = sp::collision::Shape::Type::Sensor;
    shape.setFilterCategory(CollisionCategory::projectile);
    shape.setMaskFilterCategory(CollisionCategory::projectile);
    setCollisionShape(shape);
    
    setupTexture(this, "bullet/bulletDark2_outline.png", false);
}

void Bullet::onFixedUpdate()
{
    setLinearVelocity(sp::Vector2d(10.0, 0).rotate(getRotation2D()));
    delay--;
    if (!delay)
        delete this;
}

void Bullet::onCollision(sp::CollisionInfo& info)
{
    if (canHurt(info.other))
    {
        sp::P<GameEntity> other = info.other;
        if (other)
            other->takeDamage(1, this);
        new Explosion(getParent(), getPosition2D());

        delete this;
    }
}
