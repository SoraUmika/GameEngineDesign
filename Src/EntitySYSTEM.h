#ifndef ENTITYSYSTEM_H
#define ENTITYSYSTEM_H
#include <SYSTEM.h>
#include <queue>
#include <vector>
#include <cassert>
#include <JSON/json.hpp>

class EntitySYSTEM: public SYSTEM
{
    public:
        EntitySYSTEM(Engine& engine);
        Entity Create_Entity();
        void Destroy_Entity(Entity entity);

    private:
        std::queue<Entity> available_entities;
        std::unordered_map<std::string, std::tuple<std::vector<std::string>,std::vector<std::string>>> entity_types;
        
};

#endif // !ENTITYSYSTEM