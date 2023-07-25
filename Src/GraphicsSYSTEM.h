#ifndef GRAPHICSSYSTEM_H
#define GRAPHICSSYSTEM_H
#include <SYSTEM.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <ComponentSYSTEM.h>
#include <Components.h>
#include <vector>
#include <Utility.h>
class TextureContainer
{
    public:

    private:
        
};
class GraphicsSYSTEM: public SYSTEM
{
    public:
        GraphicsSYSTEM(Engine& engine);
        ~GraphicsSYSTEM();
        bool Init_Window(const std::string& program_title, int window_Size_x, int window_Size_y);
        bool Init_Renderer(int logical_Res_x, int logical_Res_y, bool use_vsync=true);

        SDL_Renderer* Get_Renderer();
        SDL_Window* Get_Window();
        TTF_Font* Get_Font(const std::string& font);

        bool Load_Texture_From_File(TextureComponent& textureComponent, const std::string& path);
        bool Load_Texture_From_String(TextureComponent& textureComponent, const std::string& texture_text, const std::string& font, SDL_Color text_color={0,0,0,255});
        
        void Register_Texture_From_File(const std::string& identifier, const std::string& path);
        void Set_RenderInfo_Texture( const std::string& identifier, RenderInfoComponent& renderInfo);

        void Update_Texture_From_Text(Entity ID, const std::string& text, const std::string& font, SDL_Color text_color={0,0,0,255});
        
        void Set_Renderer_Color(SDL_Color color);
        void Render_Clear();
        void Draw_Rect(SDL_Rect& rect, SDL_Color color = {255,0,0,255});
        void Draw_Circle(SDL_Point position, unsigned int radius, SDL_Color color={0,0,0,255});
        void Draw_Line(SDL_Point pos1, SDL_Point pos2);
	    void Draw_Texture(const TransformComponent& transformComponent, const TextureComponent& textureComponent, double scale=1.0, 
            const SDL_Rect* clip=NULL,const SDL_Point* center=NULL, const SDL_RendererFlip flip=SDL_FLIP_NONE);
        void Draw_RenderComp(const TransformComponent& transform, const RenderInfoComponent& renderInfo);

        void Set_Camera_Target(Entity target);
        void Render();
        void Render_Present();  

    private:
        SDL_Renderer* renderer;
		SDL_Window* window;
        TTF_Font* default_font;

		SDL_Point window_size;
		SDL_Point resolution;

        Entity camera;

        std::array<TextureComponent, MAX_Textures> textures;
        size_t texture_index = 1;
        std::unordered_map<std::string, size_t> texture_identifiers;

        std::unordered_map<std::string, TTF_Font*> fonts;
};
#endif // !GRAPHICSSYSTEM_H