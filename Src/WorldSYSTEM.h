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

    private:
};
#endif // !WORLDSYSTEM_H