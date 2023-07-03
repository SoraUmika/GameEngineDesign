#ifndef ENGINE_H
#define ENGINE_H
#include <stdio.h>
#include <EventSYSTEM.h>
#include <GraphicsSYSTEM.h>
#include <FileSYSTEM.h>
#include <EntitySYSTEM.h>
#include <ComponentSYSTEM.h>
#include <PhysicsSYSTEM.h>
#include <Components.h>
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
		
		template <typename T>
		T& Get_System() {
			return std::get<T>(systems);
		}		
	private:
		std::tuple<EventSYSTEM, GraphicsSYSTEM, FileSYSTEM, EntitySYSTEM, ComponentSYSTEM, PhysicsSYSTEM> systems;
		bool run;
		const int UPDATE_PER_SECOND = 60;
		Entity FPS_entity;
};

#endif // !ENGINE_H

