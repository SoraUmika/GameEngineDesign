#ifndef ENTITYSYSTEM_H
#define ENTITYSYSTEM_H
#include <queue>
#include <cassert>
#include <SYSTEM.h>
#include <Logger.h>
#include <Components.h>

using Entity = Uint64;
constexpr std::size_t MAX_COMPONENTS = 32;
constexpr Entity MAX_ENTITIES = 1024;

class EntitySYSTEM: public SYSTEM
{
    public:
        EntitySYSTEM();
        
        Entity Create_Entity();
        void Destroy_Entity(Entity entity);

        Entity Create_Rendered_Text(std::string text, TransformComponent transform);
        Entity Create_TileMap_From_JSON();
        Entity Create_CollisionWall(int x, int y, int w, int h);
        

    private:
        std::queue<Entity> available_entities;
};

#endif // !ENTITYSYSTEM_H