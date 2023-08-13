#include <PhysicsSYSTEM.h>
#include <Engine.h>
PhysicsSYSTEM::PhysicsSYSTEM(Engine& engine): SYSTEM(engine)
{

}

PhysicsSYSTEM::~PhysicsSYSTEM()
{

}

Entity PhysicsSYSTEM::Cell_Collision_Check(SpatialPartition::Cell& cell, Entity self, const RectComponent<float>& rect){
    auto& rigidBody_array = engine.Get_ComponentSYSTEM().Get_Component_Array<RigidBodyComponent>();
    std::vector<Entity>& cell_entities = cell.Get_Cell();
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

Entity PhysicsSYSTEM::Grid_Collision_Check(SpatialPartition::Grid& grid, Entity self, const RectComponent<float>& rect){
    SpatialPartition::Grid::OverlappedBoundary boundaries = grid.Get_OverlappedBoundary(rect);
    for(int i=boundaries.start_cell_x; i<= boundaries.end_cell_x; i++){
        for(int j=boundaries.start_cell_y; j<= boundaries.end_cell_y; j++){
            Entity collided_entity = Cell_Collision_Check(grid.Get_Cell(i,j), self, rect);
            if(collided_entity){
                return collided_entity;
            }
        }
    }
    return 0;
}

bool PhysicsSYSTEM::Is_Collision(const RectComponent<float>& rect1, const RectComponent<float>& rect2)
{
    if (rect1.x + rect1.w <= rect2.x)
        return false;
    if (rect2.x + rect2.w <= rect1.x)
        return false;
    if (rect1.y + rect1.h <= rect2.y)
        return false;
    if (rect2.y + rect2.h <= rect1.y)
        return false;
    return true;
}

Vect2D<float> PhysicsSYSTEM::Get_Distance(const RectComponent<float>& rect1, const RectComponent<float>& rect2){
    //rect 1 is to the left of rect 2
    Vect2D<float> distance = {0, 0};
    if(rect1.x+rect1.w < rect2.x)
        distance.x = rect2.x - (rect1.x+rect1.w);
    //rect 1 is to the right of rect 2
    else if(rect1.x > rect2.x+rect2.w)
        distance.x = (rect2.x+rect2.w) - rect1.x;
    //between left and x mid point
    //else if((rect1.x + rect1.w)/2 > (rect2.x + rect2.x)/2)
    //    distance.x = rect1.x - (rect2.x+rect2.w);
    //between right and x mid point
    //else if((rect1.x + rect1.w)/2 <= (rect2.x + rect2.x)/2)
    //    distance.x = (rect1.x + rect1.w)  - rect2.x;

    //rect 1 is on top of rect2
    if(rect1.y+rect1.h < rect2.y)
        distance.y = rect2.y - (rect1.y+rect1.h);
    //rect1 is on botton of rect2
    else if(rect1.y > rect2.y+rect2.h)
        distance.y = (rect2.y+rect2.h) - rect1.y;
    return distance;
}

Vect2D<float> PhysicsSYSTEM::Get_Distance(const Vect2D<float>& vect1, const Vect2D<float>& vect2){
    return vect2-vect1;
}

Vect2D<float> PhysicsSYSTEM::Get_Distance_Correction(const Vect2D<float>& distance, const RectComponent<float>& rect1, const RectComponent<float>& rect2){
    Vect2D<float> correction = {0,0};
    if(distance.x < (rect1.w + rect2.w)/2)
        correction.x = distance.x - (rect1.w+rect2.w)/2;
    //else if(distance.x <= - (rect1.w+rect2.w)/2 )
    //    correction.x = distance.x + (rect1.w+rect2.w)/2;
    std::cout << distance.x <<"," << correction.x << std::endl;
    return correction;
}

Vect2D<float> PhysicsSYSTEM::Rect_Center_Pos(const RectComponent<float>& rect){
    return {(2*rect.x+rect.w)/2, (2*rect.y+rect.h)/2};
}

void PhysicsSYSTEM::Update_Scene_Physics(Scene& scene){
    auto& componentSYSTEM = engine.Get_ComponentSYSTEM();
    auto& velocity_components = componentSYSTEM.Get_Component_Array<VelocityComponent>();
    auto& velocity_entities = velocity_components.Get_Entities();

    auto& transform_components = componentSYSTEM.Get_Component_Array<TransformComponent>();
    auto& rb_components = componentSYSTEM.Get_Component_Array<RigidBodyComponent>();
    for(Entity velocity_entity : velocity_entities){
        TransformComponent& transform = transform_components.Get_Data(velocity_entity);
        VelocityComponent& velocity = velocity_components.Get_Data(velocity_entity);

        float target_x = transform.x + velocity.x;
        float target_y = transform.y + velocity.y;
        
        if(rb_components.Has_Data(velocity_entity)){
            auto& rb = rb_components.Get_Data(velocity_entity);
            RectComponent<float> target_rect_x = rb.rect;
            RectComponent<float> target_rect_y = rb.rect;
            target_rect_x.x = target_x;
            target_rect_y.y = target_y;

            Entity collided_entity_x = Grid_Collision_Check(scene.grid, velocity_entity, target_rect_x);
            Entity collided_entity_y = Grid_Collision_Check(scene.grid, velocity_entity, target_rect_y);
            if(collided_entity_x){
                auto& collided_entity_rb_x = rb_components.Get_Data(collided_entity_x);
                //Vect2D<float> distance = Get_Distanace(rb.rect, collided_entity_rb_x.rect);
                target_x = transform.x;
                target_rect_x = rb.rect;
                //target_rect_x.x += distance.x;

                std::cout << Rect_Center_Pos(rb.rect).x << "," << Rect_Center_Pos(collided_entity_rb_x.rect).x << std::endl;
                Vect2D<float> distance = Get_Distance(Rect_Center_Pos(rb.rect), Rect_Center_Pos(collided_entity_rb_x.rect));

                Vect2D<float> correction = Get_Distance_Correction(distance, rb.rect, collided_entity_rb_x.rect);
                //target_y += 1;
                //target_rect_y.y += 1;
            }
            if(collided_entity_y){
                auto& collided_entity_rb_y = rb_components.Get_Data(collided_entity_y);
                //Vect2D<float> distance = Get_Distanace(rb.rect, collided_entity_rb_y.rect);
                target_y = transform.y;
                target_rect_y = rb.rect;
                //target_rect_y.y += distance.y;

                //std::cout << distance.x << std::endl;
                //target_x += 1;
                //target_rect_y.x += 1;
            }

            RectComponent<float> target_rect(target_x, target_y, rb.rect.w, rb.rect.h);
            scene.grid.Move(velocity_entity, rb.rect, target_rect);
            rb.rect = target_rect;   
        }
        transform = TransformComponent(target_x, target_y, transform.angle);
    }   
}

void PhysicsSYSTEM::Update_Entities_Position(){

}

void PhysicsSYSTEM::Update()
{
    //Update_Entities_Position(engine.Get_WorldSYSTEM());
}