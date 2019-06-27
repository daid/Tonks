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
#include <sp2/graphics/meshdata.h>
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
#include <json11/json11.hpp>

#include "main.h"
#include "aiTank.h"
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
        sp::collision::Box2D shape(size.x * 0.9, size.y * 0.9);
        shape.setFilterCategory(CollisionCategory::player);
        tank->setCollisionShape(shape);
        tank->team = 0;
        tank->turret->team = tank->team;
        tank->engine_speed = sp::stringutil::convert::toFloat(data["engine_speed"]);
        tank->rotation_speed = sp::stringutil::convert::toFloat(data["rotation_speed"]);

        size = setupTexture(tank->turret, data["turret"], true);
        tank->turret->flash->setPosition(sp::Vector2d(size.x, 0));
        setupTexture(tank->turret->flash, data["flash"], true);
        player_tanks[index] = tank;
        return tank;
    }
    if (data["type"] == "tank")
    {
        AITank* tank = new AITank(scene->getRoot());
        sp::Vector2d size = setupTexture(tank, data["body"], false);
        sp::collision::Box2D shape(size.x * 0.9, size.y * 0.9);
        shape.setFilterCategory(CollisionCategory::other);
        tank->setCollisionShape(shape);
        tank->team = 1;
        tank->turret->team = tank->team;
        tank->setAI(data["ai"]);
        tank->engine_speed = sp::stringutil::convert::toFloat(data["engine_speed"]);
        tank->rotation_speed = sp::stringutil::convert::toFloat(data["rotation_speed"]);
        tank->hp = sp::stringutil::convert::toInt(data["hp"]);

        size = setupTexture(tank->turret, data["turret"], true);
        tank->turret->flash->setPosition(sp::Vector2d(size.x, 0));
        setupTexture(tank->turret->flash, data["flash"], true);
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
        if (sp::stringutil::convert::toBool(data["mask_projectiles"]))
            shape.setMaskFilterCategory(CollisionCategory::projectile);
        entity->setCollisionShape(shape);
        
        entity->team = -1;
        return entity;
    }
    if (data["type"] == "tree")
    {
        GameEntity* entity = new GameEntity(scene->getRoot());

        entity->render_data.type = sp::RenderData::Type::Normal;
        entity->render_data.shader = sp::Shader::get("internal:basic.shader");
        entity->render_data.order = RenderOrder::trees;
        sp::Vector2d size = setupTexture(entity, data["image"], false);
        
        sp::collision::Circle2D shape(size.x / 10.0);
        shape.type = sp::collision::Shape::Type::Static;
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
        tower->hp = sp::stringutil::convert::toInt(data["hp"]);

        size = setupTexture(tower->turret, data["turret"], true);
        tower->turret->flash->setPosition(sp::Vector2d(size.x, 0));
        setupTexture(tower->turret->flash, data["flash"], true);
        return tower;
    }
    LOG(Error, "Unknown object type:", data["type"]);
    return nullptr;
}

class GameScene : public sp::Scene
{
public:
    GameScene()
    : sp::Scene("GAME")
    {
    }
    
