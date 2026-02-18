#pragma once

#include <utility>
#include "vec.h"
#include "graphics.h"
#include "physics.h"

class World;
class FSM;

class GameObject {
public:
    GameObject(const Vec<float>& position, const Vec<float>& size, World& world, FSM* fsm, Color color);
    ~GameObject();

    void input(World& world);
    void update(World& world, double dt);

    std::pair<Vec<float>, Color> get_sprite() const;

    // GameObject data
    Vec<float> size;
    Physics physics;
    FSM* fsm;
    Color color;
};