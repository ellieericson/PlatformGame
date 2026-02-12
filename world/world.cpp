#include "world.h"
#include <SDL3/SDL_rect.h>
#include <algorithm>
#include "physics.h"

World::World(int width, int height)
    : tilemap{width, height} {

}


void World::add_platform(float x, float y, float width, float height) {
    for (int i{0}; i < height; ++i) {
        for (int j{0}; j < width; ++j) {
            tilemap(x+j, y+i) = Tile::Platform;
        }
    }
}

bool World::collides(const Vec<float>& position) {
    int x = std::floor(position.x);
    int y = std::floor(position.y);
    return tilemap(x, y) == Tile::Platform;
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
    if (collides({position.x + size.x, position.y})) {
        float dx = (position.x + size.x) - std::floor(position.x + size.x);
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
    if (collides({position.x + size.x, position.y + size.y})) {
        float dx = (position.x + size.x) - std::floor(position.x + size.x);
        float dy = (position.y + size.y) - std::floor(position.y + size.y);
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
    if (collides({position.x, position.y + size.y})) {
        float dx = std::ceil(position.x) - position.x;
        float dy = (position.y + size.y) - std::floor(position.y + size.y);
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


GameObject* World::create_player(World& world) {
    player = std::make_unique<GameObject>(Vec<float>{10, 5}, Vec<float>{1.0, 1.0}, world);
    return player.get();
}

void World::update(float dt) {
    // ... the code to update veloctiy and position
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
}

