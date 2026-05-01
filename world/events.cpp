#include "events.h"
#include "world.h"
#include "game_object.h"
#include "audio.h"

void NextLevel::perform(World& world, GameObject& obj) {
    if (&obj != world.player) return; //stop enemies from triggering the next level
    world.end_level = true;
}

void Drown::perform(World& world, GameObject& obj) {
    world.audio->play_sound("splash");
    obj.take_damage(obj.health);
}

void Gather::perform(World& world, GameObject& obj) {
    ++obj.coin_count;
    world.audio->play_sound("coin");
}