#ifndef UTILITY_H
#define UTILITY_H
#include <Logger.h>
#include <SDL2/SDL.h>
#include <vector>
#include <sol/sol.hpp>
#include <SYSTEM.h>
#include <BasicComponents.h>

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
            MappedArray(){
                data.reserve(N);
                data.emplace_back(T());
            }
            size_t add(const K& key, T& value) {
                if(exists(key)){
                    Logger::log(LogLevel::ERROR, "Failed to add mapped array element: Element already exists!");
                    return 0;
                }
                index_lookup_map[key] = data.size();
                data.push_back(std::move(value));
                return index_lookup_map[key];
            }
            void remove(const K& key) {
                if (!exists(key)) {
                    Logger::log(LogLevel::ERROR, "Failed to remove mapped array element: element does not exists!");
                    return;
                }
                size_t index_to_remove = index_lookup_map[key];
                if (index_to_remove != data.size() - 1) {
                    std::swap(data[index_to_remove], data.back());
                    for (auto& pair : index_lookup_map) {
                        if (pair.second == data.size() - 1) {
                            pair.second = index_to_remove;
                            break;
                        }
                    }
                }
                data.pop_back();
                index_lookup_map.erase(key);
            }
            bool exists(const K& key) const {
                return index_lookup_map.find(key) != index_lookup_map.end();
            }
            T& operator[](const K& key) {
                return data.at(index_lookup_map.at(key));
            }
            T& operator[](size_t index) {
                return data.at(index);
            }
            size_t get_index(const K& key){
                if(exists(key)){
                    return index_lookup_map.at(key);
                }else{
                    Logger::log(LogLevel::ERROR, "Getting index of a mapped array failed, key doesnt exist" );
                    return 0;
                }
            }
            const T& operator[](const K& key) const {
                return data.at(index_lookup_map.at(key));
            }
            std::size_t size() const {
                return data.size();
            }
            std::size_t capacity() const {
                return data.capacity();
            }
        private:
            std::vector<T> data;
            std::unordered_map<K, size_t> index_lookup_map;
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
            struct OverlappedBoundary{
                int start_cell_x{};
                int start_cell_y{};
                int end_cell_x{};
                int end_cell_y{};
            };
            Grid();
            Grid(int width, int height);
            void Init(int width, int height);
            void Insert(Entity ID, const RectComponent<float>& entity_rect);
            void Remove(Entity ID, const RectComponent<float>& entity_rect);
            void Move(Entity ID, const RectComponent<float>& entity_rect, const RectComponent<float>& target_rect);

            std::vector<Cell>& Get_Cells();
            Cell& Get_Cell(int index_x, int index_y);
            SDL_Rect Get_Cell_Rect(int index_x, int index_y);
            SDL_Point Get_Cell_Counts();

            OverlappedBoundary Get_OverlappedBoundary(const RectComponent<float>& rect);
            void Print_Grid_Info();
        private:

            int width{};
            int height{};
            int cell_count_x{};
            int cell_count_y{};
            const int CELL_SIZE = 128;
            std::vector<Cell> cells;
    };
}

#endif // !UTILITY_H