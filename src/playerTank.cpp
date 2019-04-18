#include "playerTank.h"

PlayerTank::PlayerTank(sp::P<sp::Node> parent, Controls& controls)
: TankBase(parent), controls(controls)
{
}

TankBase::Input PlayerTank::getInput()
{
    Input input;
    input.move_request.x = controls.right.getValue() - controls.left.getValue();
    input.move_request.y = controls.up.getValue() - controls.down.getValue();
    input.turret_rotate_request = 0;

    if (controls.turn_turret_left.getDown())
        input.turret_rotate_request = 45;
    if (controls.turn_turret_right.getDown())
        input.turret_rotate_request = -45;
    input.fire = controls.fire.getDown();

    return input;
}
