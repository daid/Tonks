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
#include <sp2/graphics/textureAtlas.h>
#include <sp2/collision/2d/circle.h>
#include <sp2/collision/2d/box.h>
#include <sp2/scene/scene.h>
#include <sp2/scene/node.h>
#include <sp2/scene/camera.h>
#include <sp2/scene/tilemap.h>
#include <sp2/io/keybinding.h>
#include <sp2/io/keyValueTreeLoader.h>
#include <sp2/updatable.h>

#include "main.h"
#include "playerTank.h"

Controls controls[2]{{0}, {1}};
std::map<sp::string, std::map<sp::string, sp::string>> object_config;

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

sp::AtlasManager atlas{sp::Vector2i(2048, 2048), 1};

sp::Vector2d setupTexture(sp::P<sp::Node> node, sp::string texture, bool turret_offset)
{
    sp::AtlasManager::Result result = atlas.get(texture);

    sp::MeshData::Vertices vertices;
    sp::MeshData::Indices indices{0,1,2,2,1,3};
    vertices.reserve(4);
        
    sp::Vector2f size = result.rect.size * 16.0f;
    sp::Vector2f uv0 = result.rect.position;
    sp::Vector2f uv1 = uv0 + result.rect.size;
    sp::Vector2f offset;
    if (turret_offset)
        offset.x = (size.x-size.y) * 0.8;
    vertices.emplace_back(sp::Vector3f(offset.x-size.x, offset.y-size.y, 0.0f), sp::Vector2f(uv0.x, uv1.y));
    vertices.emplace_back(sp::Vector3f(offset.x+size.x, offset.y-size.y, 0.0f), sp::Vector2f(uv1.x, uv1.y));
    vertices.emplace_back(sp::Vector3f(offset.x-size.x, offset.y+size.y, 0.0f), sp::Vector2f(uv0.x, uv0.y));
    vertices.emplace_back(sp::Vector3f(offset.x+size.x, offset.y+size.y, 0.0f), sp::Vector2f(uv1.x, uv0.y));

    node->render_data.mesh = std::make_shared<sp::MeshData>(std::move(vertices), std::move(indices));
    node->render_data.texture = result.texture;
    return sp::Vector2d(result.rect.size.x * 32.0f, result.rect.size.y * 32.0f);
}

class TurretFlash : public sp::Node
{
public:
    TurretFlash(sp::P<sp::Node> parent)
    : sp::Node(parent)
    {
        render_data.type = sp::RenderData::Type::None;
        render_data.shader = sp::Shader::get("internal:basic.shader");
        render_data.order = RenderOrder::effects;
    }
    
    void show()
    {
        render_data.type = sp::RenderData::Type::Additive;
        delay = 5;
    }
    
    virtual void onFixedUpdate()
    {
        if (delay)
        {
            delay--;
            if (!delay)
                render_data.type = sp::RenderData::Type::None;
        }
    }

private:
    int delay = 0;
};

class Explosion : public sp::Node
{
public:
    Explosion(sp::P<sp::Node> parent, sp::Vector2d position)
    : sp::Node(parent)
    {
        render_data.type = sp::RenderData::Type::Normal;
        render_data.shader = sp::Shader::get("internal:basic.shader");
        render_data.order = RenderOrder::effects;

        setPosition(position);

        setupTexture(this, "effect/explosion1.png", false);
    }
    
    virtual void onFixedUpdate()
    {
        idx++;
        if (idx < 12)
            setupTexture(this, "effect/explosion" + sp::string(idx / 2) + ".png", false);
        else
            delete this;
    }
private:
    int idx = 1;
};

class Bullet : public sp::Node
{
public:
    Bullet(sp::P<sp::Node> parent)
    : sp::Node(parent)
    {
        render_data.type = sp::RenderData::Type::Normal;
        render_data.shader = sp::Shader::get("internal:basic.shader");
        render_data.order = RenderOrder::projectile;
        
        sp::collision::Circle2D shape(0.15);
        shape.type = sp::collision::Shape::Type::Sensor;
        setCollisionShape(shape);
        
        setupTexture(this, "bullet/bulletDark2_outline.png", false);
    }
    
    virtual void onFixedUpdate()
    {
        setPosition(getPosition2D() + sp::Vector2d(0.2, 0).rotate(getRotation2D()));
        delay--;
        if (!delay)
            delete this;
    }

    virtual void onCollision(sp::CollisionInfo& info)
    {
        new Explosion(getParent(), getPosition2D());
    
        delete this;
    }
private:
    int delay = 200;
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
    
    object_config = sp::io::KeyValueTreeLoader::load("objects.txt")->getFlattenNodesByIds();

    PlayerTank* tank = new PlayerTank(scene->getRoot(), controls[0]);
    sp::Vector2d size = setupTexture(tank, object_config["RED_TANK"]["body"], false);
    tank->setCollisionShape(sp::collision::Box2D(size.x, size.y));

    size = setupTexture(tank->turret, object_config["RED_TANK"]["turret"], true);
    tank->turret->flash->setPosition(sp::Vector2d(size.x, 0));
    setupTexture(tank->turret->flash, object_config["RED_TANK"]["flash"], true);
    
    engine->run();

    return 0;
}
