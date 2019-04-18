#ifndef TURRET_H
#define TURRET_H

#include "gameEntity.h"

class TurretFlash;
class Turret : public GameEntity
{
public:
    Turret(sp::P<sp::Node> parent);

    void fire();
    
    sp::P<TurretFlash> flash;
private:
};

#endif//TURRET_H
