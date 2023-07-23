#ifndef UTILITY_H
#define UTILITY_H
#include <Logger.h>
#include <JSON/json.hpp>
#include <SDL2/SDL.h>
#include <sol/sol.hpp>
nlohmann::json Read_Json_From_Files(std::string path);

template<typename T>
T Get_JSON_Key(const nlohmann::json& json, const std::string& key, const T& default_value=T{})
{
    if(json.contains(key))
    {
        try {
            return json[key].get<T>();
        } catch (const nlohmann::json::type_error& ex) {
            Logger::log(LogLevel::ERROR,"JSON - Type conversion Error for the key %s", key.c_str());
        } catch (const nlohmann::json::exception& ex) {
            Logger::log(LogLevel::ERROR,"JSON - Exception occurred for the key %s", key.c_str());
        }        
    }else
    {
        Logger::log(LogLevel::ERROR,"JSON - key '%s' does not exists!", key.c_str());
    }
    return default_value;
}

SDL_Rect Get_Rect_From_JSON_OBJ(const nlohmann::json& json);

#endif // !UTILITY_H