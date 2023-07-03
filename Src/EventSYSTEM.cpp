#include <EventSYSTEM.h>
#include <Engine.h>
EventSYSTEM::EventSYSTEM()
{


}
EventSYSTEM::~EventSYSTEM()
{

    
}

void EventSYSTEM::Check_Default_Events()
{
	for(int i=0; i<SDLEvents.currentEventNum; i++)
	{
		if (SDLEvents.events[i].type == SDL_QUIT)
		{	
			Get_Engine()->Shut_Down();
		}
	}   
}

void EventSYSTEM::Update_SDL_Events()
{
    SDL_PumpEvents();
    SDLEvents.currentEventNum = SDL_PeepEvents(SDLEvents.events, 256, SDL_PEEKEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT);
}

void EventSYSTEM::Clear_SDL_Events()
{
    SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
}

Event<SDL_Event> EventSYSTEM::Get_SDL_Events()
{
    return SDLEvents;
}