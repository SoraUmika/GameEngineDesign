#ifndef EVENTSYSTEM_H
#define EVENTSYSTEM_H
#include <SYSTEM.h>

template <typename EventType>
class Event {
    public:
        EventType events[256];
        int currentEventNum{};
};

class EventSYSTEM: public SYSTEM
{
    public:
        EventSYSTEM(Engine& engine);
        ~EventSYSTEM();
        void Update_SDL_Events();
        void Clear_SDL_Events();
        void Check_Default_Events();
        Event<SDL_Event> Get_SDL_Events();

        void Process_Entities_Input();
        void Update();
    private:
        Event<SDL_Event> SDLEvents;
};

#endif