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

bool GraphicsSYSTEM::Init_Window(const char* program_title, int window_Size_x, int window_Size_y)
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
		this->window = SDL_CreateWindow(program_title, SDL_WINDOWPOS_CENTERED, 
			SDL_WINDOWPOS_CENTERED, window_Size_x, window_Size_y, SDL_WINDOW_SHOWN);
		if (this->window == NULL)
		{
            Logger::log(LogLevel::ERROR, "Window could not be created! SDL_Error: %s", SDL_GetError());
			success = false;
		}
	}
	return success;
}

bool GraphicsSYSTEM::Init_Renderer(int logical_Res_x, int logical_Res_y)
{
    bool success = true;
    renderer = SDL_CreateRenderer
    (
        window, -1, SDL_RENDERER_ACCELERATED
    );
    if (renderer == NULL)
    {
        Logger::log(LogLevel::ERROR, "Renderer could not be created! SDL_Error: %s", SDL_GetError());
        success = false;
    }
    else
    {
        resolution = { logical_Res_x, logical_Res_y };
        SDL_RenderSetLogicalSize(renderer, logical_Res_x, logical_Res_y);	
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

        int imgFlags = IMG_INIT_PNG;
        if (!(IMG_Init(imgFlags) & imgFlags))
        {
            Logger::log(LogLevel::ERROR, "SDL_image could not initialize! SDL_image Error: %s", IMG_GetError());
            success = false;
        }

        if (TTF_Init() == -1)
        {
            Logger::log(LogLevel::ERROR, "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
            success = false;
        }
        else
        {
            default_font = TTF_OpenFont("Content/Fonts/default_font.ttf", 20);
            if (default_font == NULL)
            {
                Logger::log(LogLevel::ERROR, "Failed to load default font! SDL_ttf Error: %s", TTF_GetError());
                success = false;
            }
            int index;
            for (unsigned char currentChar = 32; currentChar < 255; currentChar++)
            {
                if (currentChar == 127)
                {
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
TTF_Font* GraphicsSYSTEM::Get_DefaultFont(){return default_font;}

void GraphicsSYSTEM::Set_Renderer_Color(SDL_Color color){SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);}

void GraphicsSYSTEM::Draw_Rect(SDL_Rect rect){SDL_RenderDrawRect(renderer, &rect);}
void GraphicsSYSTEM::Draw_Line(SDL_Point pos1, SDL_Point pos2){SDL_RenderDrawLine(renderer, pos1.x, pos1.y, pos2.x, pos2.y);}
void GraphicsSYSTEM::Draw_Circle(SDL_Point position, unsigned int radius, SDL_Color color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    int segments = 200;
    double angle_per_seg = 2.0*M_PI/segments;
    double angle = 0;
    SDL_Point pos1 = position;
    SDL_Point pos2 = position;

    pos1.x += radius;
    for(int i=0; i<segments; i++)
    {
        angle = angle_per_seg*i;
        pos1.x = position.x + radius*SDL_cos(angle);
        pos1.y = position.y + radius*SDL_sin(angle);
        pos2.x = position.x + radius*SDL_cos(angle+angle_per_seg);
        pos2.y = position.y + radius*SDL_sin(angle+angle_per_seg);
        Draw_Line(pos1,pos2);
    }
}

bool GraphicsSYSTEM::Load_Texture_From_File(TextureComponent& textureComponent, std::string path)
{   
    bool success = true;
    if(path == "")
    {
        path = textureComponent.path;
        Logger::log(LogLevel::INFO, "Texture path not explicitly specified, using implicit path");
    }
    else{textureComponent.path = path;}
    SDL_Surface* surface = IMG_Load(path.c_str());
    if(surface == NULL)
    {
        Logger::log(LogLevel::ERROR, "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
        success = false;
    }
    else
    {
        SDL_Texture* raw_texture = SDL_CreateTextureFromSurface( renderer, surface );
        if(raw_texture == NULL)
        {
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

bool GraphicsSYSTEM::Load_Texture_From_String(TextureComponent& textureComponent, const std::string& texture_text, TTF_Font* font, SDL_Color text_color)
{
    SDL_Surface* textSurface = TTF_RenderText_Solid(font,texture_text.c_str(), text_color);
    if( textSurface == NULL )
    {
        Logger::log(LogLevel::ERROR,"Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
    }
    else
    {
        SDL_Texture* raw_texture = SDL_CreateTextureFromSurface( renderer, textSurface );
        if(raw_texture == NULL)
        {
            Logger::log(LogLevel::ERROR,"Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
        }
        else
        {
            textureComponent.texture = std::shared_ptr<SDL_Texture>(raw_texture, TextureComponent::SDLTextureDeleter());
            textureComponent.width = textSurface->w;
            textureComponent.height = textSurface->h;
        }
        SDL_FreeSurface( textSurface );
    }      
    return textureComponent.texture != NULL;
}

bool GraphicsSYSTEM::Init_Textures_From_JSON(const nlohmann::json& json)
{
    bool success = true;
    for(auto& texture: json.items())
    {
        std::string path = texture.value().at("path");
        std::string identifier = texture.key();

        TextureComponent textureComp;
        success = Load_Texture_From_File(textureComp, path);
        if(success)
        {
            Entity textureID = engine.Get_EntitySYSTEM().Create_Entity();
            engine.Get_ComponentSYSTEM().Add_Component<TextureComponent>(textureID, textureComp);
            success = Add_Texture_Identification(identifier, textureID);
        }
    }
    return success;
}

bool GraphicsSYSTEM::Add_Texture_Identification(const std::string& identifier, Entity textureID)
{
    bool success = true;
    if(texture_identifcation.find(identifier) != texture_identifcation.end())
    {
        Logger::log(LogLevel::WARNING, "New idenfication of texture failed, texture has already been identified");
        success = false;
    }
    else
    {
        texture_identifcation[identifier] = textureID;
        Logger::log(LogLevel::INFO, "New idenfication of texture %s with ID %d registered", identifier.c_str(), textureID);
    }
    return success;
}

void GraphicsSYSTEM::Set_Texture_Identification(Entity& targetID, const std::string& identifer)
{
    if(texture_identifcation.find(identifer) == texture_identifcation.end())
    {
        Logger::log(LogLevel::WARNING, "Texture identifier %s doesn't exists, texture identification failed", identifer.c_str());
        return;
    }

    targetID = texture_identifcation.at(identifer);
    Logger::log(LogLevel::INFO, "Texture identifier %s registered to an entity", identifer.c_str());  
}

void GraphicsSYSTEM::Draw_Texture(TransformComponent transformComponent, TextureComponent& textureComponent, double scale, SDL_Rect* clip,
    SDL_Point* center, SDL_RendererFlip flip)
{
	SDL_Rect renderQuad = { transformComponent.x, transformComponent.y, 
        static_cast<int>(std::round(textureComponent.width*scale)), static_cast<int>(std::round(textureComponent.height*scale))};
	if (clip != NULL)
	{
		renderQuad.w = static_cast<int>(std::round(clip->w*scale));
		renderQuad.h = static_cast<int>(std::round(clip->h*scale));
	}
	SDL_RenderCopyEx(renderer, textureComponent.texture.get(), 
		clip, &renderQuad, transformComponent.angle, center, flip);
}

void GraphicsSYSTEM::Draw_Text(Entity text_entity)
{
    auto& compSYSTEM = engine.Get_ComponentSYSTEM();
    Draw_Texture(compSYSTEM.Get_Component<TransformComponent>(text_entity), compSYSTEM.Get_Component<TextComponent>(text_entity).texture);
}

void GraphicsSYSTEM::Draw_RenderComp(Entity entity)
{
    auto& compSYSTEM = engine.Get_ComponentSYSTEM();
    auto& transformComp = compSYSTEM.Get_Component<TransformComponent>(entity);
    auto& renderComp = compSYSTEM.Get_Component<RenderComponent>(entity);
    auto& textureComp = compSYSTEM.Get_Component<TextureComponent>(renderComp.ID);
    SDL_Rect* clip = NULL;
    if(renderComp.is_clip)
    {
        clip = &renderComp.clip;
    }
    Draw_Texture(transformComp, textureComp, renderComp.scale, clip);
}

void GraphicsSYSTEM::Render_Clear()
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer);
}

void GraphicsSYSTEM::Render_Present(){SDL_RenderPresent(renderer);}

void GraphicsSYSTEM::Render()
{
    auto& compSYSTEM = engine.Get_ComponentSYSTEM();
    auto renderComp_array = compSYSTEM.Get_Component_Array<RenderComponent>().get();
    for(auto it = renderComp_array->begin(); it != renderComp_array->end(); ++it)
    {
        Entity ID = it->first;
        Draw_RenderComp(ID);
    }
}

void GraphicsSYSTEM::Update_String_Texture(Entity ID, const std::string& text, TTF_Font* font, SDL_Color text_color)
{
    TextComponent& textComp = engine.Get_ComponentSYSTEM().Get_Component<TextComponent>(ID);
    Load_Texture_From_String(textComp.texture, text, font, text_color);
    textComp.text = text;
}