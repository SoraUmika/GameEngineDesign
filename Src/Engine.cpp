#include "Engine.h"
Engine::Engine(): 
SYSTEM_event(*this),
SYSTEM_graphics(*this),
SYSTEM_file(*this),
SYSTEM_entity(*this),
SYSTEM_physics(*this),
SYSTEM_time(*this),
SYSTEM_world(*this),
SYSTEM_script(*this)
{
	Logger::initialize("log.txt");
}

Engine::~Engine(){}

EventSYSTEM& Engine::Get_EventSYSTEM(){return SYSTEM_event;}
GraphicsSYSTEM& Engine::Get_GraphicsSYSTEM(){return SYSTEM_graphics;}
FileSYSTEM& Engine::Get_FileSYSTEM(){return SYSTEM_file;}
ComponentSYSTEM& Engine::Get_ComponentSYSTEM(){return SYSTEM_component;}
EntitySYSTEM& Engine::Get_EntitySYSTEM(){return SYSTEM_entity;}
PhysicsSYSTEM& Engine::Get_PhysicsSYSTEM(){return SYSTEM_physics;}
TimeSYSTEM& Engine::Get_TimeSYSTEM(){return SYSTEM_time;}
WorldSYSTEM& Engine::Get_WorldSYSTEM(){return SYSTEM_world;}

bool Engine::Init_SYSTEMS()
{
	bool success = true;

	nlohmann::json config = SYSTEM_file.Read_Json_From_Files("configuration.json");
	SYSTEM_graphics.Init_Window("MyGameApp", 
		Get_JSON_Key<int>(config.at("window_size"), "x", 1920), Get_JSON_Key<int>(config.at("window_size"), "y", 1080));
	SYSTEM_graphics.Init_Renderer(Get_JSON_Key<int>(config.at("resolution_size"), "x", 1920), Get_JSON_Key<int>(config.at("resolution_size"), "y", 1080));

	SYSTEM_script.Init();
	SYSTEM_script.Run_Script_From_File("Content/Scripts/test.lua");
	SYSTEM_component.Register_Component_Type<TransformComponent>("Transform");
	SYSTEM_component.Register_Component_Type<VelocityComponent>("Velocity");
	SYSTEM_component.Register_Component_Type<TextureComponent>("Texture");
	SYSTEM_component.Register_Component_Type<CollisionBoxComponent>("CollisionBox");
	SYSTEM_component.Register_Component_Type<TextComponent>("Text");
	SYSTEM_component.Register_Component_Type<InputComponent>("Input");
	SYSTEM_component.Register_Component_Type<SpriteSheetComponent>("SpriteSheet");
	std::function<void(Entity, const std::string&)> renderBuilder = [this](Entity entiy, const std::string& componentInfo="")
	{
		RenderComponent renderComp;
		if(!componentInfo.empty()){
			renderComp = RenderComponent(componentInfo);
			auto jsonInfo = nlohmann::json::parse(componentInfo);
			std::string textureName = Get_JSON_Key<std::string>(jsonInfo, "name");
			this->Get_GraphicsSYSTEM().Set_Texture_Identification(renderComp.ID, textureName);
		}
		this->Get_ComponentSYSTEM().Add_Component<RenderComponent>(entiy, renderComp);
	};
	SYSTEM_component.Register_Component_Type<RenderComponent>("Render", renderBuilder);

	std::function<void(Entity, const std::string&)> tileMapBuilder = [this](Entity entiy, const std::string& componentInfo="")
	{
		TileMapComponent tileMapComp;
		if(!componentInfo.empty()){
			tileMapComp = TileMapComponent(componentInfo);
			auto jsonInfo = nlohmann::json::parse(componentInfo);
			std::string textureName = Get_JSON_Key<std::string>(jsonInfo.at("tilesets").at(0), "name");
			this->Get_GraphicsSYSTEM().Set_Texture_Identification(tileMapComp.tileset_ID, textureName);
		}
		this->Get_ComponentSYSTEM().Add_Component<TileMapComponent>(entiy, tileMapComp);
	};
	SYSTEM_component.Register_Component_Type<TileMapComponent>("Tilemap");

	nlohmann::json entityTypeJson = SYSTEM_file.Read_Json_From_Files("Content/Entities/init.json");
	SYSTEM_entity.Register_Entity_Types_From_Json(entityTypeJson);

	nlohmann::json textureJson = SYSTEM_file.Read_Json_From_Files("Content/Graphics/init.json");
	SYSTEM_graphics.Init_Textures_From_JSON(textureJson);

	nlohmann::json tileMapJson = SYSTEM_file.Read_Json_From_Files("Content/Maps/init.json");
	SYSTEM_world.Init_TileMaps_From_JSON(tileMapJson);
	return success;
}

bool Engine::Init_Variables()
{
	bool success = true;
	run = true;
	FPS_entity = SYSTEM_entity.Create_Entity_Of_Type("text");
	Entity player = SYSTEM_entity.Create_Entity_Of_Type("player");
	InputComponent inputComp;
	inputComp.action = [this](Entity ID)
	{
		const Uint8* currentKeyStates = SDL_GetKeyboardState( NULL );
        if( currentKeyStates[ SDL_SCANCODE_W ] )
        {Get_ComponentSYSTEM().Get_Component<VelocityComponent>(ID).y = -4;}
		else if (currentKeyStates[ SDL_SCANCODE_S ])
        {Get_ComponentSYSTEM().Get_Component<VelocityComponent>(ID).y = 4;}
		else if (currentKeyStates[ SDL_SCANCODE_A ])
        {Get_ComponentSYSTEM().Get_Component<VelocityComponent>(ID).x = -4;}
		else if (currentKeyStates[ SDL_SCANCODE_D ])
        {Get_ComponentSYSTEM().Get_Component<VelocityComponent>(ID).x = 4;}		
	};
	SYSTEM_component.Add_Component<InputComponent>(player, inputComp);
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
	SYSTEM_event.Update();
	SYSTEM_physics.Update();
}

void Engine::Render()
{	
	SYSTEM_graphics.Render_Clear();
	SYSTEM_graphics.Draw_Text(FPS_entity);
	SYSTEM_graphics.Render();
	SYSTEM_graphics.Render_Present();
}

bool Engine::Run_Game_Loop()
{
	uint64_t previous_time = SDL_GetTicks64();
	int lag = 0;
	TimeRepeaterComponent repeater(1000,0);
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
			intenal_variables.update_count += 1;
		}
		if(SYSTEM_time.Get_Reapeter_Signal(repeater))
		{
			std::string displayed_text = std::to_string(intenal_variables.frames_count) + "," + std::to_string(intenal_variables.update_count);
			SYSTEM_graphics.Update_String_Texture(FPS_entity, displayed_text, SYSTEM_graphics.Get_DefaultFont());
			intenal_variables.update_count = 0;
			intenal_variables.frames_count = 0;
		}
		Render();
		intenal_variables.frames_count += 1;
	}
	return 0;
}

void Engine::Shut_Down()
{
	run = false;
	SDL_Quit();
	Logger::close();
}
