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
#include <BasicComponents.h>

struct VelocityComponent{
    VelocityComponent(){}
    VelocityComponent(float x, float y, float angle): x(x), y(y), angle(angle){}
    float x = 0;
    float y = 0;
    float angle = 0.0;
    bool active = false;
};

struct TransformComponent{
    TransformComponent(){}
    TransformComponent(float x, float y, float angle): x(x), y(y), angle(angle){}
    TransformComponent operator+(const VelocityComponent& vel) const 
    {
        return TransformComponent(x+vel.x, y+vel.y, angle+vel.angle);
    }
    float x = 0;
    float y = 0;
    float angle = 0.0;
};

struct TextureComponent{
    TextureComponent(){}
    struct SDLTextureDeleter 
    {
        void operator()(SDL_Texture* texture) const {
            SDL_DestroyTexture(texture);
        }
    };

    std::unique_ptr<SDL_Texture, SDLTextureDeleter> texture = nullptr;
    int width = 0;
    int height = 0;
};

struct RigidBodyComponent{
    RigidBodyComponent(){}
    RigidBodyComponent(RectComponent<float> rect, bool is_collision, bool is_interactive): 
        rect(rect), is_collision(is_collision), is_interactive(is_interactive){}
    
    bool is_collision{};
    bool is_interactive{};
    RectComponent<float> rect{};
    float relative_x{};
    float relative_y{};
};

struct RenderInfoComponent{
    RenderInfoComponent(){}
    RenderInfoComponent(double scale, bool is_clip, SDL_Rect clip): 
        scale(scale), is_clip(is_clip), clip(clip){}
    size_t index{};
    SDL_Rect clip{};
    bool is_clip = false;
    float scale = 1.0;
    float offset_x{};
    float offset_y{};
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

struct AnimationComponent{
    AnimationComponent(){}
    int active_animation{};
    std::unordered_map<size_t, SDL_Rect> clips;
};

struct TileSet{
    int width{};
    int height{};
    std::string identifier{};
    int tilewidth{};
    int tileheight{};
    int tilecount{};
    int columns{};
    std::unordered_map<std::string, SDL_Rect> objects_clip;
};

struct TileLayer{
    TileLayer(){}
    std::vector<int> data{};
    int tile_x_count{};
    int tile_y_count{};
    std::string name{};
};

struct TileMapComponent{
    TileMapComponent(int width, int height): width(width), height(height), grid(width, height){}
    std::string map_name;
    size_t tileset_ID{}; 
    SpatialPartition::Grid grid;
    std::vector<TileLayer> layers;
    std::vector<Entity> entities;
    int width{};
    int height{};
};

struct Scene{
    Scene(const std::string& name, size_t ID, int width, int height): width(width), height(height), grid(width, height)
    {
        scene_name = name;
        scene_ID = ID;
    }
    std::string scene_name{};
    size_t scene_ID{};
    std::vector<Entity> entities;
    SpatialPartition::Grid grid;
    int width{};
    int height{};
};

struct FontComponent{
    struct SDLFontDeleter 
    {
        void operator()(TTF_Font* font) const {
            TTF_CloseFont(font);
        }
    };
    std::unique_ptr<TTF_Font, SDLFontDeleter> font = nullptr;
    TextureComponent glyphs;
    std::array<SDL_Rect, 128> glyph_clips;
};
#endif 