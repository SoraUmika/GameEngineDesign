#ifndef ELEMENTARYTYPES_H
#define ELEMENTARYTYPES_H

#include <SDL2/SDL.h>
#include <string>
#include <variant>
using Rect = SDL_Rect;
using Point = SDL_Point;
using Entity = Uint64;
using EventType = uint32_t;

struct KeyboardStateEvent
{
    SDL_Scancode scancode;
};

struct Event{
    EventType type{};
    std::variant<SDL_KeyboardEvent, SDL_MouseMotionEvent, SDL_MouseButtonEvent, KeyboardStateEvent> data;
};

#endif