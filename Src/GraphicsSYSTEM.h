#ifndef GRAPHICSSYSTEM_H
#define GRAPHICSSYSTEM_H
#include <SYSTEM.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <ComponentSYSTEM.h>
#include <Components.h>
#include <vector>
#include <queue>
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
        FontComponent& Get_Font(const std::string& font);

        void Set_Render_Target(const TextureComponent& texture);
        void Set_Render_Target();

        bool Create_Font(FontComponent& font, const std::string& path, const int size);
        bool Construct_Text_Glyphs(FontComponent& font, int font_size);

        bool Create_Texture(TextureComponent& texture_component, Uint32 format,int access, int w,int h);
        bool Create_Texture(TextureComponent& textureComponent, const std::string& path);
        
        void Register_Font(const std::string& identifier, FontComponent& font);
        size_t Register_Texture(const std::string& identifier, TextureComponent& texture);
        size_t Register_Texture_From_File(const std::string& identifier, const std::string& path);
        void Set_RenderInfo_Texture( const std::string& identifier, RenderInfoComponent& renderInfo);

        void Set_Renderer_Color(SDL_Color color);
        void Draw_Rect(const SDL_Rect& rect, SDL_Color color = {255,0,0,255});
        void Draw_Circle(SDL_Point position, unsigned int radius, SDL_Color color={0,0,0,255});
        void Draw_Line(SDL_Point pos1, SDL_Point pos2);
	    void Draw_Texture(const TransformComponent& transformComponent, const TextureComponent& textureComponent, double scale=1.0, 
            const SDL_Rect* clip=NULL,const SDL_Point* center=NULL, const SDL_RendererFlip flip=SDL_FLIP_NONE);

        void Draw_RenderComp(const TransformComponent& transform, const RenderInfoComponent& renderInfo);
        void Draw_Text(const TransformComponent& transform, const std::string& text, const SDL_Color color={0,0,0,255}, const std::string& identifier="default");

        size_t Get_Texture_By_Identifier(const std::string& identifier);

        void Push_Render_Task(const std::string& task_identifier, std::function<void(void)> task);
        void Render_Scene(Scene& scene);

        void Render();
        void Render_Present();
        void Render_Clear();

    private:
        SDL_Renderer* renderer;
		SDL_Window* window;

		SDL_Point window_size;
		SDL_Point resolution;

        DataStructure::MappedArray<std::string, TextureComponent, MAX_TEXTURES> texture_array;
        std::unordered_map<std::string, FontComponent> fonts;
    
        std::vector<std::function<void(void)>> render_tasks;
        std::unordered_map<std::string, size_t> render_tasks_map;
};
#endif // !GRAPHICSSYSTEM_H