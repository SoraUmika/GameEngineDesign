#include <GraphicsSYSTEM.h>
#include <Engine.h>
GraphicsSYSTEM::GraphicsSYSTEM(Engine& engine): SYSTEM(engine)
{
    renderer = NULL;
    window = NULL;
    default_font = NULL;
}

GraphicsSYSTEM::~GraphicsSYSTEM()
{
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    TTF_CloseFont(default_font);
    renderer = NULL;
    window = NULL;
    default_font = NULL;
    TTF_Quit();
    IMG_Quit();  
}

bool GraphicsSYSTEM::Init_Window(const std::string& program_title, int window_Size_x, int window_Size_y)
{
	bool success = true;
	if (SDL_Init(SDL_INIT_VIDEO) < 0){
        Logger::log(LogLevel::ERROR, "SDL could not initialize! SDL_Error: %s", SDL_GetError());
		success = false;
	}
	else{
		this->window_size.x = window_Size_x;
		this->window_size.y = window_Size_y;
		this->window = SDL_CreateWindow(program_title.c_str(), SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED, window_Size_x, window_Size_y, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
		if (this->window == NULL){
            Logger::log(LogLevel::ERROR, "Window could not be created! SDL_Error: %s", SDL_GetError());
			success = false;
		}
        else{
            Logger::log(LogLevel::INFO, "Window successfully initialized with width:%d height:%d", window_Size_x, window_Size_y);
        }
	}
	return success;
}

bool GraphicsSYSTEM::Init_Renderer(int logical_Res_x, int logical_Res_y, bool use_vsync){
    bool success = true;
    Uint32 flags = SDL_RENDERER_ACCELERATED;
    if(use_vsync){flags |= SDL_RENDERER_PRESENTVSYNC;}
    renderer = SDL_CreateRenderer(
        window, -1, flags
    );
    if (renderer == NULL){
        Logger::log(LogLevel::ERROR, "Renderer could not be created! SDL_Error: %s", SDL_GetError());
        success = false;
    }
    else{
        resolution = { logical_Res_x, logical_Res_y };
        SDL_RenderSetLogicalSize(renderer, logical_Res_x, logical_Res_y);	
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        Logger::log(LogLevel::INFO, "Renderer successfully initialized with width:%d height:%d", logical_Res_x, logical_Res_y);

        int imgFlags = IMG_INIT_PNG;
        if (!(IMG_Init(imgFlags) & imgFlags)){
            Logger::log(LogLevel::ERROR, "SDL_image could not initialize! SDL_image Error: %s", IMG_GetError());
            success = false;
        }

        if (TTF_Init() == -1){
            Logger::log(LogLevel::ERROR, "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
            success = false;
        }
        else{
            int size = 20;
            default_font = TTF_OpenFont("Content/Fonts/default_font.ttf", size);
            if (default_font == NULL){
                Logger::log(LogLevel::ERROR, "Failed to load default font! SDL_ttf Error: %s", TTF_GetError());
                success = false;
            }
            else{
                fonts.insert({"default", default_font});
                Construct_Text_Sprites("default_font", default_font, size);
            }
            int index;
            for (unsigned char currentChar = 32; currentChar < 255; currentChar++){
                if (currentChar == 127){
                    currentChar = 159;
                    continue;
                }
                index = TTF_GlyphIsProvided(default_font, currentChar);
                if (!index)
                    Logger::log(LogLevel::ERROR, "There is no '%c' in the loaded font! ASCII: %i", currentChar, currentChar);
            }
        }
    }
    return success;
}

SDL_Renderer* GraphicsSYSTEM::Get_Renderer(){return renderer;}
SDL_Window* GraphicsSYSTEM::Get_Window(){return window;}
TTF_Font* GraphicsSYSTEM::Get_Font(const std::string& font){return fonts.at("default");}

bool GraphicsSYSTEM::Create_Texture(TextureComponent& texture_component, Uint32 format,int access, int w,int h){
    bool success = true;
    SDL_Texture* raw_texture = SDL_CreateTexture(renderer, format, access, w, h);
    if(raw_texture == NULL){
        Logger::log(LogLevel::ERROR, "Fail to create texture: %s", IMG_GetError());
        success = false;
    }else{
        texture_component.texture = std::unique_ptr<SDL_Texture, TextureComponent::SDLTextureDeleter>(raw_texture);
        texture_component.width = w;
        texture_component.height = h;
    }
    return success;
}

bool GraphicsSYSTEM::Create_Texture(TextureComponent& textureComponent, const std::string& path){
    bool success = true;
    SDL_Surface* surface = IMG_Load(path.c_str());
    if(surface == NULL){
        Logger::log(LogLevel::ERROR, "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
        success = false;
    }
    else{
        SDL_Texture* raw_texture = SDL_CreateTextureFromSurface( renderer, surface );
        if(raw_texture == NULL){
            Logger::log(LogLevel::ERROR,"Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
            success = false;
        }
        textureComponent.texture = std::unique_ptr<SDL_Texture, TextureComponent::SDLTextureDeleter>(raw_texture);
        textureComponent.width = surface->w;
        textureComponent.height = surface->h;
        SDL_FreeSurface(surface);
        Logger::log(LogLevel::INFO, "Texture %s loaded", path.c_str());
    }
    return success;
}

void GraphicsSYSTEM::Construct_Text_Sprites(const std::string& identifier, TTF_Font* font, int font_size){
    TextureComponent character_texture;
    const int character_count = 128;
    if(Create_Texture(character_texture, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, font_size*character_count, font_size)){
        Set_Render_Target(character_texture);
        //SDL_SetTextureBlendMode(character_texture.texture.get(), SDL_BLENDMODE_BLEND);
        SDL_Color text_color = {0,0,0,255};
        int x = 0;
        for(int i=0; i<character_count; i++){
            
            const char character[] = { static_cast<char>(i), '\0' };
            SDL_Surface* surface = TTF_RenderText_Solid(font, character, text_color);
            if( surface == NULL ){
                Logger::log(LogLevel::ERROR,"Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
            }
            else{
                SDL_Texture* char_texture = SDL_CreateTextureFromSurface( renderer, surface );
                if(char_texture == NULL){
                    Logger::log(LogLevel::ERROR,"Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
                }
                Logger::log(LogLevel::INFO,"character: %s, width: %d, height:%d", character, surface->w, surface->h);
                SDL_Rect renderQuad = { x, 0, surface->w, surface->h };
                
                SDL_RenderCopy(renderer, char_texture, NULL, &renderQuad);
                x += surface->w;
                SDL_FreeSurface( surface );
            }
        }
        Set_Render_Target();
        Register_Texture(identifier, character_texture);
    }
}

void GraphicsSYSTEM::Set_Render_Target(const TextureComponent& texture){
    SDL_SetRenderTarget(renderer, texture.texture.get());
}
void GraphicsSYSTEM::Set_Render_Target(){
    SDL_SetRenderTarget(renderer, NULL);
}

void GraphicsSYSTEM::Set_Renderer_Color(SDL_Color color){SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);}

void GraphicsSYSTEM::Draw_Rect(SDL_Rect& rect, SDL_Color color){
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawRect(renderer, &rect);
}

void GraphicsSYSTEM::Draw_Line(SDL_Point pos1, SDL_Point pos2){SDL_RenderDrawLine(renderer, pos1.x, pos1.y, pos2.x, pos2.y);}

void GraphicsSYSTEM::Draw_Circle(SDL_Point position, unsigned int radius, SDL_Color color){
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    int segments = 200;
    double angle_per_seg = 2.0*M_PI/segments;
    double angle = 0;
    SDL_Point pos1 = position;
    SDL_Point pos2 = position;

    pos1.x += radius;
    for(int i=0; i<segments; i++){
        angle = angle_per_seg*i;
        pos1.x = position.x + radius*SDL_cos(angle);
        pos1.y = position.y + radius*SDL_sin(angle);
        pos2.x = position.x + radius*SDL_cos(angle+angle_per_seg);
        pos2.y = position.y + radius*SDL_sin(angle+angle_per_seg);
        Draw_Line(pos1,pos2);
    }
}

size_t GraphicsSYSTEM::Register_Texture(const std::string& identifier, TextureComponent& texture){
    if(texture_identifiers.find(identifier) != texture_identifiers.end()){
        Logger::log(LogLevel::ERROR,"Unable to register texture from file: identifier %s conflict error", identifier.c_str());
        return 0;     
    }
    textures[texture_index] = std::move(texture);
    texture_identifiers[identifier] = texture_index;
    Logger::log(LogLevel::INFO,"Texture with identifier %s identified with index %d", identifier.c_str(), texture_index);  
    texture_index++;
    return texture_identifiers[identifier];
}

size_t GraphicsSYSTEM::Register_Texture_From_File(const std::string& identifier, const std::string& path){
    if(texture_identifiers.find(identifier) != texture_identifiers.end()){
        Logger::log(LogLevel::ERROR,"Unable to register texture from file: identifier %s conflict error", identifier.c_str());
        return 0; 
    }
    else{
        TextureComponent textureComp;
        if(Create_Texture(textureComp, path)){
            return Register_Texture(identifier, textureComp);
        }
        return 0;
    }
}

size_t GraphicsSYSTEM::Get_Texture_By_Identifier(const std::string& identifier){
    if(texture_identifiers.find(identifier) == texture_identifiers.end()){
        Logger::log(LogLevel::ERROR,"Getting texture failed with identifier %s", identifier.c_str());
        return 0;    
    }else{
        return texture_identifiers[identifier];
    }
}

void GraphicsSYSTEM::Set_RenderInfo_Texture(const std::string& identifier, RenderInfoComponent& renderInfo)
{
    if(texture_identifiers.find(identifier) == texture_identifiers.end()){
        Logger::log(LogLevel::ERROR,"Unable to set texture to renderInfo, identifier %s not found", identifier.c_str());     
    }
    else{
        size_t index = texture_identifiers.at(identifier);
        renderInfo.index = index;
        Logger::log(LogLevel::INFO,"RenderInfo index set by identifier %s with index value %d", identifier.c_str(), index);
    }
}

bool GraphicsSYSTEM::Create_Texture_From_String(TextureComponent& textureComponent, const std::string& texture_text, const std::string& font, SDL_Color text_color){
    SDL_Surface* textSurface = TTF_RenderText_Solid(fonts.at(font),texture_text.c_str(), text_color);
    if( textSurface == NULL ){
        Logger::log(LogLevel::ERROR,"Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
    }
    else{
        SDL_Texture* raw_texture = SDL_CreateTextureFromSurface( renderer, textSurface );
        if(raw_texture == NULL){
            Logger::log(LogLevel::ERROR,"Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
        }
        else{
            textureComponent.texture = std::unique_ptr<SDL_Texture, TextureComponent::SDLTextureDeleter>(raw_texture);
            textureComponent.width = textSurface->w;
            textureComponent.height = textSurface->h;
        }
        SDL_FreeSurface( textSurface );
    }      
    return textureComponent.texture != NULL;
}

void GraphicsSYSTEM::Draw_Texture(const TransformComponent& transformComponent, const TextureComponent& textureComponent, double scale, const SDL_Rect* clip,
    const SDL_Point* center, const SDL_RendererFlip flip)
{
	SDL_Rect renderQuad = { static_cast<int>(std::round(transformComponent.x)), static_cast<int>(std::round(transformComponent.y)), 
        static_cast<int>(std::round(textureComponent.width*scale)), static_cast<int>(std::round(textureComponent.height*scale))};
	if (clip != NULL){
		renderQuad.w = static_cast<int>(std::round(clip->w*scale));
		renderQuad.h = static_cast<int>(std::round(clip->h*scale));
	}
	SDL_RenderCopyEx(renderer, textureComponent.texture.get(), 
		clip, &renderQuad, transformComponent.angle, center, flip);
}

void GraphicsSYSTEM::Draw_RenderComp(const TransformComponent& transform, const RenderInfoComponent& renderInfo){

    const SDL_Rect* clip = NULL;
    if(renderInfo.is_clip){
        clip = &renderInfo.clip;
    }
    TransformComponent transform_ = transform;
    transform_.x += renderInfo.offset_x; 
    transform_.y += renderInfo.offset_y;
    Draw_Texture(transform_, textures[renderInfo.index], renderInfo.scale, clip);
}

void GraphicsSYSTEM::Render_Clear(){
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer);
}

void GraphicsSYSTEM::Render_Present(){
    SDL_RenderPresent(renderer);
}

void GraphicsSYSTEM::Render(){
    for(std::function<void(void)> render_task : render_tasks){
        render_task();
    }
}

void GraphicsSYSTEM::Update_Texture_From_Text(Entity ID, const std::string& text, const std::string& font, SDL_Color text_color){
    Create_Texture_From_String(engine.Get_ComponentSYSTEM().Get_Component<TextureComponent>(ID), text, font, text_color);
}

void GraphicsSYSTEM::Push_Render_Task(const std::string& task_identifier, std::function<void(void)> task){
    render_tasks.push_back(task);
    render_tasks_map[task_identifier] = render_tasks.size()-1;
}

void GraphicsSYSTEM::Render_Scene(Scene& scene){
    std::sort(scene.entities.begin(), scene.entities.end(), [this](Entity entity1, Entity entity2){
        ComponentSYSTEM& componentSYSTEM = engine.Get_ComponentSYSTEM();
        TransformComponent& transform1 = componentSYSTEM.Get_Component<TransformComponent>(entity1);
        TransformComponent& transform2 = componentSYSTEM.Get_Component<TransformComponent>(entity2);
        RenderInfoComponent& renderInfo1 = componentSYSTEM.Get_Component<RenderInfoComponent>(entity1);
        RenderInfoComponent& renderInfo2 = componentSYSTEM.Get_Component<RenderInfoComponent>(entity2);
        return (transform1.y - renderInfo1.clip.h) < (transform2.y - renderInfo2.clip.h) ;
    });

    SDL_Point cell_count = scene.grid.Get_Cell_Counts();

    TextureComponent textTexture;
    Create_Texture_From_String(textTexture, std::to_string(0));
    for(int j=0; j<cell_count.y; j++){
        for(int i=0; i<cell_count.x; i++){
            SDL_Rect cell_rect = scene.grid.Get_Cell_Rect(i,j);
            Draw_Rect(cell_rect, {0,0,0, 255});
            size_t entity_count = scene.grid.Get_Cell(i,j).size();
            //Load_Texture_From_String(textTexture, std::to_string(entity_count));
            TransformComponent text_transform = TransformComponent(cell_rect.x+5, cell_rect.y+5, 0);
            Draw_Texture(text_transform, textTexture);
        }
    }

    auto& SYSTEM_graphics = engine.Get_GraphicsSYSTEM();
    ComponentSYSTEM& componentSYSTEM = engine.Get_ComponentSYSTEM();
    for(Entity entity : scene.entities){
        if(componentSYSTEM.Entity_Has_Component<RenderInfoComponent>(entity)){
            TransformComponent& transform_ = componentSYSTEM.Get_Component<TransformComponent>(entity);
            RenderInfoComponent& renderInfo_ = componentSYSTEM.Get_Component<RenderInfoComponent>(entity);
            SYSTEM_graphics.Draw_RenderComp(transform_, renderInfo_);
        }
        if(componentSYSTEM.Entity_Has_Component<RigidBodyComponent>(entity)){
            RigidBodyComponent& rigid_body = componentSYSTEM.Get_Component<RigidBodyComponent>(entity);
            SYSTEM_graphics.Draw_Rect(rigid_body.rect);
        }
    }
    
    size_t char_index = texture_identifiers.at("default_font");
    Draw_Texture(TransformComponent(0,0,0), textures[char_index]);
}