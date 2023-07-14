#include <PhysicsSYSTEM.h>
#include <Engine.h>
PhysicsSYSTEM::PhysicsSYSTEM(Engine& engine): SYSTEM(engine)
{

}

PhysicsSYSTEM::~PhysicsSYSTEM()
{

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
        transformComp = transformComp + velocityComp;
        velocityComp.x = velocityComp.y = velocityComp.angle = 0;
    }
}

void PhysicsSYSTEM::Update()
{
    Update_Entities_Position();
}