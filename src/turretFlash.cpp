#include "turretFlash.h"
#include "main.h"


TurretFlash::TurretFlash(sp::P<sp::Node> parent)
: sp::Node(parent)
{
    render_data.type = sp::RenderData::Type::None;
    render_data.shader = sp::Shader::get("internal:basic.shader");
    render_data.order = RenderOrder::effects;
}

void TurretFlash::show()
{
    render_data.type = sp::RenderData::Type::Additive;
    delay = 5;
}

void TurretFlash::onFixedUpdate()
{
    if (delay)
    {
        delay--;
        if (!delay)
            render_data.type = sp::RenderData::Type::None;
    }
}
