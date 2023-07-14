#include <EntitySYSTEM.h>
#include <Engine.h>

EntitySYSTEM::EntitySYSTEM(Engine& engine): SYSTEM(engine)
{
    for (Entity entity = 1; entity < MAX_ENTITIES; ++entity)
    {
        available_entities.push(entity);
    }
}

Entity EntitySYSTEM::Create_Entity()
{
    assert(available_entities.size() > 0 && "Too many entities in existence.");
    Entity ID = available_entities.front();
    available_entities.pop();
    Logger::log(LogLevel::INFO, "Entity ID %d reserved", ID);
    return ID;
}

void EntitySYSTEM::Destroy_Entity(Entity entity)
{
    assert(entity < MAX_ENTITIES && "Entity out of range.");
    available_entities.push(entity);
    Logger::log(LogLevel::INFO, "Entity ID %d Freed", entity);
}

Entity EntitySYSTEM::Create_Entity_Of_Type(const std::string& entityType, const std::vector<std::string>& args)
{
    if (entity_types.find(entityType) == entity_types.end()) 
    {
        throw std::runtime_error("Entity type '" + entityType + "' is not registered.");
    }

    Entity newEntity = Create_Entity();
    auto& componentTuple = entity_types[entityType];
    std::vector<std::string>& components = std::get<0>(componentTuple);
    std::vector<std::string>& arguments = std::get<1>(componentTuple);
    if(!args.empty()){arguments = args;}

    Logger::log(LogLevel::INFO, "Entity of type %s created with ID %d", entityType.c_str(), newEntity);
    for (int i=0; i<components.size(); i++) 
    {  
        engine.Get_ComponentSYSTEM().Add_Component_By_Name(newEntity, components[i], arguments[i]);
    }
    return newEntity;
}

void EntitySYSTEM::Register_Entity_Type(const EntityType& entityType, const std::vector<std::string>& components, const std::vector<std::string>& default_values)
{
    if(entity_types.find(entityType) == entity_types.end())
    {
        for(int i=0; i<components.size(); i++)
        {
            if(!engine.Get_ComponentSYSTEM().Has_Component_Type(components.at(i)))
            {
                Logger::log(LogLevel::ERROR, "Component type '%s' does not exists, component not registered to entity type", components[i].c_str());
            }
        }
    }
    else
    {
        Logger::log(LogLevel::WARNING, "Registering entity type '%s' failed, already registered", entityType.c_str());
        return;
    }
    auto& componentTuple = entity_types[entityType];
    std::get<0>(componentTuple) = components;
    std::get<1>(componentTuple) = default_values;
    Logger::log(LogLevel::INFO, "Entity type '%s' registered with components: %s \n with default values: %s", 
        entityType.c_str(), nlohmann::json(components).dump().c_str(), nlohmann::json(default_values).dump().c_str());
}

void EntitySYSTEM::Register_Entity_Types_From_Json(const nlohmann::json& json)
{
    for(auto& type : json.items()) 
    {   
        if(type.value().is_object() || type.value().is_array()) 
        {
            std::vector<std::string> components;
            std::vector<std::string> deafult_values;
            components.reserve(type.value().size());
            deafult_values.reserve(type.value().size());
            for(auto& component : type.value().items()) 
            {
                components.push_back(component.key());
                deafult_values.push_back(component.value().dump());
            }
            Register_Entity_Type(type.key(), components, deafult_values);
        }
    }
}

