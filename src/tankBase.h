#ifndef TANK_BASE_H
#define TANK_BASE_H

#include "gameEntity.h"


class Turret;
class TankBase : public GameEntity
{
public:
    TankBase(sp::P<sp::Node> parent);
    
    virtual void onFixedUpdate() override;

    sp::P<Turret> turret;
    double engine_speed = 5.0;
    double rotation_speed = 250.0;
protected:
    class Input
    {
    public:
        sp::Vector2d move_request;
        double turret_rotate_request;
        bool fire;
    };
    
    virtual Input getInput() = 0;
private:
    double turret_angle = 90;
    
    sp::Vector2d last_position;
};

#endif//TANK_BASE_H
