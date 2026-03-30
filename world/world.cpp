#include "world.h"
#include <SDL3/SDL_rect.h>
#include <algorithm>
#include "physics.h"
#include "states.h"
#include "fsm.h"
#include "vec.h"
#include "game_object.h"
#include "level.h"
#include "audio.h"
#include "events.h"

World::World(const Level& level, Audio& audio, GameObject* player, std::map<std::string, Event*> events)
    : tilemap{level.width, level.height}, player{player}, audio{&audio}, events{events} {
    load_level(level);
}

void World::add_platform(float x, float y, float width, float height) {
    for (int i{0}; i < height; ++i) {
        for (int j{0}; j < width; ++j) {
            tilemap(x+j, y+i) = Tile{};
        }
    }
}

bool World::collides(const Vec<float>& position) {
    int x = std::floor(position.x);
    int y = std::floor(position.y);
    return tilemap(x, y).blocking;
}

void World::move_to(Vec<float>& position, const Vec<float>& size, Vec<float>& velocity) {
    // test for collisions on the bottom or top first

    if (collides(position) && collides({position.x + size.x, position.y})) {
        position.y = std::ceil(position.y);
        velocity.y = 0;
    }
    else if (collides({position.x, position.y + size.y}) && collides({position.x + size.x, position.y + size.y})) {
        position.y = std::floor(position.y);
        velocity.y = 0;
    }

    // then test for collisions on the left and right sides
    if (collides(position) && collides({position.x, position.y + size.y})) {
        position.x = std::ceil(position.x);
        velocity.x = 0;
    }
    else if (collides({position.x + size.x, position.y}) && collides({position.x + size.x, position.y + size.y})) {
        position.x = std::floor(position.x);
        velocity.x = 0;
    }

    //now test each corner
    //bottom left
    if (collides(position)) {
        float dx = std::ceil(position.x) - position.x;
        float dy = std::ceil(position.y) - position.y;
        if (dx > dy) {
            position.y = std::ceil(position.y);
            velocity.y = 0;
        }
        else {
            position.x = std::ceil(position.x);
            velocity.x = 0;
        }
    }
    //bottom right
    else if (collides({position.x + size.x, position.y})) {
        float dx = (position.x) - std::floor(position.x);
        float dy = std::ceil(position.y) - position.y;
        if (dx > dy) {
            position.y = std::ceil(position.y);
            velocity.y = 0;
        }
        else {
            position.x = std::floor(position.x);
            velocity.x = 0;
        }
    }
    //top right
    else if (collides({position.x + size.x, position.y + size.y})) {
        float dx = (position.x) - std::floor(position.x);
        float dy = (position.y) - std::floor(position.y);
        if (dx > dy) {
            position.y = std::floor(position.y);
            velocity.y = 0;
        }
        else {
            position.x = std::floor(position.x);
            velocity.x = 0;
        }
    }
    //top left
    else if (collides({position.x, position.y + size.y})) {
        float dx = std::ceil(position.x) - position.x;
        float dy = (position.y) - std::floor(position.y);
        if (dx > dy) {
            position.y = std::floor(position.y);
            velocity.y = 0;
        }
        else {
            position.x = std::ceil(position.x);
            velocity.x = 0;
        }
    }
}

void World::update(float dt) {
    // currently only updating player
    auto position = player->physics.position;
    auto velocity = player->physics.velocity;
    auto acceleration = player->physics.acceleration;

    velocity += 0.5f * acceleration * dt;
    position += velocity * dt;
    velocity += 0.5f * acceleration * dt;
    velocity.x *= player->physics.damping;

    velocity.x = std::clamp(velocity.x, -player->physics.terminal_velocity, player->physics.terminal_velocity);
    velocity.y = std::clamp(velocity.y, -player->physics.terminal_velocity, player->physics.terminal_velocity);

     //check for collisions in the world - x direction
     Vec<float> future_position{position.x, player->physics.position.y};
     Vec<float> future_veloctiy{velocity.x, 0};
     move_to(future_position, player->size, future_veloctiy);

     // now y direction after (maybe) moving in x
     future_veloctiy.y = velocity.y;
     future_position.y = position.y;
     move_to(future_position, player->size, future_veloctiy);

     //update the player position and velocity
    player->physics.position = future_position;
    player->physics.velocity = future_veloctiy;

    touch_tiles(*player);
}

void World::load_level(const Level& level) {
    for (const auto& [pos, tile_id] : level.tile_locations) {
        tilemap(pos.x, pos.y) = level.tile_types.at(tile_id);
    }
    audio->load_sounds({});
}

void World::touch_tiles(GameObject& obj) {
    int x = std::floor(obj.physics.position.x);
    int y = std::floor(obj.physics.position.y);
    const std::vector<Vec<int>> displacements{{0,0}, {static_cast<int>(obj.size.x), 0}, {0, static_cast<int>(obj.size.y)}, {static_cast<int>(obj.size.x), static_cast<int>(obj.size.y)}};
    for (const auto& displacement : displacements) {
        Tile& tile = tilemap(x+displacement.x, y + displacement.y);
        if (!tile.event_name.empty()) {
            auto itr = events.find(tile.event_name);
            if (itr == events.end()) {
                throw std::runtime_error("Cannot find event: " + tile.event_name);
            }
            itr->second->perform(*this, obj);
        }
    }
}