    void loadLevel(sp::string name)
    {
        for(sp::P<sp::Node> n : getRoot()->getChildren())
            n.destroy();
        
        sp::Camera* camera = new sp::Camera(getRoot());
        camera->setOrtographic(10.0);
        setDefaultCamera(camera);
        
        sp::Tilemap* tilemap = new sp::Tilemap(getRoot(), "towerDefense_tilesheet.png", 2.0, 2.0, 19, 12);
        tilemap->render_data.order = RenderOrder::tilemap;

        sp::string err;
        json11::Json map_json = json11::Json::parse(sp::io::ResourceProvider::get("level/" + name + ".json")->readAll(), err);
        map_size.x = map_json["width"].int_value();
        map_size.y = map_json["height"].int_value();
        for(const auto& layer : map_json["layers"].array_items())
        {
            if (layer["type"].string_value() == "tilelayer")
            {
                const json11::Json& tile_data_json = layer["data"];
                for (int y = 0; y < map_size.y; y++)
                    for (int x = 0; x < map_size.x; x++)
                        tilemap->setTile(x, y, tile_data_json[x + (map_size.y - 1 - y) * map_size.x].int_value() - 1);
            }
            if (layer["type"].string_value() == "objectgroup")
            {
                for(const auto& json_object : layer["objects"].array_items())
                {
                    ObjectInfo info;
                    info.position = sp::Vector2d(json_object["x"].number_value() / 32.0, map_size.y * 2.0 - json_object["y"].number_value() / 32.0);
                    info.rotation = sp::stringutil::convert::toFloat(json_object["type"].string_value());
                    info.name = json_object["name"].string_value();
                    objects.push_back(info);
                }
            }
            std::sort(objects.begin(), objects.end(), [](const ObjectInfo& a, const ObjectInfo& b)
            {
                return a.position.y < b.position.y;
            });
        }
        
        {
            sp::P<sp::Node> node = new sp::Node(getRoot());
            sp::collision::Box2D shape(2.0, 22.0, -11.0, 0);
            shape.type = sp::collision::Shape::Type::Kinematic;
            node->setCollisionShape(shape);
            player_borders.add(node);
        }
        {
            sp::P<sp::Node> node = new sp::Node(getRoot());
            sp::collision::Box2D shape(2.0, 22.0, 11.0, 0);
            shape.type = sp::collision::Shape::Type::Kinematic;
            node->setCollisionShape(shape);
            player_borders.add(node);
        }
        {
            sp::P<sp::Node> node = new sp::Node(getRoot());
            sp::collision::Box2D shape(22.0, 5.0, 0.0, -12.5);
            shape.type = sp::collision::Shape::Type::Kinematic;
            shape.setMaskFilterCategory(CollisionCategory::other);
            node->setCollisionShape(shape);
            player_borders.add(node);
        }
        {
            sp::P<sp::Node> node = new sp::Node(getRoot());
            sp::collision::Box2D shape(22.0, 5.0, 0.0, 12.5);
            shape.type = sp::collision::Shape::Type::Kinematic;
            shape.setMaskFilterCategory(CollisionCategory::other);
            node->setCollisionShape(shape);
            player_borders.add(node);
        }
    }

    virtual void onUpdate(float delta)
    {
        sp::Vector2d camera_position = getCamera()->getPosition2D();
        camera_position.x = 10.0;
        camera_position.y = std::max(camera_position.y, 10.0);
        for(int n=0; n<2; n++)
        {
            if (player_tanks[n])
                camera_position.y = std::max(camera_position.y, std::min(camera_position.y + 0.1, player_tanks[n]->getPosition2D().y + 2.0));
        }
        if (scroll_lock)
        {
            camera_position.y = std::min(scroll_lock - 10.0, camera_position.y);
            bool keep_lock = false;
            for(sp::P<GameEntity> ge : getRoot()->getChildren())
            {
                if (ge && ge->team > 0)
                {
                    keep_lock = true;
                    break;
                }
            }
            if (!keep_lock)
                scroll_lock = 0.0;
        }
        for(int n=0; n<2; n++)
            if (player_tanks[n] && player_tanks[n]->getPosition2D().y < camera_position.y - 10)
                    player_tanks[n]->setPosition(sp::Vector2d(player_tanks[n]->getPosition2D().x, camera_position.y - 10));
        getCamera()->setPosition(camera_position);
        
        while(objects.size() > 0 && objects.front().position.y < camera_position.y + 12.0)
        {
            if (objects.front().name == "SCROLL_LOCK")
            {
                scroll_lock = objects.front().position.y;
            }
            else
            {
                sp::P<sp::Node> node = createObject(this, objects.front().name);
                if (node)
                {
                    node->setPosition(objects.front().position);
                    node->setRotation(objects.front().rotation);
                }
            }
            objects.erase(objects.begin());
        }
        for(auto node : player_borders)
        {
            node->setPosition(camera_position);
        }
    }
private:
    class ObjectInfo
    {
    public:
        sp::string name;
        sp::Vector2d position;
        double rotation;
    };
    
    double scroll_lock = 0.0;
    sp::Vector2i map_size;
    std::vector<ObjectInfo> objects;
    sp::PList<sp::Node> player_borders;
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

    object_config = sp::io::KeyValueTreeLoader::load("objects.txt")->getFlattenNodesByIds();

    GameScene* scene = new GameScene();
    scene->loadLevel("1");

    engine->run();

    return 0;
}
