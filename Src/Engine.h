#ifndef ENGINE_H
#define ENGINE_H
#include <EventSYSTEM.h>
#include <GraphicsSYSTEM.h>
#include <FileSYSTEM.h>
#include <EntitySYSTEM.h>
#include <ComponentSYSTEM.h>
#include <PhysicsSYSTEM.h>
#include <TimeSYSTEM.h>
#include <WorldSYSTEM.h>
#include <ScriptSYSTEM.h>
class Engine
{
	public:
		Engine();
		~Engine();
		bool Init_SYSTEMS();
		bool Init_Everything();
		bool Init_Variables();
		bool Run_Game_Loop();
		void Update();
		void Render();
		
		void Shut_Down();
		EventSYSTEM& Get_EventSYSTEM();
		GraphicsSYSTEM& Get_GraphicsSYSTEM();
		FileSYSTEM& Get_FileSYSTEM();
		ComponentSYSTEM& Get_ComponentSYSTEM();
		EntitySYSTEM& Get_EntitySYSTEM();
		PhysicsSYSTEM& Get_PhysicsSYSTEM();
		TimeSYSTEM& Get_TimeSYSTEM();
		WorldSYSTEM& Get_WorldSYSTEM();

		struct Internal_Variables
		{
			uint64_t frames_count{};
			uint64_t update_count{};
		};
		
	private:
		EventSYSTEM SYSTEM_event;
		GraphicsSYSTEM SYSTEM_graphics;
		FileSYSTEM SYSTEM_file;
		ComponentSYSTEM SYSTEM_component;
		EntitySYSTEM SYSTEM_entity;
		PhysicsSYSTEM SYSTEM_physics;
		TimeSYSTEM SYSTEM_time;
		WorldSYSTEM SYSTEM_world;
		ScriptSYSTEM SYSTEM_script;
		bool run;
		const int UPDATE_PER_SECOND = 50;
		Entity FPS_entity;

		Internal_Variables intenal_variables;
};

#endif // !ENGINE_H

