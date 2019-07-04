#include "controls.h"

Controls::Controls(int index)
: index(index)
, up("up" + sp::string(index))
, down("down" + sp::string(index))
, left("left" + sp::string(index))
, right("right" + sp::string(index))
, turn_turret_left("turn_turret_left_" + sp::string(index))
, fire("fire_" + sp::string(index))
, turn_turret_right("turn_turret_right_" + sp::string(index))
, unknown3("unknown3_" + sp::string(index))
, unknown4("unknown4_" + sp::string(index))
, unknown5("unknown5_" + sp::string(index))
, start("start" + sp::string(index))
{
    if (index == 0)
    {
        up.setKeys({"up", "keypad 8"});
        down.setKeys({"down", "keypad 2"});
        left.setKeys({"left", "keypad 4"});
        right.setKeys({"right", "keypad 6"});
        
        turn_turret_left.setKey("space");
        fire.setKey("z");
        turn_turret_right.setKey("x");
        unknown3.setKey("c");
        unknown4.setKey("v");
        unknown5.setKey("b");
        
        start.setKey("1");
    }

    if (index == 1)
    {
        up.setKey("w");
        down.setKey("s");
        left.setKey("a");
        right.setKey("d");
        
        turn_turret_left.setKey("q");
        fire.setKey("e");
        turn_turret_right.setKey("r");
        unknown3.setKey("f");
        unknown4.setKey("t");
        unknown5.setKey("g");
        
        start.setKey("2");
    }
}
