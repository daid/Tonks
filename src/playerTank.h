#ifndef PLAYER_TANK_H
#define PLAYER_TANK_H

#include "tankBase.h"
#include "controls.h"

class PlayerTank : public TankBase
{
public:
    PlayerTank(sp::P<sp::Node> parent, Controls& controls);

    virtual Input getInput() override;
private:
    Controls& controls;
};

#endif//PLAYER_TANK_H
