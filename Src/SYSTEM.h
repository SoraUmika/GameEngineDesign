#ifndef SYSTEM_H
#define SYSTEM_H
#include <Logger.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <Events.h>

using Entity = uint64_t;
using EventCode = uint32_t;
constexpr Entity MAX_ENTITIES = 65536;
using EntityType = std::string;

class Engine;
class SYSTEM
{
    public:
        SYSTEM(Engine& engine):engine(engine)
        {}

    protected:
        Engine& engine;
};
#endif // !SYSTEM_H