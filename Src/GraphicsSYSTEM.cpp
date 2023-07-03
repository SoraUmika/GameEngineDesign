#include <GraphicsSYSTEM.h>
#include <Engine.h>
GraphicsSYSTEM::GraphicsSYSTEM()
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
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
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
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
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
        window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    if (renderer == NULL)
    {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
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
            printf
            (
                "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError()
            );
            success = false;
        }

        if (TTF_Init() == -1)
        {
            printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
            success = false;
        }
        else
        {
            default_font = TTF_OpenFont("Content/Fonts/default_font.ttf", 20);
            if (default_font == NULL)
            {
                printf("Failed to load default font! SDL_ttf Error: %s\n", TTF_GetError());
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
                    printf("There is no '%c' in the loaded font! ASCII: %i\n", 
                        currentChar, currentChar);
            }
        }
    }
    return success;
}

SDL_Renderer* GraphicsSYSTEM::Get_Renderer()
{
    return renderer;
}

SDL_Window* GraphicsSYSTEM::Get_Window()
{
    return window;
}

TTF_Font* GraphicsSYSTEM::Get_DefaultFont()
{
    return default_font;
}

void GraphicsSYSTEM::Set_Renderer_Color(SDL_Color color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

void GraphicsSYSTEM::Draw_Rect(SDL_Rect rect)
{
    SDL_RenderDrawRect(renderer, &rect);
}

void GraphicsSYSTEM::Draw_Circle(SDL_Point position, unsigned int radius)
{

}

void GraphicsSYSTEM::Draw_Line(SDL_Point pos1, SDL_Point pos2)
{
    SDL_RenderDrawLine(renderer, pos1.x, pos1.y, pos2.x, pos2.y);
}

void GraphicsSYSTEM::Draw_Texture(TransformComponent transformComponent, TextureComponent& textureComponent, SDL_Rect* clip,
    SDL_Point* center, SDL_RendererFlip flip)
{
	SDL_Rect renderQuad = { transformComponent.position.x, transformComponent.position.y, textureComponent.width, textureComponent.height };
	if (clip != NULL)
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}
	SDL_RenderCopyEx(renderer, textureComponent.texture.get(), 
		clip, &renderQuad, transformComponent.angle, center, flip);
}

void GraphicsSYSTEM::Draw_Texture(Entity entity)
{
    auto& compSYSTEM = Get_Engine()->Get_System<ComponentSYSTEM>();
    Draw_Texture(compSYSTEM.Get_Component<TransformComponent>(entity), compSYSTEM.Get_Component<TextureComponent>(entity));
}

void GraphicsSYSTEM::Render_Clear()
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer);
}

void GraphicsSYSTEM::Render_Present()
{
    SDL_RenderPresent(renderer);
}

bool GraphicsSYSTEM::Load_Texture_From_File(TextureComponent& textureComponent, std::string path)
{   
    bool success = true;
    SDL_Surface* surface = IMG_Load(path.c_str());
    if(surface == NULL)
    {
        printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
        success = false;
    }
    else
    {
        SDL_Texture* raw_texture = SDL_CreateTextureFromSurface( renderer, surface );
        if(raw_texture == NULL)
        {
            printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
            success = false;
        }
        textureComponent.texture = std::shared_ptr<SDL_Texture>(raw_texture, TextureComponent::SDLTextureDeleter());
        textureComponent.width = surface->w;
        textureComponent.height = surface->h;
        SDL_FreeSurface(surface);
    }
    Logger::log(LogLevel::INFO, "Texture %s loaded", path);
    return success;
}

bool GraphicsSYSTEM::Load_Texture_From_Text(TextureComponent& textureComponent, std::string texture_text, TTF_Font* font, SDL_Color text_color)
{
    SDL_Surface* textSurface = TTF_RenderText_Solid(font,texture_text.c_str(), text_color);
    if( textSurface == NULL )
    {
        printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
    }
    else
    {
        SDL_Texture* raw_texture = SDL_CreateTextureFromSurface( renderer, textSurface );
        if(raw_texture == NULL)
        {
            printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
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