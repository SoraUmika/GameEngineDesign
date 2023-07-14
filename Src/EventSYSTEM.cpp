#include <EventSYSTEM.h>
#include <Engine.h>
EventSYSTEM::EventSYSTEM(Engine& engine): SYSTEM(engine)
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
			engine.Shut_Down();
		}
	}
}

void EventSYSTEM::Process_Entities_Input()
{
    auto& compSYSTEM = engine.Get_ComponentSYSTEM();
    auto intputComp_array = compSYSTEM.Get_Component_Array<InputComponent>().get();
    for(auto it = intputComp_array->begin(); it != intputComp_array->end(); ++it)
    {
        Entity ID = it->first;
        auto& inputComp = compSYSTEM.Get_Component<InputComponent>(ID);
		inputComp.action(ID);
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

void EventSYSTEM::Update()
{
	Update_SDL_Events();
	Check_Default_Events();
	Process_Entities_Input();
	Clear_SDL_Events();
}