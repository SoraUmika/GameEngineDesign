#ifndef ENGINE_H
#define ENGINE_H
#include <EventSYSTEM.h>
#include <GraphicsSYSTEM.h>
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
		void Init_SYSTEMS();
		void Init_Everything();
		void Init_Variables();
		bool Run_Game_Loop();
		void Update();
		void Render();
		
		void Shut_Down();
		EventSYSTEM& Get_EventSYSTEM();
		GraphicsSYSTEM& Get_GraphicsSYSTEM();
		ComponentSYSTEM& Get_ComponentSYSTEM();
		EntitySYSTEM& Get_EntitySYSTEM();
		PhysicsSYSTEM& Get_PhysicsSYSTEM();
		TimeSYSTEM& Get_TimeSYSTEM();
		WorldSYSTEM& Get_WorldSYSTEM();

		struct Internal_Variables
		{
			uint64_t frames_count{};
			uint64_t update_count{};
			Entity ID;
		};
		
	private:
		ScriptSYSTEM SYSTEM_script;
		ComponentSYSTEM SYSTEM_component;
		EventSYSTEM SYSTEM_event;
		GraphicsSYSTEM SYSTEM_graphics;
		EntitySYSTEM SYSTEM_entity;
		PhysicsSYSTEM SYSTEM_physics;
		TimeSYSTEM SYSTEM_time;
		WorldSYSTEM SYSTEM_world;
		bool run;
		const double UPDATE_PER_SECOND = 50;

		Internal_Variables intenal_variables;
};

#endif // !ENGINE_H

