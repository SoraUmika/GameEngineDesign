#ifndef UTILITY_H
#define UTILITY_H
#include <Logger.h>
#include <JSON/json.hpp>
#include <SDL2/SDL.h>
#include <vector>
#include <sol/sol.hpp>
#include <SYSTEM.h>
nlohmann::json Read_Json_From_Files(std::string path);

template<typename T>
T Get_JSON_Key(const nlohmann::json& json, const std::string& key, const T& default_value=T{})
{
    if(json.contains(key))
    {
        try {
            return json[key].get<T>();
        } catch (const nlohmann::json::type_error& ex) {
            Logger::log(LogLevel::ERROR,"JSON - Type conversion Error for the key %s", key.c_str());
        } catch (const nlohmann::json::exception& ex) {
            Logger::log(LogLevel::ERROR,"JSON - Exception occurred for the key %s", key.c_str());
        }        
    }else
    {
        Logger::log(LogLevel::ERROR,"JSON - key '%s' does not exists!", key.c_str());
    }
    return default_value;
}

namespace SpatialPartition
{
    class Cell
    {
        public:
            Cell();
            bool Insert(Entity ID);
            bool Remove(Entity ID);
            std::vector<Entity>& Get_Cell();
            std::unordered_map<Entity, size_t>& Get_Index_Map();
            void Clear_Cell();
            void Print_Cell();
            Entity& operator[](int index) {
                if(index < 0 || index >= cell.size()) {
                    throw std::out_of_range("Index out of range");
                }
                return cell[index];
            }
            size_t size();
        private:
            std::vector<Entity> cell{};
            std::unordered_map<Entity, size_t> index_map;
    };

    class Grid
    {
        public:
            Grid();
            Grid(int width, int height);
            void Init(int width, int height);
            void Insert(Entity ID, const SDL_Rect& entity_rect);
            void Remove(Entity ID, const SDL_Rect& entity_rect);
            void Move(Entity ID, const SDL_Rect& entity_rect, const SDL_Rect& target_rect);
            Cell& Get_Cell(int index_x, int index_y);
            void Print_Grid_Info();
        private:
            struct CellsRect{
                int start_cell_x{};
                int start_cell_y{};
                int end_cell_x{};
                int end_cell_y{};
            };
            CellsRect Calculate_Cells_Rect(const SDL_Rect& rect){
                CellsRect cellsRect{};
                cellsRect.start_cell_x = static_cast<int>(rect.x / cell_length);
                cellsRect.start_cell_y = static_cast<int>(rect.y / cell_length);
                cellsRect.end_cell_x = static_cast<int>((rect.x + rect.w) / cell_length);
                cellsRect.end_cell_y = static_cast<int>((rect.y + rect.h) / cell_length);
                return cellsRect;
            }
            int width{};
            int height{};
            int cell_count_x{};
            int cell_count_y{};

            const int cell_length = 512;
            std::vector<Cell> cells;
    };
}

#endif // !UTILITY_H