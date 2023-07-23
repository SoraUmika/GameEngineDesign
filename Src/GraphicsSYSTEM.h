#ifndef GRAPHICSSYSTEM_H
#define GRAPHICSSYSTEM_H
#include <SYSTEM.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <ComponentSYSTEM.h>
#include <Components.h>
#include <Utility.h>
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

        bool Load_Texture_From_File(TextureComponent& textureComponent);
        bool Load_Texture_From_String(TextureComponent& textureComponent, const std::string& texture_text, const std::string& font, SDL_Color text_color={0,0,0,255});
        
        void Update_Texture_From_Text(Entity ID, const std::string& text, const std::string& font, SDL_Color text_color={0,0,0,255});
        
        void Set_Renderer_Color(SDL_Color color);
        void Render_Clear();
        void Draw_Rect(SDL_Rect rect, SDL_Color color = {255,0,0,255});
        void Draw_Circle(SDL_Point position, unsigned int radius, SDL_Color color={0,0,0,255});
        void Draw_Line(SDL_Point pos1, SDL_Point pos2);
	    void Draw_Texture(TransformComponent transformComponent, TextureComponent& textureComponent, double scale=1.0, SDL_Rect* clip=NULL,
		    SDL_Point* center=NULL, SDL_RendererFlip flip=SDL_FLIP_NONE);
        void Draw_RenderComp(Entity entity);

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
        std::unordered_map<std::string, Entity> texture_identifcation;

        std::unordered_map<std::string, TTF_Font*> fonts;
};
#endif // !GRAPHICSSYSTEM_H