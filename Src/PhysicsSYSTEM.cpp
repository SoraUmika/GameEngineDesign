#include <PhysicsSYSTEM.h>
#include <Engine.h>
PhysicsSYSTEM::PhysicsSYSTEM(Engine& engine): SYSTEM(engine)
{

}

PhysicsSYSTEM::~PhysicsSYSTEM()
{

}

bool PhysicsSYSTEM::Is_Collision(const SDL_Rect rect1, const SDL_Rect rect2)
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
    auto velocityComp_array = compSYSTEM.Get_Component_Array<VelocityComponent>().get();
    for(auto it = velocityComp_array->begin(); it != velocityComp_array->end(); ++it)
    {
        Entity ID = it->first;
        auto& transformComp = compSYSTEM.Get_Component<TransformComponent>(ID);
        auto& velocityComp = compSYSTEM.Get_Component<VelocityComponent>(ID);
        transformComp.x += velocityComp.x; transformComp.y += velocityComp.y; transformComp.angle += velocityComp.angle;
        if(compSYSTEM.Entity_Has_Component<RigidBodyComponent>(ID))
        {
            auto& rigidBodyComp = compSYSTEM.Get_Component<RigidBodyComponent>(ID);
            rigidBodyComp.rect.x = transformComp.x; rigidBodyComp.rect.y = transformComp.y;
            transformComp.x += velocityComp.x; transformComp.y += velocityComp.y; transformComp.angle += velocityComp.angle;
            continue;
        }
        transformComp.x += velocityComp.x; transformComp.y += velocityComp.y; transformComp.angle += velocityComp.angle;
    }
}

void PhysicsSYSTEM::Update()
{
    Update_Entities_Position();
}