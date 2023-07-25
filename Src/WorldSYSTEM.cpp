#include <WorldSYSTEM.h>
#include <Engine.h>
WorldSYSTEM::WorldSYSTEM(Engine& engine):SYSTEM(engine)
{

}

WorldSYSTEM::~WorldSYSTEM()
{

}

void WorldSYSTEM::Register_Tilemap(const sol::table& tilemap){
    int width = tilemap["width"];
    int height = tilemap["height"];
    int tilewidth = tilemap["tilewidth"];
    int tileheight = tilemap["tileheight"];
    int a = 1+1;
    
}

void WorldSYSTEM::Update()
{
    
}

void WorldSYSTEM::Render()
{
    
}
