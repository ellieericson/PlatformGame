#include "asset_manager.h"

#include <filesystem>
#include <fstream>

Sprite AssetManager::get_game_object_sprite(const std::string& name, Graphics& graphics) {
    auto path_start = std::filesystem::current_path() / "assets";
    auto path = path_start/ (name + ".json");

    std::ifstream file(path);
    if (!file) {
        throw std::runtime_error("Could not open file: " + path.string());
    }
    nlohmann::json json;
    file >> json;
}
