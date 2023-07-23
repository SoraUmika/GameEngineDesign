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

