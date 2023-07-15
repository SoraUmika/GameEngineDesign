#include <ScriptSYSTEM.h>
#include <Engine.h>

ScriptSYSTEM::ScriptSYSTEM(Engine& engine): SYSTEM(engine) 
{

}

void ScriptSYSTEM::Init()
{
    lua.open_libraries(sol::lib::base, sol::lib::package);
    lua["Create_Entity"] = [&](){return engine.Get_EntitySYSTEM().Create_Entity();};
    lua["Add_Component_By_Name"] = [&]()
    {
        
    };
    
}

void ScriptSYSTEM::Run_Script_From_File(const std::string& script)
{
    try
    {
        lua.script_file(script);
    }
    catch(const std::exception& e)
    {
        Logger::log(LogLevel::ERROR, "Caught a sol::error exception: %s", e.what());
    }
}