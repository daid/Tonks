#ifndef AI_TANK_H
#define AI_TANK_H

#include "tankBase.h"
#include "aiScript.h"

class AITank : public TankBase
{
public:
    AITank(sp::P<sp::Node> parent);

    virtual Input getInput() override;
    
    void setAI(sp::string script);

    virtual void takeDamage(int amount, sp::P<GameEntity> from) override;
    
    int hp = 15;
protected:
    void move(sp::Vector2d amount);
    void rotate(float amount);
    void fire();
    double getTurretRotation();
    
    virtual void onRegisterScriptBindings(sp::ScriptBindingClass& script_binding_class) override;
private:
    Input input;
    AIScript script;
};

#endif//PLAYER_TANK_H
