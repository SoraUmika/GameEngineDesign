
#include <Engine.h>
#include <ComponentSYSTEM.h>
#include <Components.h>
#include <EntitySYSTEM.h>
#include <Logger.h>

int main(int argc, char* args[])
{
    
    Engine engine;
    engine.Init_Everything();
    engine.Run_Game_Loop();
    Grid<Entity> grid(5,10);
    grid.Insert_Element(0,0,5);
    grid.Insert_Element(0,0,4);
    grid.Insert_Element(0,0,8);
    
    std::vector<Entity>& cell = grid.Get_Cell(0,0);
    for (int i=0; i<cell.size(); i++)
    {
        Logger::log(LogLevel::INFO, "%d", cell[i]);
    }

    grid.Remove_Element(0,0,5);
    grid.Remove_Element(0,0,2);
    for (int i=0; i<cell.size(); i++)
    {
        Logger::log(LogLevel::INFO, "%d", cell[i]);
    }

    //JSON jsonData = fileSYSTEM.Read_Json_From_Files("Tiled Projects/myMap.json");
    //auto test = jsonData["layers"];
    //std::cout << jsonData.dump(4) << std::endl;
    return 0;
}
