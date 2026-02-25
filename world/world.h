#pragma once

#include <memory>
#include <SDL3/SDL.h>
#include "game_object.h"
#include "tilemap.h"
#include "vec.h"
#include "keyboard_input.h"

class Player;

class World {
public:
    World(int width, int height);
    void add_platform(float x, float y, float width, float height);
    bool collides(const Vec<float>& position);
    void move_to(Vec<float>& position, const Vec<float>& size, Vec<float>& velocity);
    GameObject* create_player(World& world);
    void update(float dt);

    Tilemap tilemap;

private:
    std::unique_ptr<GameObject> player;
};