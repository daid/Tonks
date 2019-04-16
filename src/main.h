#ifndef MAIN_H
#define MAIN_H

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

#endif//MAIN_H
