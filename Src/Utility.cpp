#include <Utility.h>
/*
nlohmann::json Read_Json_From_Files(std::string path)
{
    SDL_RWops* file = SDL_RWFromFile(path.c_str(), "r");
    if (file == NULL)
    {
        Logger::log(LogLevel::ERROR, "Json file %s cannot be read! %s", path, SDL_GetError()); 
    }
    Sint64 fileSize = SDL_RWsize(file);

    if(fileSize == -1)
    {
        Logger::log(LogLevel::ERROR, "Json file %s size cannot be read! %s", path, SDL_GetError());
    }

    char* buffer = new char[fileSize + 1];
    Sint64 bytesRead = SDL_RWread(file, buffer, 1, fileSize);
    buffer[fileSize] = '\0';  // Add null terminator at the end

    // Create a string from the buffer
    std::string jsonString(buffer);

    // Parse the JSON string using nlohmann/json
    nlohmann::json jsonData = nlohmann::json::parse(jsonString);

    // Clean up resources
    delete[] buffer;
    SDL_RWclose(file);
    return jsonData;
}
*/

namespace DataStructure
{


}

namespace SpatialPartition
{
    Cell::Cell(){
        cell.reserve(16);
    }

    bool Cell::Insert(Entity ID){
        if(index_map.find(ID) == index_map.end(ID)){
            cell.push_back(ID);
            index_map[ID] = cell.size()-1;
            return true;
        }else{
            Logger::log(LogLevel::WARNING, "Entity %d is already in cell", ID);
            return false;
        }
    }
    bool Cell::Remove(Entity ID){
        auto it = index_map.find(ID);
        if(it != index_map.end()){
            std::swap(cell[it->second], cell.back());
            index_map[cell[it->second]] = it->second;
            cell.pop_back();
            index_map.erase(it);
            return true;
        }else{
            Logger::log(LogLevel::WARNING, "Removing non-existence entity %d from cell", ID);
            return false;              
        }
    }

    std::vector<Entity>& Cell::Get_Cell(){
        return cell;
    }
    std::unordered_map<Entity, size_t>& Cell::Get_Index_Map(){
        return index_map;
    }
    void Cell::Clear_Cell(){
        cell.clear();
        index_map.clear();
    }
    void Cell::Print_Cell(){
        for (int i=0; i< cell.size(); i++){
            std::cout << i << ":" << cell[i] << std::endl;
        }
        for(auto it=index_map.begin(); it != index_map.end(); ++it){
            std::cout << it->second << ":" << it->first << std::endl;
        }
        std::cout << std::endl;
    }
    size_t Cell::size(){
        return cell.size();
    }

    Grid::Grid(){};
    Grid::Grid(int width, int height)
    : width(width), height(height){
        cell_count_x = width/CELL_SIZE+1;
        cell_count_y = height/CELL_SIZE+1;
        cells.resize(cell_count_x*cell_count_y);
    }
    void Grid::Init(int width, int height){
        Grid(width, height);
    }
    void Grid::Insert(Entity ID, const RectComponent<float>& entity_rect){
        if(entity_rect.x < 0 || entity_rect.y < 0){
            Logger::log(LogLevel::ERROR,"Entity %d could not be inserted. Coordinates contains negative values.", ID);
            return;
        }
        OverlappedBoundary cellsRect = Get_OverlappedBoundary(entity_rect);
        if(cellsRect.end_cell_x >= cell_count_x || cellsRect.end_cell_y >= cell_count_y){
            Logger::log(LogLevel::ERROR,"Entity %d could not be inserted. Coordinates exceed grid dimensions.", ID);
            return;
        }
        for(int x = cellsRect.start_cell_x; x <= cellsRect.end_cell_x; x++){
            for(int y = cellsRect.start_cell_y; y <= cellsRect.end_cell_y; y++){
                int index = x * cell_count_y + y;
                if(!cells.at(index).Insert(ID)){
                    Logger::log(LogLevel::WARNING,"Entity %d cannot be inserted to cell [%d,%d].", ID, x,y);
                }
            }
        }
    }
    void Grid::Remove(Entity ID, const RectComponent<float>& entity_rect){
        if(entity_rect.x < 0 || entity_rect.y < 0){
            Logger::log(LogLevel::ERROR,"Entity %d could not be removed. Negative coordinates.", ID);
            return;
        }
        OverlappedBoundary cellsRect = Get_OverlappedBoundary(entity_rect);
        if(cellsRect.end_cell_x >= cell_count_x || cellsRect.end_cell_y >= cell_count_y){
            Logger::log(LogLevel::ERROR,"Entity %d could not be removed. Coordinates exceed grid dimensions.", ID);
            return;
        }
        for(int x = cellsRect.start_cell_x; x <= cellsRect.end_cell_x; x++){
            for(int y = cellsRect.start_cell_y; y <= cellsRect.end_cell_y; y++){
                int index = x * cell_count_y + y;
                if(!cells.at(index).Remove(ID)){
                    Logger::log(LogLevel::WARNING,"Entity cannot %d removed from cell [%d,%d].", ID, x,y);
                }
            }
        }     
    }
    void Grid::Move(Entity ID, const RectComponent<float>& entity_rect, const RectComponent<float>& target_rect){
        Remove(ID, entity_rect);
        Insert(ID, target_rect);
        
    }
    Cell& Grid::Get_Cell(int index_x, int index_y){
        return cells.at(index_x*cell_count_y+index_y);
    }

    std::vector<Cell>& Grid::Get_Cells(){
        return cells;
    }

    Grid::OverlappedBoundary Grid::Get_OverlappedBoundary(const RectComponent<float>& rect){
        OverlappedBoundary cellsRect{};
        cellsRect.start_cell_x = static_cast<int>(rect.x / CELL_SIZE);
        cellsRect.start_cell_y = static_cast<int>(rect.y / CELL_SIZE);
        cellsRect.end_cell_x = static_cast<int>((rect.x + rect.w) / CELL_SIZE);
        cellsRect.end_cell_y = static_cast<int>((rect.y + rect.h) / CELL_SIZE);
        return cellsRect;
    }

    void Grid::Print_Grid_Info(){
        for(int y=0; y < cell_count_y; y++){
            for(int x=0; x < cell_count_x; x++){
                int index = x*cell_count_y + y;
                int cell_size = cells.at(index).size();
                std::cout << cell_size << "\t";
            }
            std::cout << std::endl;
        }
    }
    SDL_Rect Grid::Get_Cell_Rect(int index_x, int index_y){
        return {index_x*CELL_SIZE, index_y*CELL_SIZE, CELL_SIZE, CELL_SIZE};
    }
    
    SDL_Point Grid::Get_Cell_Counts(){
        return {cell_count_x, cell_count_y};
    }
}

