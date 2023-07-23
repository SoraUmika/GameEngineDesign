#ifndef SCRIPTSYSTEM_H
#define SCRIPTSYSTEM_H
#include <SYSTEM.h>
#include <sol/sol.hpp>
class ScriptSYSTEM: public SYSTEM
{
    public:
        ScriptSYSTEM(Engine& engine);
        
        void Init();
        void Init_Elementary_Type();
        void Init_SYSTEMS();
        void Run_Script_From_File(const std::string& script);

        sol::state& Get_Lua_State();
        template <typename T>
        T Get_Lua_Object(const std::string& object_name, T defaultValue)
        {
            try
            {
                sol::optional<T> lua_object = lua[object_name];
                
                if (lua_object)
                {
                    if constexpr (std::is_same<T, sol::table>::value)
                    {
                        if(Table_Is_Missing_Keys(lua_object.value(), defaultValue))
                        {
                            return defaultValue;
                        }
                    }
                    return lua_object.value();
                }
                else
                {
                    Logger::log(LogLevel::ERROR, "Lua object '%s' not found or of wrong type", object_name.c_str());
                }
            }
            catch(const std::exception& e)
            {
                Logger::log(LogLevel::ERROR, "Caught a sol::error exception: %s", e.what());
            }

            return defaultValue;
        }

        void Garbage_Collect();


    private:
        sol::state lua;
        bool Table_Is_Missing_Keys(const sol::table& lua_table, const sol::table& reference_table);
};

#endif