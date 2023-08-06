#ifndef WORLDSYSTEM_H
#define WORLDSYSTEM_H
#include <SYSTEM.h>
#include <Utility.h>
#include <Components.h>
#include <sol/sol.hpp>
#include <algorithm>

class WorldSYSTEM: SYSTEM
{
    public:
        WorldSYSTEM(Engine& engine);
        ~WorldSYSTEM();


        void Register_Scene(const std::string& name);
        void Add_Entity_To_Scene(const std::string& name, Entity entity);
        void Remove_Entity_From_Scene(const std::string& name, Entity entity);

        size_t Create_New_Scene(const std::string& name, int width, int height);
        void Set_Current_Scene(const std::string& name);
        void Set_Current_Scene(size_t index);
        void Lock_Camara_To_Entity(Entity entity);


        std::vector<Scene>& Get_Scenes();
        size_t Get_Current_Map();
        std::unordered_map<Entity, size_t>& Get_Entity_Map();

        void Update();
        void Render();
        struct Camera{
            Entity target{};
            bool lock = false;
        };
    private:
        std::unordered_map<std::string, size_t> scene_name_to_scene_index{};
        std::vector<Scene> scenes;
        std::unordered_map<Entity, size_t> entity_to_scene_index;

        size_t current_scene_index{};

        TransformComponent camera_transform{};

        Camera camera;
};
#endif // !WORLDSYSTEM_H