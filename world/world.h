#pragma once

#include <memory>
#include <SDL3/SDL.h>
#include "game_object.h"
#include "tilemap.h"
#include "vec.h"
#include "keyboard_input.h"

class Player;
class Level;
class Audio;
class Event;

class World {
public:
    World(const Level& level, Audio& audio, GameObject* player, std::map<std::string, Event*> events);
    void add_platform(float x, float y, float width, float height);
    bool collides(const Vec<float>& position);
    void move_to(Vec<float>& position, const Vec<float>& size, Vec<float>& velocity);
    void update(float dt);
    void load_level(const Level& level);

    Tilemap tilemap;
    bool end_level{false};

private:
    GameObject* player;
    Audio* audio;
    std::map<std::string, Event*> events;

    void touch_tiles(GameObject& obj);
};