#ifndef EVENTSYSTEM_H
#define EVENTSYSTEM_H
#include <SDL2/SDL.h>
#include <SYSTEM.h>

template <typename EventType>
class Event {
    public:
        EventType events[256];
        int currentEventNum;
};

class EventSYSTEM: public SYSTEM
{
    public:
        EventSYSTEM();
        ~EventSYSTEM();
        void Update_SDL_Events();
        void Clear_SDL_Events();
        
        void Check_Default_Events();
        Event<SDL_Event> Get_SDL_Events();
    private:
        Event<SDL_Event> SDLEvents;
};

#endif