#include <WorldSYSTEM.h>
#include <Engine.h>
WorldSYSTEM::WorldSYSTEM(Engine& engine):SYSTEM(engine)
{

}

WorldSYSTEM::~WorldSYSTEM()
{

}

void WorldSYSTEM::Init_TileMaps_From_JSON(const nlohmann::json& json)
{
    for(auto& tilemap: json.items())
    {
        std::string mapName = tilemap.key();
        std::string path = Get_JSON_Key<std::string>(tilemap.value(), "path");
        nlohmann::json tilemapJSON = engine.Get_FileSYSTEM().Read_Json_From_Files(path);
        Entity mapID = engine.Get_EntitySYSTEM().Create_Entity();
        engine.Get_ComponentSYSTEM().Add_Component_By_Name(mapID, "Tilemap", tilemapJSON.dump());
    }
}