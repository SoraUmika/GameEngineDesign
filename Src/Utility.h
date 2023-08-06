#ifndef UTILITY_H
#define UTILITY_H
#include <Logger.h>
#include <SDL2/SDL.h>
#include <vector>
#include <sol/sol.hpp>
#include <SYSTEM.h>

/*
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
*/
namespace DataStructure
{
    template <typename K, typename T, std::size_t N>
    class MappedArray{
        public:
            void add(const K& key, const T& value) {
                if (size_count >= N) {
                    throw std::overflow_error("MappedArray is full");
                }
                index_lookup_map[key] = size_count;
                data[size_count] = value;
                size_count++;
            }
            void remove(const K& key) {
                if (!exists(key)) {
                    throw std::out_of_range("Key does not exist");
                }
                size_t index_to_remove = index_lookup_map[key];

                if (index_to_remove != size_count - 1) {
                    std::swap(data[index_to_remove], data[size_count - 1]);
                    for (auto& pair : index_lookup_map) {
                        if (pair.second == size_count - 1) {
                            pair.second = index_to_remove;
                            break;
                        }
                    }
                }

                index_lookup_map.erase(key);
                size_count--;
            }
            bool exists(const K& key) const {
                return index_lookup_map.find(key) != index_lookup_map.end();
            }
            T& operator[](const K& key) {
                return data.at(index_lookup_map.at(key));
            }
            const T& operator[](const K& key) const {
                return data.at(index_lookup_map.at(key));
            }
            std::size_t size() const {
                return size_count;
            }
            std::size_t capacity() const {
                return N;
            }
        private:
            std::array<T, N> data;
            std::unordered_map<K, size_t> index_lookup_map;
            size_t size_count = 1;
    };
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

            std::vector<Cell>& Get_Cells();
            Cell& Get_Cell(int index_x, int index_y);
            SDL_Rect Get_Cell_Rect(int index_x, int index_y);
            SDL_Point Get_Cell_Counts();
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
                cellsRect.start_cell_x = static_cast<int>(rect.x / CELL_SIZE);
                cellsRect.start_cell_y = static_cast<int>(rect.y / CELL_SIZE);
                cellsRect.end_cell_x = static_cast<int>((rect.x + rect.w) / CELL_SIZE);
                cellsRect.end_cell_y = static_cast<int>((rect.y + rect.h) / CELL_SIZE);
                return cellsRect;
            }
            int width{};
            int height{};
            int cell_count_x{};
            int cell_count_y{};
            const int CELL_SIZE = 128;
            std::vector<Cell> cells;
    };
}

#endif // !UTILITY_H