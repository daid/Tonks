#include "threadDecal.h"
#include "main.h"


ThreadDecal::ThreadDecal(sp::P<sp::Node> parent, sp::Vector2d position, double rotation)
: sp::Node(parent)
{
    render_data.type = sp::RenderData::Type::Additive;
    render_data.shader = sp::Shader::get("internal:basic.shader");
    render_data.order = RenderOrder::tracks;

    setPosition(position);
    setRotation(rotation);

    setupTexture(this, "effect/tracksSmall.png", false);
}

void ThreadDecal::onFixedUpdate()
{
    render_data.color.a -= 0.001;
    if (render_data.color.a <= 0.0)
        delete this;
}
