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


void PhysicsSYSTEM::Update_Scene_Physics(Scene& scene){
    auto& componentSYSTEM = engine.Get_ComponentSYSTEM();
    auto& velocity_components = componentSYSTEM.Get_Component_Array<VelocityComponent>();
    auto& velocity_entities = velocity_components.Get_Entities();

    auto& transform_components = componentSYSTEM.Get_Component_Array<TransformComponent>();
    auto& rigid_body_components = componentSYSTEM.Get_Component_Array<RigidBodyComponent>();
    for(Entity velocity_entity : velocity_entities){
        TransformComponent& transform = transform_components.Get_Data(velocity_entity);
        VelocityComponent& velocity = velocity_components.Get_Data(velocity_entity);
        
        transform.x += velocity.x; transform.y+= velocity.y; transform.angle += velocity.angle;
        if(rigid_body_components.Has_Data(velocity_entity)){
            auto& rigid_body = rigid_body_components.Get_Data(velocity_entity);
            rigid_body.rect.x = transform.x; rigid_body.rect.y = transform.y;
        }   
    }   
}

void PhysicsSYSTEM::Update_Entities_Position(){

}

void PhysicsSYSTEM::Update()
{
    //Update_Entities_Position(engine.Get_WorldSYSTEM());
}