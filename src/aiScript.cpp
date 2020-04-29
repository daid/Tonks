#include "aiScript.h"
#include "playerTank.h"
#include "main.h"

#include <sp2/random.h>
#include <thread>

static int luaYield(lua_State* lua)
{
    return lua_yield(lua, 0);
}

static sp::P<PlayerTank> luaGetPlayer(int index)
{
    if (index < 0)
        return nullptr;
    if (index > 1)
        return nullptr;
    return player_tanks[index];
}


void AIScript::setup(sp::P<sp::script::BindingObject> object, sp::string script_name)
{
    script.setGlobal("random", sp::random);
    script.setGlobal("irandom", sp::irandom);
    script.setGlobal("yield", luaYield);
    script.setGlobal("self", object);
    script.setGlobal("getPlayer", luaGetPlayer);
    this->script_name = script_name;
    script.load(script_name);
    resource_update_time = sp::io::ResourceProvider::getModifyTime(script_name);
}

void AIScript::update()
{
    if (sp::io::ResourceProvider::getModifyTime(script_name) != resource_update_time)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        update_coroutine = nullptr;
        script.load(script_name);
        resource_update_time = sp::io::ResourceProvider::getModifyTime(script_name);
    }

    if (!update_coroutine || !update_coroutine->resume())
        update_coroutine = script.callCoroutine("update");
}
