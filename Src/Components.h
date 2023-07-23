#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <Logger.h>
#include <SDL2/SDL.h>
#include <memory>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <Utility.h>
#include <Events.h>

struct VelocityComponent{
    VelocityComponent(){}
    VelocityComponent(double x, double y, double angle): x(x), y(y), angle(angle){}
    double x = 0;
    double y = 0;
    double angle = 0.0;
    bool active = false;
};

struct TransformComponent{
    TransformComponent(){}
    TransformComponent(double x, double y, double angle): x(x), y(y), angle(angle){}
    TransformComponent operator+(const VelocityComponent& vel) const 
    {
        return TransformComponent(x+vel.x, y+vel.y, angle+vel.angle);
    }
    double x = 0;
    double y = 0;
    double angle = 0.0;
};

struct TextureComponent{
    TextureComponent(){}
    TextureComponent(const std::string& path):path(path){}
    struct SDLTextureDeleter 
    {
        void operator()(SDL_Texture* texture) const {
            SDL_DestroyTexture(texture);
            texture = nullptr;
        }
    };

    std::shared_ptr<SDL_Texture> texture = nullptr;
    int width = 0;
    int height = 0;
    std::string path{};
};

struct SpriteSheetComponent{
    SpriteSheetComponent(){}
    std::unordered_map<std::string, std::vector<SDL_Rect>> sprite_sheet;
};

struct RigidBodyComponent{
    RigidBodyComponent(){}
    RigidBodyComponent(SDL_Rect rect, bool is_collision, bool is_interactive): 
        rect(rect), is_collision(is_collision), is_interactive(is_interactive){}
    bool is_collision{};
    bool is_interactive{};
    SDL_Rect rect{};
    int relative_x{};
    int relative_y{};
};

struct RenderInfoComponent{
    RenderInfoComponent(){}
    RenderInfoComponent(double scale, bool is_clip, SDL_Rect clip): 
        scale(scale), is_clip(is_clip), clip(clip){}
    Entity ID{};
    SDL_Rect clip{};
    bool is_clip = false;
    double scale = 1.0;
    int offset_x{};
    int offset_y{};
};

struct EventListenerComponent{
    EventListenerComponent(){}
    EventListenerComponent(std::function<void(const Event&)> func): execute(func){}
    std::function<void(const Event&)> execute = [](const Event&){};
};

struct TimedStateMachineComponent
{
    TimedStateMachineComponent(){}
    TimedStateMachineComponent(int num_state, Uint64 state_duration): num_state(num_state), state_duration(state_duration){}
    Uint64 previous_time{};
    Uint64 elapsed_time{};
    Uint64 state_duration = 10;
    int current_state = 0;
    int num_state = 2;
    bool trigger = false;

};

struct TileLayer{
    TileLayer(){}
    std::vector<int> data;
    int tile_count_horizontal{};
    int tile_count_vertical{};
    std::string name{};};

struct TileMapComponent{
    TileMapComponent(){}
    std::string map_name;
    Entity tileset_ID{};
    std::vector<TileLayer> layers;
    int width{};
    int height{};
};

#endif