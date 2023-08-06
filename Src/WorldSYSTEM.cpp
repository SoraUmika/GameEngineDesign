#include <WorldSYSTEM.h>
#include <Engine.h>
WorldSYSTEM::WorldSYSTEM(Engine& engine):SYSTEM(engine){
    //camera = engine.Get_EntitySYSTEM().Create_Entity();
    //engine.Get_ComponentSYSTEM().Add_Component<TransformComponent>(camera, TransformComponent(0,0,0));
    current_scene_index = Create_New_Scene("Default", 2000, 2000);
}

WorldSYSTEM::~WorldSYSTEM()
{

}

size_t WorldSYSTEM::Create_New_Scene(const std::string& name, int width, int height){
    if(scene_name_to_scene_index.find(name) != scene_name_to_scene_index.end()){
        Logger::log(LogLevel::ERROR,"Scene name %s already registered?", name.c_str());
        return 0;
    }
    size_t scene_index = scenes.size();
    Scene newScene = Scene(name, scene_index, width, height);
    scenes.emplace_back(newScene);
    scene_name_to_scene_index.insert({name, scene_index});
    Logger::log(LogLevel::INFO, "Scene %s created with index %d", name.c_str(), scene_index);
    return scene_index;
}


void WorldSYSTEM::Register_Scene(const std::string& name){
    /*
    if(scene_name_to_scene_index.find(name) != scene_name_to_scene_index.end()){
        Logger::log(LogLevel::ERROR,"Tilemap name %s already registered?", name.c_str());
        return;
    }
    int width = tilemap["width"];
    int height = tilemap["height"];
    int tilewidth = tilemap["tilewidth"];
    int tileheight = tilemap["tileheight"];
    std::string tileset_name = tilemap["tilesets"][1]["name"];
    sol::table layers = tilemap["layers"];
    TileMapComponent tmp_map(width, height);

    tmp_map.tileset_ID = engine.Get_GraphicsSYSTEM().Get_Texture_By_Identifier(tileset_name);
    tmp_map.map_name = name;
    tmp_map.layers.reserve(layers.size());
    for (int i = 1; i <= layers.size(); ++i) {
        sol::table layer = layers[i];
        TileLayer tile_layer;
        tile_layer.name = layer["name"];
        tile_layer.tile_x_count = layer["width"];
        tile_layer.tile_y_count = layer["height"];
        sol::object data = layer["data"];
        tile_layer.data = data.as<std::vector<int>>();
        tmp_map.layers.emplace_back(tile_layer);
    }
    scenes.emplace_back(tmp_map);
    size_t scene_index = scenes.size() - 1;
    scene_name_to_scene_index.insert({name, scene_index});
    current_scene_index = scene_index;
    Logger::log(LogLevel::INFO, "Tilemap %s registered with index %d", name.c_str(), scene_index);
    */ 
}

void WorldSYSTEM::Add_Entity_To_Scene(const std::string& name, Entity entity){
    if(scene_name_to_scene_index.find(name) == scene_name_to_scene_index.end()){
        Logger::log(LogLevel::WARNING, "Cannot add entity %d to tile map %s, map does not exists", entity, name.c_str());
        return;        
    }
    size_t scene_index = scene_name_to_scene_index.at(name);
    entity_to_scene_index[entity] = scene_index;
    scenes.at(scene_index).entities.push_back(entity);
    if(engine.Get_ComponentSYSTEM().Entity_Has_Component<RigidBodyComponent>(entity)){
        RigidBodyComponent& rigid_body = engine.Get_ComponentSYSTEM().Get_Component<RigidBodyComponent>(entity);
        scenes.at(scene_index).grid.Insert(entity, rigid_body.rect);
    }
    Logger::log(LogLevel::INFO, "Added entity %d to tile map %s with index %d", entity, name.c_str(), scene_index);
}

void WorldSYSTEM::Remove_Entity_From_Scene(const std::string& name, Entity entity){
    /*
    if(scenes.find(name) == scenes.end()){
        Logger::log(LogLevel::WARNING, "Cannot remove entity %d from tile map %s, map does not exists", entity, name.c_str());
        return;        
    }
    */
}
        
size_t WorldSYSTEM::Get_Current_Map(){
    return current_scene_index;
}

std::vector<Scene>& WorldSYSTEM::Get_Scenes(){
    return scenes;
}

void WorldSYSTEM::Set_Current_Scene(const std::string& name){
    if(scene_name_to_scene_index.find(name) == scene_name_to_scene_index.end()){
        Logger::log(LogLevel::WARNING, "Cannot set current scene by name, scene not found %s", name.c_str());
        return;        
    }
    current_scene_index = scene_name_to_scene_index[name];
}


void WorldSYSTEM::Set_Current_Scene(size_t index){
    if(index >= scenes.size()){
        Logger::log(LogLevel::WARNING, "Cannot set current scene by index %d, index not found", index);
        return;        
    }
    current_scene_index = index;
}

std::unordered_map<Entity, size_t>& WorldSYSTEM::Get_Entity_Map(){
    return entity_to_scene_index;
}

void WorldSYSTEM::Lock_Camara_To_Entity(Entity entity){
    camera.target = entity;
    camera.lock = true;
}

void WorldSYSTEM::Update()
{   
    Scene& scene = scenes.at(current_scene_index);
    engine.Get_PhysicsSYSTEM().Update_Scene_Physics(scene);
}

void WorldSYSTEM::Render()
{
    /*
    int tile_column_count = 140;
    int tile_row_count = 150;

    RenderInfoComponent renderInfo;
    TransformComponent transform;
    renderInfo.is_clip = true;
    renderInfo.index = map.tileset_ID;
    renderInfo.scale = 2.0;
    renderInfo.offset_x = 0;
    renderInfo.offset_y = -300;

    for(int i=0; i< map.layers.size(); i++){
        const TileLayer& layer = map.layers.at(i);
        for(int y=0; y<layer.tile_y_count; y++){
            for(int x=0; x<layer.tile_x_count; x++){
                int index = layer.data.at(y*layer.tile_x_count+x)-1;
                if(index < 0){continue;}
                int index_x = index % tile_column_count;
                int index_y = index / tile_column_count;
                renderInfo.clip = {index_x*16,index_y*16, 16,16};
                transform.x = x*16*renderInfo.scale;
                transform.y = y*16*renderInfo.scale;
                SYSTEM_graphics.Draw_RenderComp(transform, renderInfo);
            }
        }
    }    
    */
    Scene& scene = scenes.at(current_scene_index);
    engine.Get_GraphicsSYSTEM().Render_Scene(scene);
}
