#ifndef PHYSICSSYSTEM_H
#define PHYSICSSYSTEM_H
#include <SYSTEM.h>
#include <vector>
#include <unordered_map>
#include <EntitySYSTEM.h>
#include <Utility.h>
#include <Components.h>

class WorldSYSTEM;
class PhysicsSYSTEM: public SYSTEM
{
    public:
        PhysicsSYSTEM(Engine& engine);
        ~PhysicsSYSTEM();
        
        Entity Cell_Collision_Check(SpatialPartition::Cell& cell, Entity self, const RectComponent<float>& rect);
        Entity Grid_Collision_Check(SpatialPartition::Grid& grid, Entity self, const RectComponent<float>& rect);
        bool Is_Collision(const RectComponent<float>& rect1, const RectComponent<float>& rect2);
        
        Vect2D<float> Get_Distance(const RectComponent<float>& rect1, const RectComponent<float>& rect2);
        Vect2D<float> Get_Distance(const Vect2D<float>& vect1, const Vect2D<float>& vect2);
        void Update_Scene_Physics(Scene& scene);
        void Update_Entities_Position();
        void Update();
    private:
        Vect2D<float> Get_Distance_Correction(const Vect2D<float>& distance, const RectComponent<float>& rect1, const RectComponent<float>& rect2);
        Vect2D<float> Rect_Center_Pos(const RectComponent<float>& rect);
};

#endif // !PHYSICSSYSTEM_H