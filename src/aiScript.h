#ifndef AI_SCRIPT_H
#define AI_SCRIPT_H

#include <sp2/script/environment.h>


class AIScript
{
public:
    void setup(sp::P<sp::script::BindingObject> object, sp::string script_name);
    void update();
private:
    sp::string script_name;
    
    sp::script::Environment script;
    sp::script::CoroutinePtr update_coroutine;
    
    std::chrono::system_clock::time_point resource_update_time;
};

#endif//AI_SCRIPT_H
