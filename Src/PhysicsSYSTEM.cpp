#include <PhysicsSYSTEM.h>
#include <Engine.h>
PhysicsSYSTEM::PhysicsSYSTEM(Engine& engine): SYSTEM(engine)
{

}

PhysicsSYSTEM::~PhysicsSYSTEM()
{

}

Entity PhysicsSYSTEM::Cell_Collision_Check(SpatialPartition::Cell& cell, Entity self, const SDL_Rect& rect){
    auto& rigidBody_array = engine.Get_ComponentSYSTEM().Get_Component_Array<RigidBodyComponent>();
    auto& cell_entities = cell.Get_Cell();
    for(int i=0; i<cell_entities.size(); i++){
        Entity entity = cell_entities[i];
        if(self != entity){
            RigidBodyComponent& rigidBody = rigidBody_array.Get_Data(entity);
            if(rigidBody.is_collision){
                if(Is_Collision(rigidBody.rect, rect)){
                    return entity;
                }
            }
        }
    }
    return 0;
}

bool PhysicsSYSTEM::Is_Collision(const SDL_Rect& rect1, const SDL_Rect& rect2)
{
    if (rect1.x + rect1.w < rect2.x)
        return false;
    if (rect2.x + rect2.w < rect1.x)
        return false;
    if (rect1.y + rect1.h < rect2.y)
        return false;
    if (rect2.y + rect2.h < rect1.y)
        return false;
    return true;
}

void PhysicsSYSTEM::Update_Entities_Position()
{
    auto& compSYSTEM = engine.Get_ComponentSYSTEM();
    ComponentArray<TransformComponent>& transform_comp_array = compSYSTEM.Get_Component_Array<TransformComponent>();
    ComponentArray<VelocityComponent>& velocity_comp_array = compSYSTEM.Get_Component_Array<VelocityComponent>();
    ComponentArray<RigidBodyComponent>& rigidBody_comp_array = compSYSTEM.Get_Component_Array<RigidBodyComponent>();
    auto& entities_list = velocity_comp_array.Get_Entities();

    for(int i=0; i<entities_list.size(); i++){
        Entity entity = entities_list[i];
        auto& transform = transform_comp_array.Get_Data(entity);
        auto& velocity = velocity_comp_array.Get_Data(entity);
        transform.x += velocity.x; transform.y+= velocity.y; transform.angle += velocity.angle;
        if(rigidBody_comp_array.Has_Data(entity)){
            auto& rigidBody = rigidBody_comp_array.Get_Data(entity);
            rigidBody.rect.x = transform.x; rigidBody.rect.y = transform.y;
        }
    }
}

void PhysicsSYSTEM::Update()
{
    Update_Entities_Position();
}