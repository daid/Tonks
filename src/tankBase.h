#ifndef TANK_BASE_H
#define TANK_BASE_H

#include <sp2/scene/node.h>

class Turret;
class TankBase : public sp::Node
{
public:
    TankBase(sp::P<sp::Node> parent);
    
    virtual void onFixedUpdate() override;

    sp::P<Turret> turret;
private:
    double turret_angle = 90;
    double engine_speed = 5.0;
    double rotation_speed = 250.0;
};

#endif//TANK_BASE_H
