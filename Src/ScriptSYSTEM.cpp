#include <ScriptSYSTEM.h>
#include <Engine.h>

ScriptSYSTEM::ScriptSYSTEM(Engine& engine): SYSTEM(engine) 
{

}

void ScriptSYSTEM::Init_SYSTEMS()
{
    lua.new_usertype<ComponentSYSTEM>("ComponentSYSTEM",
        "Add_Component", sol::overload(&ComponentSYSTEM::Add_Component<TransformComponent>,
                                       &ComponentSYSTEM::Add_Component<VelocityComponent>,
                                       &ComponentSYSTEM::Add_Component<RenderInfoComponent>,
                                       &ComponentSYSTEM::Add_Component<RigidBodyComponent>,
                                       &ComponentSYSTEM::Add_Component<EventListenerComponent>,
                                       &ComponentSYSTEM::Add_Component<TimedStateMachineComponent>
                                       )
    );
    lua.new_usertype<EntitySYSTEM>("EntitySYSTEM",
        "Create_Entity", &EntitySYSTEM::Create_Entity
    );
    lua.new_usertype<GraphicsSYSTEM>("GraphicsSYSTEM",
        "Create_Texture", sol::overload(
            sol::resolve<bool(TextureComponent&, Uint32, int, int, int)>(&GraphicsSYSTEM::Create_Texture),
            sol::resolve<bool(TextureComponent&, const std::string&)>(&GraphicsSYSTEM::Create_Texture)),
        "Register_Texture_From_File", &GraphicsSYSTEM::Register_Texture_From_File,
        "Set_RenderInfo_Texture", &GraphicsSYSTEM::Set_RenderInfo_Texture
    );

    lua.new_usertype<EventSYSTEM>("EventSYSTEM", 
       "Register_New_Event_Type", &EventSYSTEM::Register_Event_Type,
       "Get_Event_Type", &EventSYSTEM::Get_Event_Type,
       "Push_Event", &EventSYSTEM::Push_Event);

    lua.new_usertype<TimeSYSTEM>("TimeSYSTEM",
        "Update_Time_Tracker", &TimeSYSTEM::Update_Time_Tracker
    );

    lua.new_usertype<WorldSYSTEM>("WorldSYSTEM",
        "Register_Scene", &WorldSYSTEM::Register_Scene,
        "Add_Entity_To_Scene", &WorldSYSTEM::Add_Entity_To_Scene,
        "Set_Current_Scene", sol::overload(sol::resolve<void(size_t)>(&WorldSYSTEM::Set_Current_Scene),
                                            sol::resolve<void(const std::string&)>(&WorldSYSTEM::Set_Current_Scene)),
        "Create_New_Scene", &WorldSYSTEM::Create_New_Scene
    );
    lua.new_usertype<Engine>("Engine",
        "Get_ComponentSYSTEM", &Engine::Get_ComponentSYSTEM,
        "Get_EntitySYSTEM", &Engine::Get_EntitySYSTEM,
        "Get_GraphicsSYSTEM", &Engine::Get_GraphicsSYSTEM,
        "Get_EventSYSTEM", &Engine::Get_EventSYSTEM,
        "Get_TimeSYSTEM", &Engine::Get_TimeSYSTEM,
        "Get_WorldSYSTEM", &Engine::Get_WorldSYSTEM
    );
    lua["Engine"] = &engine;
}

void ScriptSYSTEM::Init_Elementary_Type()
{
    lua.new_usertype<KeyboardStateEvent>("KeyboardStateEvent", "scancode", &KeyboardStateEvent::scancode);
    
    lua.new_usertype<SDL_Keysym>("SDL_Keysym", "sym", &SDL_Keysym::sym, "scancode", &SDL_Keysym::scancode, "mod", &SDL_Keysym::mod);
    lua.new_usertype<SDL_KeyboardEvent>("SDL_KeyboardEvent", "keysym", &SDL_KeyboardEvent::keysym, "state", &SDL_KeyboardEvent::state,
        "repeats", &SDL_KeyboardEvent::repeat, "timestamp", &SDL_KeyboardEvent::timestamp);

    lua.new_usertype<SDL_MouseMotionEvent>("SDL_MouseMotionEvent", "x", &SDL_MouseMotionEvent::x, "y", &SDL_MouseMotionEvent::y);
    lua.new_usertype<SDL_MouseButtonEvent>("SDL_MouseButtonEvent", "x", &SDL_MouseButtonEvent::x, "y", &SDL_MouseButtonEvent::y,
        "state", &SDL_MouseButtonEvent::state, "clicks", &SDL_MouseButtonEvent::clicks, "button", &SDL_MouseButtonEvent::button);

    lua.new_usertype<Event>("Event", "type", &Event::type, "data", &Event::data);
    lua.new_usertype<SDL_Rect>("Rectangle", "x", &SDL_Rect::x,"y", &SDL_Rect::y,"w", &SDL_Rect::w,"h", &SDL_Rect::h);
    lua.new_usertype<SDL_Color>("Color", "a", &SDL_Color::a, "r", &SDL_Color::r, "g", &SDL_Color::g, "b", &SDL_Color::b);
    lua.new_usertype<SDL_Point>("Point", "x", &SDL_Point::x, "y", &SDL_Point::y);
}

void ScriptSYSTEM::Init()
{
    lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math);
    lua.script(R"(package.path = 'Content/Scripts/?.lua;' .. package.path)");

    Init_Elementary_Type();
    Init_SYSTEMS();
}

sol::state& ScriptSYSTEM::Get_Lua_State()
{
    return this->lua;
}

void ScriptSYSTEM::Run_Script_From_File(const std::string& script)
{
    try{
        lua.script_file(script);
    }
    catch(const std::exception& e){
        Logger::log(LogLevel::ERROR, "Caught a sol::error exception: %s", e.what());
    }
}

void ScriptSYSTEM::Garbage_Collect()
{
    lua.collect_garbage();
}

bool ScriptSYSTEM::Table_Is_Missing_Keys(const sol::table& lua_table, const sol::table& reference_table)
{
    bool missing_keys = false;
    
    reference_table.for_each([&](sol::object const& key, sol::object const& value) {
        auto result = lua_table.get_or<sol::optional<sol::object>>(key, sol::nullopt);
        if (!result) {
            missing_keys = true;
            Logger::log(LogLevel::ERROR, "Key %s does not exists in target table", key.as<std::string>().c_str());
        }
    });

    return missing_keys;
}
