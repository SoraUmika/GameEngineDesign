#include <EntitySYSTEM.h>
#include <Engine.h>

EntitySYSTEM::EntitySYSTEM()
{
    for (Entity entity = 0; entity < MAX_ENTITIES; ++entity)
    {
        available_entities.push(entity);
    }
}

Entity EntitySYSTEM::Create_Entity()
{
    assert(available_entities.size() > 0 && "Too many entities in existence.");

    // Take an ID from the front of the queue
    Entity ID = available_entities.front();
    available_entities.pop();
    Logger::log(LogLevel::INFO, "Entity ID %d reserved", ID);
    return ID;
}

void EntitySYSTEM::Destroy_Entity(Entity entity)
{
    assert(entity < MAX_ENTITIES && "Entity out of range.");

    // Invalidate the destroyed entity's signature

    // Put the destroyed ID back into the queue
    available_entities.push(entity);
    Logger::log(LogLevel::INFO, "Entity ID %d Freed", entity);
}

Entity EntitySYSTEM::Create_Rendered_Text(std::string text, TransformComponent transform)
{
    Entity entity = Create_Entity();
	TextureComponent textureComp;
	auto& componentSYSTEM = Get_Engine()->Get_System<ComponentSYSTEM>();
    auto& graphicsSYSTEM = Get_Engine()->Get_System<GraphicsSYSTEM>();
    graphicsSYSTEM.Load_Texture_From_Text(textureComp, text, graphicsSYSTEM.Get_DefaultFont());
	componentSYSTEM.Add_Component<TextureComponent>(entity, textureComp);
	componentSYSTEM.Add_Component<TransformComponent>(entity, transform);
    return entity;
}

Entity EntitySYSTEM::Create_TileMap_From_JSON()
{
    Entity entity = Create_Entity();

    return entity;
}

Entity EntitySYSTEM::Create_CollisionWall(int x, int y, int w, int h)
{
    Entity entity = Create_Entity();
    return entity;
}