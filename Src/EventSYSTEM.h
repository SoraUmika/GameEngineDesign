#ifndef EVENTSYSTEM_H
#define EVENTSYSTEM_H
#include <SYSTEM.h>
#include <queue>
#include <unordered_map>
class EventSYSTEM: public SYSTEM
{
    public:
        EventSYSTEM(Engine& engine);
        ~EventSYSTEM();
        EventCode Register_Event_Type(const std::string& type_name);
        EventCode Get_Event_Type(const std::string& type_name);
        void Push_Event(const Event& event);
        void Update();
    private:
        SDL_Event sdl_event;
        std::queue<Event> event_queue;
        std::unordered_map<std::string, EventType> type_map;

        EventType available_event_type = SDL_EventType::SDL_LASTEVENT + 1;
        void Handle_Events();
        void Poll_Events();
        void Poll_SDL_Events();
};

#endif