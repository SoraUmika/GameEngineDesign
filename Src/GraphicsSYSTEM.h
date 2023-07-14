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
        bool Init_Window(const char* program_title, int window_Size_x, int window_Size_y);
        bool Init_Renderer(int logical_Res_x, int logical_Res_y);

        SDL_Renderer* Get_Renderer();
        SDL_Window* Get_Window();
        TTF_Font* Get_DefaultFont();

        bool Load_Texture_From_File(TextureComponent& textureComponent, std::string path="");
        bool Load_Texture_From_String(TextureComponent& textureComponent, const std::string& texture_text, TTF_Font* font, SDL_Color text_color={0,0,0,255});
        bool Init_Textures_From_JSON(const nlohmann::json& json);
        bool Add_Texture_Identification(const std::string& identifier, Entity textureID);

        void Set_Texture_Identification(Entity& targetID, const std::string& identifer);

        void Update_String_Texture(Entity ID, const std::string& text, TTF_Font* font, SDL_Color text_color={0,0,0,255});
        
        void Set_Renderer_Color(SDL_Color color);
        void Render_Clear();
        void Draw_Rect(SDL_Rect rect);
        void Draw_Circle(SDL_Point position, unsigned int radius, SDL_Color color={0,0,0,255});
        void Draw_Line(SDL_Point pos1, SDL_Point pos2);
	    void Draw_Texture(TransformComponent transformComponent, TextureComponent& textureComponent, double scale=1.0, SDL_Rect* clip=NULL,
		    SDL_Point* center=NULL, SDL_RendererFlip flip=SDL_FLIP_NONE);
        void Draw_Text(Entity text_entity);
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
};
#endif // !GRAPHICSSYSTEM_H