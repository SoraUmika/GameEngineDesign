#ifndef PHYSICSSYSTEM_H
#define PHYSICSSYSTEM_H
#include <SYSTEM.h>
#include <vector>
#include <unordered_map>
#include <EntitySYSTEM.h>
#include <Utility.h>

class PhysicsSYSTEM: public SYSTEM
{
    public:
        PhysicsSYSTEM(Engine& engine);
        ~PhysicsSYSTEM();
        
        Entity Cell_Collision_Check(SpatialPartition::Cell& cell, Entity self, const SDL_Rect& rect);
        bool Is_Collision(const SDL_Rect& rect1, const SDL_Rect& rect2);
        
        void Update_Entities_Position();
        void Update();
    private:
};

#endif // !PHYSICSSYSTEM_H