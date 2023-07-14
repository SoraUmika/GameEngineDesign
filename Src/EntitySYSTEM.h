#ifndef ENTITYSYSTEM_H
#define ENTITYSYSTEM_H
#include <SYSTEM.h>
#include <queue>
#include <vector>
#include <cassert>
#include <Components.h>
#include <ComponentSYSTEM.h>
#include <JSON/json.hpp>

class EntitySYSTEM: public SYSTEM
{
    public:
        EntitySYSTEM(Engine& engine);
        Entity Create_Entity();
        void Destroy_Entity(Entity entity);
        
        //Component should be of the form {component: {...}}. If the object within {...} does not exists, the default paramters will be used
        //For now we assume component is in json form
        void Register_Entity_Type(const EntityType& entityType, const std::vector<std::string>& components, const std::vector<std::string>& default_values);

        //Here we assume that the json is of the form {type1:{comp1,comp2...}, type2: {comp1,comp2...}...}
        void Register_Entity_Types_From_Json(const nlohmann::json &json);
        
        Entity Create_Entity_Of_Type(const std::string& entityType, const std::vector<std::string>& args=std::vector<std::string>{});

    private:
        std::queue<Entity> available_entities;
        std::unordered_map<std::string, std::tuple<std::vector<std::string>,std::vector<std::string>>> entity_types;
        
};

#endif // !ENTITYSYSTEM