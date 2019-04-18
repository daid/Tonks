#include "explosion.h"
#include "main.h"


Explosion::Explosion(sp::P<sp::Node> parent, sp::Vector2d position)
: sp::Node(parent)
{
    render_data.type = sp::RenderData::Type::Normal;
    render_data.shader = sp::Shader::get("internal:basic.shader");
    render_data.order = RenderOrder::effects;

    setPosition(position);

    setupTexture(this, "effect/explosion1.png", false);
}

void Explosion::onFixedUpdate()
{
    idx++;
    if (idx < 12)
        setupTexture(this, "effect/explosion" + sp::string(idx / 2) + ".png", false);
    else
        delete this;
}
