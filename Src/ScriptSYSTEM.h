#ifndef SCRIPTSYSTEM_H
#define SCRIPTSYSTEM_H
#include <SYSTEM.h>
#include <sol/sol.hpp>
class ScriptSYSTEM: public SYSTEM
{
    public:
        ScriptSYSTEM(Engine& engine);
        
        void Init();
        void Run_Script_From_File(const std::string& script);
    private:
        sol::state lua;
        
};

#endif