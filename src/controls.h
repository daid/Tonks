#ifndef CONTROLS_H
#define CONTROLS_H

#include <sp2/io/keybinding.h>

class Controls
{
public:
    Controls(int index);

    int index;
    
    sp::io::Keybinding up;
    sp::io::Keybinding down;
    sp::io::Keybinding left;
    sp::io::Keybinding right;

    sp::io::Keybinding turn_turret_left;
    sp::io::Keybinding fire;
    sp::io::Keybinding turn_turret_right;
    sp::io::Keybinding unknown3;
    sp::io::Keybinding unknown4;
    sp::io::Keybinding unknown5;
    
    sp::io::Keybinding start;
};

#endif//CONTROLS_H
