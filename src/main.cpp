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
#include "turret.h"
#include "turretFlash.h"
#include "tower.h"

Controls controls[2]{{0}, {1}};
std::map<sp::string, std::map<sp::string, sp::string>> object_config;
sp::P<PlayerTank> player_tanks[2];

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
    if (result.rect.size.x < 0)
        LOG(Warning, "Failed to find", texture);

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

sp::P<sp::Node> createObject(sp::P<sp::Scene> scene, sp::string name)
{
    std::map<sp::string, sp::string> data = object_config[name];
    
    if (data["type"] == "playertank")
    {
        int index = sp::stringutil::convert::toInt(data["player_index"]);
        PlayerTank* tank = new PlayerTank(scene->getRoot(), controls[index]);
        sp::Vector2d size = setupTexture(tank, data["body"], false);
        tank->setCollisionShape(sp::collision::Box2D(size.x * 0.9, size.y * 0.9));
        tank->team = 0;
        tank->turret->team = tank->team;

        size = setupTexture(tank->turret, data["turret"], true);
        tank->turret->flash->setPosition(sp::Vector2d(size.x, 0));
        setupTexture(tank->turret->flash, data["flash"], true);
        player_tanks[index] = tank;
        return tank;
    }
    if (data["type"] == "smallobject")
    {
        GameEntity* entity = new GameEntity(scene->getRoot());

        entity->render_data.type = sp::RenderData::Type::Normal;
        entity->render_data.shader = sp::Shader::get("internal:basic.shader");
        entity->render_data.order = RenderOrder::objects;
        sp::Vector2d size = setupTexture(entity, data["image"], false);
        
        sp::collision::Box2D shape(size.x, size.y);
        shape.linear_damping = 10.0;
        shape.angular_damping = 10.0;
        shape.density = sp::stringutil::convert::toFloat(data["density"]);
        entity->setCollisionShape(shape);
        
        entity->team = -1;
        return entity;
    }
    if (data["type"] == "tower")
    {
        Tower* tower = new Tower(scene->getRoot());
        sp::Vector2d size = setupTexture(tower, data["base"], false);
        sp::collision::Box2D shape(size.x * 0.9, size.y * 0.9);
        shape.type = sp::collision::Shape::Type::Static;
        tower->setCollisionShape(shape);
        tower->team = 1;
        tower->turret->team = tower->team;
        tower->setAI(data["ai"]);

        size = setupTexture(tower->turret, data["turret"], true);
        tower->turret->flash->setPosition(sp::Vector2d(size.x, 0));
        setupTexture(tower->turret->flash, data["flash"], true);
        return tower;
    }
    LOG(Error, "Unknown object type:", data["type"]);
    return nullptr;
}


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
#ifndef DEBUG
    window->setFullScreen(true);
#endif

    sp::gui::Theme::loadTheme("default", "gui/theme/basic.theme.txt");
    new sp::gui::Scene(sp::Vector2d(640, 480), sp::gui::Scene::Direction::Horizontal);

    sp::P<sp::SceneGraphicsLayer> scene_layer = new sp::SceneGraphicsLayer(1);
    scene_layer->addRenderPass(new sp::BasicNodeRenderPass());
#ifdef DEBUG
    scene_layer->addRenderPass(new sp::CollisionRenderPass());
#endif
    window->addLayer(scene_layer);

    sp::Scene* scene = new sp::Scene("TEST");
    sp::Camera* camera = new sp::Camera(scene->getRoot());
    camera->setOrtographic(10.0);
    scene->setDefaultCamera(camera);
    
    sp::Tilemap* tilemap = new sp::Tilemap(scene->getRoot(), "towerDefense_tilesheet.png", 2.0, 2.0, 19, 12);
    for(int x=0;x<10;x++)
        for(int y=0;y<10;y++)
            tilemap->setTile(x, y, sp::irandom(0, 100));
    tilemap->setPosition(sp::Vector2d(-10, -10));
    tilemap->render_data.order = RenderOrder::tilemap;
    
    object_config = sp::io::KeyValueTreeLoader::load("objects.txt")->getFlattenNodesByIds();

    createObject(scene, "PLAYER_RED_TANK");
    createObject(scene, "METAL_CRATE");
    createObject(scene, "WOOD_CRATE");
    createObject(scene, "BARICADE_METAL");
    createObject(scene, "BARICADE_WOOD");
    createObject(scene, "FENCE_RED");
    createObject(scene, "FENCE_YELLOW");
    createObject(scene, "SNIPE_TOWER");
    
    engine->run();

    return 0;
}
