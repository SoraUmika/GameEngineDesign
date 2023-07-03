#ifndef GRAPHICSSYSTEM_H
#define GRAPHICSSYSTEM_H
#include <SYSTEM.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <ComponentSYSTEM.h>
#include <Components.h>
#include <stdio.h>

class GraphicsSYSTEM: public SYSTEM
{
    public:
        GraphicsSYSTEM();
        ~GraphicsSYSTEM();
        bool Init_Window(const char* program_title, int window_Size_x, int window_Size_y);
        bool Init_Renderer(int logical_Res_x, int logical_Res_y);

        SDL_Renderer* Get_Renderer();
        SDL_Window* Get_Window();
        TTF_Font* Get_DefaultFont();

        bool Load_Texture_From_File(TextureComponent& textureComponent, std::string path);
        bool Load_Texture_From_Text(TextureComponent& textureComponent, std::string texture_text, TTF_Font* font, SDL_Color text_color={0,0,0,255});

        void Set_Renderer_Color(SDL_Color color);
        void Render_Clear();
        void Draw_Rect(SDL_Rect rect);
        void Draw_Circle(SDL_Point position, unsigned int radius);
        void Draw_Line(SDL_Point pos1, SDL_Point pos2);
	    void Draw_Texture(TransformComponent transformComponent, TextureComponent& textureComponent, SDL_Rect* clip=NULL,
		    SDL_Point* center=NULL, SDL_RendererFlip flip=SDL_FLIP_NONE);
        void Draw_Texture(Entity entity);
        void Draw_Using_RenderComp(RenderComponent& renderComp);
        void Render_Present();  

    private:
        SDL_Renderer* renderer;
		SDL_Window* window;
        TTF_Font* default_font;

		SDL_Point window_size;
		SDL_Point resolution;
};
#endif // !GRAPHICSSYSTEM_H