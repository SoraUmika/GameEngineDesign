#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <SDL2/SDL.h>
#include <memory>
#include <vector>
#include <Logger.h>
#include <unordered_map>
#include <algorithm>
template <typename T>
class Grid
{
    public:
        Grid(int cell_count_x, int cell_count_y) :cell_count_x(cell_count_x), cell_count_y(cell_count_y){};
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
        int cell_count_x;
        int cell_count_y;
};

struct TextureComponent
{
    struct SDLTextureDeleter {
        void operator()(SDL_Texture* texture) const {
            SDL_DestroyTexture(texture);
            texture = nullptr;
            Logger::log(LogLevel::INFO, "Texture Destroyed");
        }
    };
    std::shared_ptr<SDL_Texture> texture = nullptr;
    int width = 0;
    int height = 0;
};

struct TileTextureComponent: public TextureComponent
{
    int tmp;
};

struct TransformComponent
{
    SDL_Point position = {0,0};
    double angle = 0.0;
};

struct CollisonBoxComponent
{
    SDL_Rect box;
};

struct RenderComponent
{
    TextureComponent* texture_ptr;
    SDL_Rect clip;
};


class TileLayer
{
    std::vector<int> tile_data;
    int tile_count_horizontal = 0;
    int tile_count_vertical = 0;
};

class TileMapComponent
{
    std::string map_name;
    std::vector<TileLayer> tile_layers;
};

class TimeRepeaterComponent
{
    public:
        TimeRepeaterComponent(Uint64 interval)
        {
            this->interval = interval;
            this->previous_time = 0;
        }
        ~TimeRepeaterComponent(){}

        bool Get_Signal()
        {
            if (SDL_GetTicks64() - previous_time >= interval)
            {
                previous_time = SDL_GetTicks();
                return true;
            }
            else
            {
                return false;
            }           
        }
        void Set_SignalInterval(Uint64 interval)
        {
            this->interval = interval;
            this->previous_time = 0;
        }
    private:
        Uint64 interval;
        Uint64 previous_time;
};

#endif