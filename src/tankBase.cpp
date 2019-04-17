#include "tankBase.h"
#include "turret.h"
#include "main.h"


TankBase::TankBase(sp::P<sp::Node> parent)
: sp::Node(parent)
{
    render_data.type = sp::RenderData::Type::Normal;
    render_data.shader = sp::Shader::get("internal:basic.shader");
    render_data.order = RenderOrder::objects;
    
    turret = new Turret(this);
}

void TankBase::onFixedUpdate()
{
    sp::Vector2d move_request;//(controls.right.getValue() - controls.left.getValue(), controls.up.getValue() - controls.down.getValue());
    double current_rotation = getRotation2D();
    double engine_request = 0.0;
    double rotate_request = 0.0;
    sp::Vector2d forward = sp::Vector2d(1, 0).rotate(current_rotation);

    if (move_request.x || move_request.y)
    {
        double target_rotation = move_request.angle();
        double a = sp::angleDifference(current_rotation, target_rotation);
        if (std::abs(a) > 120)//If we need to rotate too much, we best drive in reverse.
            a = sp::angleDifference(180.0, a);
        
        engine_request = std::min(1.0, std::max(-1.0, forward.dot(move_request) * 1.5));
        rotate_request = std::min(1.0, std::max(-1.0, a * 10.0 / rotation_speed));
        if (rotate_request && std::abs(engine_request) < 0.2)
            engine_request = 0.2;
    }
    sp::Vector2d velocity;
    velocity += forward * engine_request * engine_speed;
    setLinearVelocity(velocity);
    setAngularVelocity(rotate_request * rotation_speed);

    if (controls.turn_turret_left.getDown())
        turret_angle += 45;
    if (controls.turn_turret_right.getDown())
        turret_angle -= 45;
    if (controls.fire.getDown())
        turret->fire();
    
    turret->setRotation(-current_rotation + turret_angle);
}
