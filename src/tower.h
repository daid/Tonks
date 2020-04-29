#ifndef TOWER_H
#define TOWER_H

#include "gameEntity.h"
#include "aiScript.h"


class Turret;
class Tower : public GameEntity
{
public:
    Tower(sp::P<sp::Node> parent);
    
    virtual void onFixedUpdate() override;
    void setAI(sp::string script);

    virtual void takeDamage(int amount, sp::P<GameEntity> from) override;
    
    sp::P<Turret> turret;
    int hp = 10;
protected:
    void rotate(float amount);
    void fire();
    double getTurretRotation();
    
    virtual void onRegisterScriptBindings(sp::script::BindingClass& script_binding_class) override;
private:
    AIScript script;
    double turret_angle = 0;
};

#endif//TOWER_H
