#pragma once

#include <memory>
#include <SDL3/SDL.h>
#include "game_object.h"
#include "tilemap.h"
#include "vec.h"
#include "keyboard_input.h"
#include "quadtree.h"

class Player;
class Level;
class Audio;
class Event;

class World {
public:
    World(const Level& level, Audio& audio, GameObject* player, std::map<std::string, Event*> events);
    ~World();
    void add_platform(float x, float y, float width, float height);
    bool collides(const Vec<float>& position);
    void move_to(Vec<float>& position, const Vec<float>& size, Vec<float>& velocity);
    void update(float dt);
    void load_level(const Level& level);

    Tilemap tilemap;
    bool end_level{false};
    bool end_game{false};

    void build_quadtree();

    GameObject* player;
    Audio* audio;
    std::vector<GameObject*> game_objects;
    std::map<std::string, Event*> events;
    std::vector<Background> backgrounds;

    void touch_tiles(GameObject& obj);

    QuadTree quadtree;
};