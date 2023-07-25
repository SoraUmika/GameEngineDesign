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
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
        Logger::log(LogLevel::ERROR, "SDL could not initialize! SDL_Error: %s", SDL_GetError());
		success = false;
	}
	else
	{
		this->window_size.x = window_Size_x;
		this->window_size.y = window_Size_y;
		this->window = SDL_CreateWindow(program_title.c_str(), SDL_WINDOWPOS_CENTERED, 
			SDL_WINDOWPOS_CENTERED, window_Size_x, window_Size_y, SDL_WINDOW_SHOWN);
		if (this->window == NULL)
		{
            Logger::log(LogLevel::ERROR, "Window could not be created! SDL_Error: %s", SDL_GetError());
			success = false;
		}
        else
        {
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
            default_font = TTF_OpenFont("Content/Fonts/default_font.ttf", 20);
            if (default_font == NULL){
                Logger::log(LogLevel::ERROR, "Failed to load default font! SDL_ttf Error: %s", TTF_GetError());
                success = false;
            }
            else{
                fonts.insert({"default", default_font});
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

bool GraphicsSYSTEM::Load_Texture_From_File(TextureComponent& textureComponent, const std::string& path){
    bool success = true;
    SDL_Surface* surface = IMG_Load(path.c_str());
    if(surface == NULL){
        Logger::log(LogLevel::ERROR, "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
    }
    else{
        SDL_Texture* raw_texture = SDL_CreateTextureFromSurface( renderer, surface );
        if(raw_texture == NULL){
            Logger::log(LogLevel::ERROR,"Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
            success = false;
        }
        textureComponent.texture = std::shared_ptr<SDL_Texture>(raw_texture, TextureComponent::SDLTextureDeleter());
        textureComponent.width = surface->w;
        textureComponent.height = surface->h;
        SDL_FreeSurface(surface);
        Logger::log(LogLevel::INFO, "Texture %s loaded", path.c_str());
    }
    return success;
}

void GraphicsSYSTEM::Register_Texture_From_File(const std::string& identifier, const std::string& path)
{
    if(texture_identifiers.find(identifier) != texture_identifiers.end()){
        Logger::log(LogLevel::ERROR,"Unable to register texture from file: identifier %s conflict error", identifier.c_str());     
    }
    else{
        TextureComponent textureComp;
        if(Load_Texture_From_File(textureComp, path)){
            textures[texture_index] = textureComp;
            texture_identifiers[identifier] = texture_index;
            Logger::log(LogLevel::INFO,"Texture with identifier %s identified with index %d", identifier.c_str(), texture_index);     
            texture_index++;
        }
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

bool GraphicsSYSTEM::Load_Texture_From_String(TextureComponent& textureComponent, const std::string& texture_text, const std::string& font, SDL_Color text_color){
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
            textureComponent.texture = std::shared_ptr<SDL_Texture>(raw_texture, TextureComponent::SDLTextureDeleter());
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
    Draw_Texture(transform, textures[renderInfo.index], renderInfo.scale, clip);
}

void GraphicsSYSTEM::Render_Clear(){
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer);
}

void GraphicsSYSTEM::Render_Present(){SDL_RenderPresent(renderer);}

void GraphicsSYSTEM::Render(){

    auto& compSYSTEM = engine.Get_ComponentSYSTEM();
    ComponentArray<RenderInfoComponent>& renderInfo_comp_array = compSYSTEM.Get_Component_Array<RenderInfoComponent>();
    ComponentArray<TransformComponent>& transform_comp_array = compSYSTEM.Get_Component_Array<TransformComponent>();
    auto& render_entities_list = renderInfo_comp_array.Get_Entities();

    for(int i=0; i<render_entities_list.size(); i++)
    {
        Entity entity = render_entities_list[i];
        auto& transform = transform_comp_array.Get_Data(entity);
        auto& renderInfo = renderInfo_comp_array.Get_Data(entity);
        Draw_RenderComp(transform, renderInfo);
    }

    
    ComponentArray<RigidBodyComponent>& rigidBody_comp_array = compSYSTEM.Get_Component_Array<RigidBodyComponent>();
    auto& rigidBody_entities_list = rigidBody_comp_array.Get_Entities();
    for(int i=0; i<rigidBody_entities_list.size(); i++)
    {
        Entity entity = rigidBody_entities_list[i];
        auto& rigidBody = rigidBody_comp_array.Get_Data(entity);
        Draw_Rect(rigidBody.rect);
    }
    
}

void GraphicsSYSTEM::Update_Texture_From_Text(Entity ID, const std::string& text, const std::string& font, SDL_Color text_color){
    Load_Texture_From_String(engine.Get_ComponentSYSTEM().Get_Component<TextureComponent>(ID), text, font, text_color);
}