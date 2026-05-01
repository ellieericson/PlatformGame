#pragma once
#include "tilemap.h"
#include "toggle.h"
#include "vec.h"
#include "physics.h"

class Graphics;
class Color;
struct Sprite;
class GameObject;

class Camera {
public:
    Camera(Graphics& graphics, float tilesize);

    void handle_input();
    void update(const Vec<float>& new_location, float dt);
    Vec<float> world_to_screen(const Vec<float>& world_positon) const;
    void set_location(const Vec<float>& new_location);

    void render(const Vec<float>& position, const Color& color, bool filled=true) const;
    void render(const Tilemap& tiles) const;
    void render(const Vec<float>& position, const Sprite& sprite) const;
    void render(const GameObject& obj) const;
    void render(const std::vector<Background>& backgrounds);

    void render_game_over();
    void render_winning_tally(const GameObject& player);

private:
    Graphics& graphics;
    float tilesize;
    Toggle grid_toggle;

    void calculate_visible_tiles();
    Vec<int> visible_min, visible_max;
    Vec<float> goal;
    Physics physics;
};