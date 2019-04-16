#include <sp2/engine.h>
#include <sp2/window.h>
#include <sp2/logging.h>
#include <sp2/random.h>
#include <sp2/io/directoryResourceProvider.h>
#include <sp2/graphics/gui/scene.h>
#include <sp2/graphics/gui/theme.h>
#include <sp2/graphics/gui/loader.h>
#include <sp2/graphics/scene/graphicslayer.h>
#include <sp2/graphics/scene/basicnoderenderpass.h>
#include <sp2/graphics/scene/collisionrenderpass.h>
#include <sp2/graphics/meshData.h>
#include <sp2/graphics/textureManager.h>
#include <sp2/collision/2d/circle.h>
#include <sp2/collision/2d/box.h>
#include <sp2/scene/scene.h>
#include <sp2/scene/node.h>
#include <sp2/scene/camera.h>
#include <sp2/scene/tilemap.h>
#include <sp2/io/keybinding.h>
#include <sp2/updatable.h>

#include "main.h"

/** ideas
enemies:
    tanks
    soldiers
    turrets (+ power supply)
        bullet
        laser
    planes
        bullets
        bombs
        
*/



sp::P<sp::Window> window;

sp::io::Keybinding escape_key{"exit", "Escape"};
sp::io::Keybinding up{"UP", "up"};
sp::io::Keybinding down{"DOWN", "down"};
sp::io::Keybinding left{"LEFT", "left"};
sp::io::Keybinding right{"RIGHT", "right"};

class Turret : public sp::Node
{
public:
    Turret(sp::P<sp::Node> parent)
    : sp::Node(parent)
    {
        render_data.type = sp::RenderData::Type::Normal;
        render_data.mesh = sp::MeshData::createQuad(sp::Vector2f(0.5, 0.25));
        render_data.shader = sp::Shader::get("internal:basic.shader");
        render_data.texture = sp::texture_manager.get("turrets/specialBarrel1_outline.png");
        render_data.order = getParent()->render_data.order + 1;
    }
    
    virtual void onFixedUpdate()
    {
        setRotation(-getParent()->getRotation2D());
    }
};


class Tank : public sp::Node
{
public:
    Tank(sp::P<sp::Node> parent)
    : sp::Node(parent)
    {
        render_data.type = sp::RenderData::Type::Normal;
        render_data.mesh = sp::MeshData::createQuad(sp::Vector2f(1, 1));
        render_data.shader = sp::Shader::get("internal:basic.shader");
        render_data.texture = sp::texture_manager.get("tankBody_dark_outline.png");
        render_data.order = RenderOrder::objects;
        
        setCollisionShape(sp::collision::Box2D(1, 1));
        
        (new Turret(this))->setPosition(sp::Vector2d(0.2, 0));
    }
    
    virtual void onFixedUpdate()
    {
        sp::Vector2d move_request(right.getValue() - left.getValue(), up.getValue() - down.getValue());
        double current_rotation = getRotation2D();
        double engine_request = 0.0;
        double rotate_request = 0.0;
        sp::Vector2d forward = sp::Vector2d(1, 0).rotate(current_rotation);

        if (move_request.x || move_request.y)
        {
            double target_rotation = move_request.angle();
            double a = sp::angleDifference(current_rotation, target_rotation);
            if (std::abs(a) > 90)//If we need to rotate more then 90deg, we best drive in reverse.
                a = sp::angleDifference(180.0, a);
            
            engine_request = std::min(1.0, std::max(-1.0, forward.dot(move_request) * 1.5));
            rotate_request = std::min(1.0, std::max(-1.0, a));
            if (rotate_request && std::abs(engine_request) < 0.2)
                engine_request = 0.2;
        }
        sp::Vector2d velocity;
        velocity += forward * engine_request * engine_speed;
        setLinearVelocity(velocity);
        setAngularVelocity(rotate_request * rotation_speed);
    }

private:
    double engine_speed = 3.0;
    double rotation_speed = 150.0;
};


int main(int argc, char** argv)
{
    sp::P<sp::Engine> engine = new sp::Engine();

    //Create resource providers, so we can load things.
    new sp::io::DirectoryResourceProvider("resources");

    //Disable or enable smooth filtering by default, enabling it gives nice smooth looks, but disabling it gives a more pixel art look.
    sp::texture_manager.setDefaultSmoothFiltering(false);

    //Create a window to render on, and our engine.
    window = new sp::Window(4.0/3.0);
    window->setPosition(sp::Vector2f(0.5, 0.5));
    //window->setFullScreen(true);

    sp::gui::Theme::loadTheme("default", "gui/theme/basic.theme.txt");
    new sp::gui::Scene(sp::Vector2d(640, 480), sp::gui::Scene::Direction::Horizontal);

    sp::P<sp::SceneGraphicsLayer> scene_layer = new sp::SceneGraphicsLayer(1);
    scene_layer->addRenderPass(new sp::BasicNodeRenderPass());
    scene_layer->addRenderPass(new sp::CollisionRenderPass());
    window->addLayer(scene_layer);

    sp::Scene* scene = new sp::Scene("TEST");
    sp::Camera* camera = new sp::Camera(scene->getRoot());
    camera->setOrtographic(10.0);
    scene->setDefaultCamera(camera);
    
    sp::Tilemap* tilemap = new sp::Tilemap(scene->getRoot(), "terrainTiles_default.png", 1.0, 1.0, 10, 4);
    for(int x=0;x<20;x++)
        for(int y=0;y<20;y++)
            tilemap->setTile(x, y, sp::random(0, 100) < 50 ? 0 : 10);
    tilemap->setPosition(sp::Vector2d(-10, -10));
    
    new Tank(scene->getRoot());
    
    engine->run();

    return 0;
}
