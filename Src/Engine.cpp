#include "Engine.h"
Engine::Engine() : SYSTEM_event(*this),
				   SYSTEM_graphics(*this),
				   SYSTEM_entity(*this),
				   SYSTEM_physics(*this),
				   SYSTEM_time(*this),
				   SYSTEM_world(*this),
				   SYSTEM_script(*this)
{
	Logger::initialize("log.txt");
}

Engine::~Engine() {}

EventSYSTEM &Engine::Get_EventSYSTEM() { return SYSTEM_event; }
GraphicsSYSTEM &Engine::Get_GraphicsSYSTEM() { return SYSTEM_graphics; }
ComponentSYSTEM &Engine::Get_ComponentSYSTEM() { return SYSTEM_component; }
EntitySYSTEM &Engine::Get_EntitySYSTEM() { return SYSTEM_entity; }
PhysicsSYSTEM &Engine::Get_PhysicsSYSTEM() { return SYSTEM_physics; }
TimeSYSTEM &Engine::Get_TimeSYSTEM() { return SYSTEM_time; }
WorldSYSTEM &Engine::Get_WorldSYSTEM() { return SYSTEM_world; }

void Engine::Init_SYSTEMS(){
	SYSTEM_script.Init();

	sol::state &lua = SYSTEM_script.Get_Lua_State();
	
	SYSTEM_script.Run_Script_From_File("configuration.lua");
	sol::table default_window = lua.create_table_with("width", 1920, "height", 1080, "name", "Application");
	sol::table default_resolution = lua.create_table_with("width", 1920, "height", 1080, "vsync", true);
	sol::table window = SYSTEM_script.Get_Lua_Object<sol::table>("window", default_window);
	sol::table resolution = SYSTEM_script.Get_Lua_Object<sol::table>("resolution", default_resolution);

	SYSTEM_graphics.Init_Window(window["name"], window["width"], window["height"]);
	SYSTEM_graphics.Init_Renderer(resolution["width"], resolution["height"], resolution["vsync"]);

	SYSTEM_component.Register_Component_Type<TransformComponent>(lua, "Transform", "new", 
		sol::constructors<TransformComponent(), TransformComponent(int, int, double)>(),
		"x", &TransformComponent::x,"y", &TransformComponent::y,"angle", &TransformComponent::angle);
	SYSTEM_component.Register_Component_Type<VelocityComponent>(lua,"Velocity", "new", 
		sol::constructors<VelocityComponent(), VelocityComponent(int, int, double)>(),
		"x", &VelocityComponent::x,"y", &VelocityComponent::y, "angle", &VelocityComponent::angle, "active", &VelocityComponent::active);
	SYSTEM_component.Register_Component_Type<TextureComponent>(lua,"Texture", "new", 
		sol::constructors<TextureComponent(), TextureComponent(const std::string&)>(), 
		"path", &TextureComponent::path);
	SYSTEM_component.Register_Component_Type<RigidBodyComponent>(lua,"RigidBody", "new",
		sol::constructors<RigidBodyComponent(), RigidBodyComponent(SDL_Rect, bool, bool)>(),
		"rect", &RigidBodyComponent::rect, "is_collision", &RigidBodyComponent::is_collision, "is_interactive", &RigidBodyComponent::is_interactive);
	SYSTEM_component.Register_Component_Type<RenderInfoComponent>(lua,"RenderInfo", "new",
		sol::constructors<RenderInfoComponent(), RenderInfoComponent(double, bool, SDL_Rect)>(),
		"clip", &RenderInfoComponent::clip, "is_clip", &RenderInfoComponent::is_clip, "scale", &RenderInfoComponent::scale, "ID", &RenderInfoComponent::ID);
	SYSTEM_component.Register_Component_Type<EventListenerComponent>(lua,"EventListener", "new",
		sol::constructors<EventListenerComponent(),EventListenerComponent(std::function<void(const Event&)>)>(),
		"execute", &EventListenerComponent::execute);
	SYSTEM_component.Register_Component_Type<TimedStateMachineComponent>(lua, "TimedStateMachine", "new",
		sol::constructors<TimedStateMachineComponent(), TimedStateMachineComponent(int, Uint64)>(),
		"num_state", &TimedStateMachineComponent::num_state, "current_state", &TimedStateMachineComponent::current_state, "state_duration", 
		&TimedStateMachineComponent::state_duration, "elapsed_time", &TimedStateMachineComponent::elapsed_time, "trigger", &TimedStateMachineComponent::trigger);

	SYSTEM_component.Register_Component_Type<SpriteSheetComponent>(lua, "SpriteSheet", "new",
		sol::constructors<SpriteSheetComponent()>(),
		"group", &SpriteSheetComponent::sprite_sheet);	
	SYSTEM_component.Register_Component_Type<TileMapComponent>(lua,"Tilemap");

    //SYSTEM_script.Run_Script_From_File("Content/Scripts/helper.lua");
	SYSTEM_script.Run_Script_From_File("Content/Scripts/initialize.lua");
}

void Engine::Init_Variables(){
	run = true;
	intenal_variables.ID = SYSTEM_entity.Create_Entity();
}

void Engine::Init_Everything(){
	Init_SYSTEMS();
	Init_Variables();
}

void Engine::Update(){
	SYSTEM_event.Update();
	SYSTEM_physics.Update();
}

void Engine::Render(){
	SYSTEM_graphics.Render_Clear();
	SYSTEM_graphics.Render();
	SYSTEM_graphics.Render_Present();
	intenal_variables.frames_count += 1;
}

bool Engine::Run_Game_Loop(){
	uint64_t previous_time = SDL_GetTicks64();
	double lag = 0;
	TimedStateMachineComponent time_tracker_second = TimedStateMachineComponent(2, 1000);
	while (run)
	{
		double current_time = SDL_GetTicks64();
		double elapsed_time = current_time - previous_time;
		previous_time = current_time;
		lag += elapsed_time;
		int counter = 0;
		while (lag >= 1000.0 / UPDATE_PER_SECOND)
		{
			counter += 1;
			lag -= 1000.0 / UPDATE_PER_SECOND;
			Update();
			SYSTEM_time.Update_Time_Tracker(time_tracker_second);
			if (time_tracker_second.trigger)
			{
				std::string displayed_text = std::to_string(intenal_variables.frames_count) + "," + std::to_string(intenal_variables.update_count);
				intenal_variables.update_count = 0;
				intenal_variables.frames_count = 0;
				Logger::log(LogLevel::INFO, displayed_text);
			}
			intenal_variables.update_count += 1;
		}
		Render();
	}
	return 0;
}

void Engine::Shut_Down(){
	run = false;
	SDL_Quit();
	Logger::close();
}
