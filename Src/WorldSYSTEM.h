#ifndef WORLDSYSTEM_H
#define WORLDSYSTEM_H
#include <SYSTEM.h>
#include <JSON/json.hpp>
#include <Utility.h>
#include <Components.h>
#include <sol/sol.hpp>
class WorldSYSTEM: SYSTEM
{
    public:
        WorldSYSTEM(Engine& engine);
        ~WorldSYSTEM();

        void Register_Tilemap(const sol::table& tilemap);
        
        void Update();
        void Render();
    private:
        std::unordered_map<std::string,TileMapComponent> tile_maps{};
};
#endif // !WORLDSYSTEM_H