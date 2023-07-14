#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <Logger.h>
#include <SDL2/SDL.h>
#include <memory>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <JSON/json.hpp>
#include <Utility.h>
using Entity = Uint64;


template <typename T>
class Grid
{
    public:
        Grid(int cell_count_x, int cell_count_y) :cell_count_x(cell_count_x), cell_count_y(cell_count_y){};
        Grid(){}
        ~Grid(){};

        void Insert_Element(int cell_x_index, int cell_y_index, T element)
        {
            if(cell_x_index < 0 || cell_x_index >= cell_count_x|| cell_y_index < 0 || cell_y_index >= cell_count_y) 
            {
                throw std::out_of_range("Coordinates are out of grid bounds");
            }
            grid[cell_x_index][cell_y_index].push_back(element);
        }

        void Remove_Element(int cell_x_index, int cell_y_index, T element)
        {
            auto& cell = grid[cell_x_index][cell_y_index];
            auto position = std::find(cell.begin(), cell.end(), element);
            if(position != cell.end()) {
                cell.erase(position);
            }
            else
            {
                Logger::log(LogLevel::WARNING, "Removing an non-existing element from grid at [%d,%d]", cell_x_index, cell_y_index);
            }
        }

        std::vector<T>& Get_Cell(int cell_x_index, int cell_y_index)
        {
            if(cell_x_index < 0 || cell_x_index >= cell_count_x || cell_y_index < 0 || cell_y_index >= cell_count_y) 
            {
                throw std::out_of_range("Coordinates are out of grid bounds");
            }
            return grid[cell_x_index][cell_y_index];
        }

    private:
        std::unordered_map<int, std::unordered_map<int, std::vector<T>>> grid;
        int cell_count_x{};
        int cell_count_y{};
};

struct TextureComponent
{
    struct SDLTextureDeleter 
    {
        void operator()(SDL_Texture* texture) const {
            SDL_DestroyTexture(texture);
            texture = nullptr;
            //Logger::log(LogLevel::INFO, "Texture Destroyed");
        }
    };
    TextureComponent(){}
    TextureComponent(const std::string& TextureInfo)
    {
        const nlohmann::json& Texture = nlohmann::json::parse(TextureInfo);
        path = Get_JSON_Key<std::string>(Texture, "path", "");
    }
    std::shared_ptr<SDL_Texture> texture = nullptr;
    int width = 0;
    int height = 0;
    std::string path{};
};

struct SpriteSheetComponent
{
    SpriteSheetComponent(){}
    SpriteSheetComponent(const std::string& SpriteSheetInfo)
    {
        const nlohmann::json& SpriteSheet = nlohmann::json::parse(SpriteSheetInfo);
        
        for(auto& identifier : SpriteSheet.at("group").items())
        {
            for(auto& clipObject: identifier.value())
            {
                SDL_Rect clip = Get_Rect_From_JSON_OBJ(clipObject);
                group[identifier.key()].push_back(clip);
            }
        }
    }
    std::unordered_map<std::string, std::vector<SDL_Rect>> group;
};

struct VelocityComponent
{
    VelocityComponent(){}
    VelocityComponent(const std::string& VelocityInfo)
    {
        const nlohmann::json& Velocity = nlohmann::json::parse(VelocityInfo);
        x = Get_JSON_Key<int>(Velocity, "x");
        y = Get_JSON_Key<int>(Velocity, "y");
        angle = Get_JSON_Key<int>(Velocity, "angle");
    }
    int x = 0;
    int y = 0;
    double angle = 0.0;

};

struct TransformComponent
{
    TransformComponent(){}
    TransformComponent(int x, int y, double angle): x(x), y(y), angle(angle){}
    TransformComponent operator+(const VelocityComponent& vel) const 
    {
        return TransformComponent(x+vel.x, y+vel.y, angle+vel.angle);
    }
    TransformComponent(const std::string& TransformInfo)
    {
        const nlohmann::json& Transform = nlohmann::json::parse(TransformInfo);
        x = Get_JSON_Key<int>(Transform, "x");
        y = Get_JSON_Key<int>(Transform, "y");
        angle = Get_JSON_Key<double>(Transform, "angle");
    }
    int x = 0;
    int y = 0;
    double angle = 0.0;
};

struct CollisionBoxComponent
{
    CollisionBoxComponent(){}
    CollisionBoxComponent(SDL_Rect rect): rect(rect){}
    CollisionBoxComponent(const std::string& CollisonBoxInfo)
    {
        const nlohmann::json& CollisonBox = nlohmann::json::parse(CollisonBoxInfo);
        rect = Get_Rect_From_JSON_OBJ(CollisonBox.at("rect"));
    }
    SDL_Rect rect{};
};

struct RenderComponent
{
    RenderComponent(){}
    RenderComponent(const std::string& RenderInfo)
    {
        const nlohmann::json& Render = nlohmann::json::parse(RenderInfo);
        if(Get_JSON_Key<bool>(Render, "is_clip", false))
        {
            clip = Get_Rect_From_JSON_OBJ(Render.at("clip"));
            is_clip = true;
        }
        scale = Get_JSON_Key<double>(Render, "scale", 1.0);
    }
    Entity ID{};
    SDL_Rect clip{};
    bool is_clip = false;
    double scale = 1.0;
};

struct TextComponent
{
    TextComponent(){}
    TextComponent(const std::string& TextInfo)
    {
        const nlohmann::json& Text = nlohmann::json::parse(TextInfo);
        text = Get_JSON_Key<std::string>(Text, "text", "None");
    }
    TextureComponent texture{};
    std::string text{};
};

struct TimeRepeaterComponent
{
    TimeRepeaterComponent(){}
    TimeRepeaterComponent(Uint64 interval, Uint64 previous_time): interval(interval), previous_time(previous_time){}
    TimeRepeaterComponent(const nlohmann::json& TimeRepeater)
    {
        interval = TimeRepeater["interval"].get<int>();
    }
    Uint64 interval{};
    Uint64 previous_time{};
};

struct TileLayer
{
    TileLayer(){}
    TileLayer(const std::string& TileLayerInfo)
    {
        const nlohmann::json& TileLayer = nlohmann::json::parse(TileLayerInfo);
        data = Get_JSON_Key<std::vector<int>>(TileLayer, "data");
        tile_count_horizontal = Get_JSON_Key<int>(TileLayer, "width");
        tile_count_vertical = Get_JSON_Key<int>(TileLayer, "height");
        name = Get_JSON_Key<std::string>(TileLayer, "name");
    } 
    std::vector<int> data;
    int tile_count_horizontal{};
    int tile_count_vertical{};
    std::string name{};
};

struct TileMapComponent
{
    TileMapComponent(){}
    TileMapComponent(const std::string& TileMapInfo)
    {
        const nlohmann::json& TileMap = nlohmann::json::parse(TileMapInfo);
        width = Get_JSON_Key<int>(TileMap, "width");
        height = Get_JSON_Key<int>(TileMap, "height");
        layers.reserve(10);
        for(auto& layer: TileMap.at("layers").items())
        {
            layers.push_back(TileLayer(layer.value().dump()));
        }
        
    }
    std::string map_name;
    Entity tileset_ID{};
    std::vector<TileLayer> layers;
    Grid<Entity> grid;
    int width{};
    int height{};
};

struct InputComponent
{
    InputComponent(){}
    InputComponent(const std::string& InputInfo){}
    std::function<void(Entity)> action;
};

#endif