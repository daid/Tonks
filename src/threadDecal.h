#ifndef THREAD_DECAL_H
#define THREAD_DECAL_H

#include <sp2/scene/node.h>

class ThreadDecal : public sp::Node
{
public:
    ThreadDecal(sp::P<sp::Node> parent, sp::Vector2d position, double rotation);
    
    virtual void onFixedUpdate() override;
private:
    int idx = 0;
};

#endif//THREAD_DECAL_H
