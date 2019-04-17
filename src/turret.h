#ifndef TURRET_H
#define TURRET_H

#include <sp2/scene/node.h>

class TurretFlash;
class Turret : public sp::Node
{
public:
    Turret(sp::P<sp::Node> parent);

    void fire();
    
    sp::P<TurretFlash> flash;
private:
};

#endif//TURRET_H