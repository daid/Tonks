#ifndef MAIN_H
#define MAIN_H

#include "controls.h"

class PlayerTank;
extern Controls controls[2];
extern std::map<sp::string, std::map<sp::string, sp::string>> object_config;
extern sp::P<PlayerTank> player_tanks[2];

//Rendering orders
class RenderOrder
{
public:
    static constexpr int tilemap = -10;
    static constexpr int tracks = -5;
    static constexpr int decals = -4;
    static constexpr int objects = 0;
    static constexpr int projectile = 5;
    static constexpr int effects = 10;
    static constexpr int trees = 15;
    static constexpr int flying_objects = 20;
};

sp::Vector2d setupTexture(sp::P<sp::Node> node, sp::string texture, bool turret_offset);

#endif//MAIN_H
