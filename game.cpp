#include "game.h"
#include "asset_manager.h"
#include "fsm.h"
#include "states.h"
#include "level.h"

Game::Game(std::string title, int width, int height)
    : graphics{title, width, height}, camera{graphics, 64}, dt{1.0/60.0}, lag{0.0}, performance_frequency{SDL_GetPerformanceFrequency()}, prev_counter{SDL_GetPerformanceCounter()} {

    //load events
    get_events();

    //give the player its assets and put it in the correct state
    create_player();
    AssetManager::get_game_object_details("player", graphics, *player);

    //load first level
    load_level();
}

Game::~Game() {
    delete world;
    for (auto [_, event] : events) {
        delete event;
    }
}


void Game::handle_event(SDL_Event* event) {
    switch (mode) {
        case GameMode::Playing:
            player->input->collect_discrete_event(event);
            break;
    }
}

void Game::input() {
    switch (mode) {
        case GameMode::Playing:
            player->input->get_input();
            camera.handle_input();
            break;
    }
}

void Game::update() {
    Uint64 now = SDL_GetPerformanceCounter();
    lag += (now - prev_counter) / (float)performance_frequency;
    prev_counter = now;
    while (lag >= dt) {
        switch (mode) {
            case GameMode::Playing:
                player->input->handle_input(*world, *player);
                world->update(dt);
                //put the camera slightly ahead of the player
                float L = length(player->physics.velocity);
                Vec displacement = 8.0f * player->physics.velocity / (1.0f + L);
                camera.update(player->physics.position + displacement, dt);
                if (world->end_level) {
                    load_level();
                }
                if (world->end_game) {
                    mode = GameMode::GameOver;
                }
                break;
        }
        lag -= dt;
    }
}

void Game::render() {
    // clear screen
    graphics.clear();

    // render backgrounds
    camera.render(world->backgrounds);

    //draw the world
    camera.render(world->tilemap);

    //draw the player
    camera.render(*player);

    //coins
    for (auto& obj : world->game_objects) {
        camera.render(*obj);
    }

    if (mode == GameMode::GameOver) {
        camera.render_game_over();
    }
    if (current_level == 3) {
        camera.render_winning_tally(*player);
    }

    //update
    graphics.update();
}

void Game::get_events() {
    events["next_level"] = new NextLevel();
    events["drown"] = new Drown();
    events["gather"] = new Gather();
}

void Game::load_level() {
    std::string level_name = "level_" + std::to_string(++current_level);
    Level level{level_name};
    AssetManager::get_level_details(graphics, level);

    //create the world
    delete world;
    world = new World(level, audio, player.get(), events);

    // object details for coins
    for (auto& obj : world->game_objects) {
        if (obj == world->player) continue;
        AssetManager::get_game_object_details(obj->obj_name, graphics, *obj);
    }

    player->physics.position = {static_cast<float>(level.player_spawn_location.x), static_cast<float>(level.player_spawn_location.y)};
    player->fsm->current_state->on_enter(*world, *player);
    camera.set_location(player->physics.position);
    audio.play_sound("background", true);
}

void Game::create_player() {
    // Create FSM
    Transitions transitions = {
        {{StateType::Standing, Transition::Jump}, StateType::InAir},
        {{StateType::InAir, Transition::Stop}, StateType::Standing},
        {{StateType::Standing, Transition::Move}, StateType::Running},
        {{StateType::Running, Transition::Stop}, StateType::Standing},
        {{StateType::Running, Transition::Jump}, StateType::InAir},
        {{StateType::Running, Transition::Move}, StateType::Running},
        {{StateType::InAir, Transition::Jump}, StateType::DoubleInAir},
        {{StateType::DoubleInAir, Transition::Stop}, StateType::Standing}
    };
    States states = {
        {StateType::Standing, new Standing()},
        {StateType::InAir, new InAir()},
        {StateType::Running, new Running()},
        {StateType::DoubleInAir, new DoubleInAir()}
    };
    FSM* fsm = new FSM{transitions, states, StateType::Standing};

    //player input
    KeyboardInput* input = new KeyboardInput();

    player = std::make_unique<GameObject>("player", fsm, input, Color{255, 0, 255, 255});
}
