#ifndef WORLDSYSTEM_H
#define WORLDSYSTEM_H
#include <SYSTEM.h>
#include <JSON/json.hpp>
#include <Utility.h>
class WorldSYSTEM: SYSTEM
{
    public:
        WorldSYSTEM(Engine& engine);
        ~WorldSYSTEM();

        void Init_TileMaps_From_JSON(const nlohmann::json& json);
    private:
};
#endif // !WORLDSYSTEM_H