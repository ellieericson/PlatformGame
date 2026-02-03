#include "world.h"
#include <SDL3/SDL_rect.h>
#include <algorithm>

void World::add_platform(float x, float y, float width, float height) {
    SDL_FRect rect{x, y, width, height};
    platforms.push_back(rect);
}

const std::vector<SDL_FRect>& World::get_platforms() const {
    return platforms;
}

bool World::has_any_collisions(const SDL_FRect& box) const {
    return std::any_of(std::begin(platforms), std::end(platforms),
        [&](const SDL_FRect& platform){return SDL_HasRectIntersectionFloat(&platform, &box);});
}

Player* World::create_player() {
    player = std::make_unique<Player>(Vec<float>{600, 300}, Vec<float>{64, 64});
    return player.get();
}

void World::update(float dt) {
    // currently only updating player
    auto position = player->position;
    auto velocity = player->velocity;
    auto acceleration = player->acceleration;

    velocity += 0.5f * acceleration * dt;
    position += velocity * dt;
    velocity += 0.5f * acceleration * dt;

    SDL_FRect future{position.x, position.y, player->size.x, player->size.y};

    // check for collisions in the world
    if (!has_any_collisions(future)) {
        player->position.x = future.x;
        player->position.y = future.y;
        player->velocity = velocity;
    }
}

