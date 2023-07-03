#include "Engine.h"

Engine::Engine()
{
}

Engine::~Engine()
{
	SDL_Quit();
}

bool Engine::Init_SYSTEMS()
{
	bool success = true;
	SYSTEM::Set_Engine_Ptr(this);
	Get_System<GraphicsSYSTEM>().Init_Window("MyGameApp", 1920,1080);
	Get_System<GraphicsSYSTEM>().Init_Renderer(1920,1080);

	Get_System<ComponentSYSTEM>().Register_Component_Type<TextureComponent>();
	Get_System<ComponentSYSTEM>().Register_Component_Type<TransformComponent>();
	Get_System<ComponentSYSTEM>().Register_Component_Type<RenderComponent>();
	Get_System<ComponentSYSTEM>().Register_Component_Type<CollisonBoxComponent>();
	return success;
}


bool Engine::Init_Variables()
{
	bool success = true;
	run = true;
	FPS_entity = Get_System<EntitySYSTEM>().Create_Rendered_Text(std::to_string(0), {5,5,0.0});
	return success;
}

bool Engine::Init_Everything()
{
	bool system_flag = Init_SYSTEMS();
	bool variables_flag = Init_Variables();
	return system_flag && variables_flag;
}

void Engine::Update()
{
	Get_System<EventSYSTEM>().Update_SDL_Events();
	Get_System<EventSYSTEM>().Check_Default_Events();
	Get_System<EventSYSTEM>().Clear_SDL_Events();
}

void Engine::Render()
{	
	auto& graphicsSYSTEM = Get_System<GraphicsSYSTEM>();
	graphicsSYSTEM.Render_Clear();
	
	graphicsSYSTEM.Draw_Texture(FPS_entity);
	graphicsSYSTEM.Render_Present();
}

bool Engine::Run_Game_Loop()
{
	int previous_time = SDL_GetTicks64();
	int lag = 0;
	int update_count = 0;
	int frame_count = 0;
	TimeRepeaterComponent repeater(1000);
	while(run)
	{
		int current_time = SDL_GetTicks64();
		int elapsed_time = current_time-previous_time;
		previous_time = current_time;
		lag += elapsed_time;

		while(lag >= 1000.0/UPDATE_PER_SECOND)
		{
			lag -= 1000.0/UPDATE_PER_SECOND;
			Update();
			update_count += 1;
		}
		if(repeater.Get_Signal())
		{
			printf("%d,%d\n", update_count, frame_count);
			TextureComponent& FPS_textureComp = Get_System<ComponentSYSTEM>().Get_Component<TextureComponent>(FPS_entity);
			Get_System<GraphicsSYSTEM>().Load_Texture_From_Text(FPS_textureComp, std::to_string(frame_count), Get_System<GraphicsSYSTEM>().Get_DefaultFont());
			update_count = 0;
			frame_count = 0;
		}
		Render();
		frame_count += 1;
	}
	return 0;
}

void Engine::Shut_Down()
{
	run = false;
}
