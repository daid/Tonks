#ifndef TURRET_FLASH_H
#define TURRET_FLASH_H

#include <sp2/scene/node.h>

class TurretFlash : public sp::Node
{
public:
    TurretFlash(sp::P<sp::Node> parent);
    
    void show();
    
    virtual void onFixedUpdate() override;
private:
    int delay = 0;
};

#endif//TURRET_FLASH_H
