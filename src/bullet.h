#ifndef BULLET_H
#define BULLET_H

#include "gameEntity.h"


class Bullet : public GameEntity
{
public:
    Bullet(sp::P<sp::Node> parent);
    
    virtual void onFixedUpdate() override;

    virtual void onCollision(sp::CollisionInfo& info) override;
private:
    int delay = 200;
};


#endif//BULLET_H
