#include <EventSYSTEM.h>
#include <Engine.h>
EventSYSTEM::EventSYSTEM(Engine& engine): SYSTEM(engine)
{
	Register_Event_Type("KEYSTATE");
}
EventSYSTEM::~EventSYSTEM()
{
}


EventType EventSYSTEM::Register_Event_Type(const std::string& type_name)
{

	if(type_map.find(type_name) == type_map.end())
	{
		EventType new_event_type = available_event_type;
		type_map[type_name] = new_event_type;
		available_event_type += 1;
		Logger::log(LogLevel::INFO, "New event type %s registered with type code %d", type_name.c_str() ,new_event_type);
		return new_event_type;
	}else{
		Logger::log(LogLevel::WARNING, "Event type %s registeration failed, already registered with type code %d", type_name.c_str(), type_map.at(type_name));
		return 0;
	}
}

EventCode EventSYSTEM::Get_Event_Type(const std::string& type_name)
{
	if(type_map.find(type_name) != type_map.end())
	{
		return type_map.at(type_name);
	}else{
		Logger::log(LogLevel::ERROR, "Cannot find event type %s with a valid type code", type_name.c_str());
		return 0;
	}
}

void EventSYSTEM::Push_Event(const Event& event)
{
	event_queue.push(event);
}

void EventSYSTEM::Poll_SDL_Events()
{
	const Uint8* key_state = SDL_GetKeyboardState(NULL);
	for(int i=0; i<SDL_Scancode::SDL_NUM_SCANCODES; i++)
	{
		if(key_state[i] == 1)
		{
			Event new_event;
			new_event.type = type_map.at("KEYSTATE");
			KeyboardStateEvent keyboard_state = KeyboardStateEvent();
			keyboard_state.scancode = static_cast<SDL_Scancode>(i);
			new_event.data = keyboard_state;
			event_queue.push(new_event);
		}
	}
	if(SDL_PollEvent(&sdl_event)){
		Event event;
		event.type = sdl_event.type;
		switch(event.type)
		{
			case SDL_QUIT:
				event_queue.push(event);
				break;
			case SDL_KEYDOWN:
			case SDL_KEYUP:
				event.data = sdl_event.key;
				event_queue.push(event);
				break;
		}
	}
}

void EventSYSTEM::Poll_Events()
{

}

void EventSYSTEM::Handle_Events()
{
	while(!event_queue.empty())
	{
		Event event = event_queue.front();	
		if(event.type == SDL_QUIT){
			std::queue<Event> empty_queue;
			std::swap(event_queue, empty_queue);
			engine.Shut_Down();
			break;
		}
		
		auto& eventListener_array = engine.Get_ComponentSYSTEM().Get_Component_Array<EventListenerComponent>();
		auto& entities_list = eventListener_array.Get_Entities();
		for(int i=0; i<entities_list.size();i++)
		{
			Entity entity = entities_list[i];
			auto& listener = eventListener_array.Get_Data(entity);
			listener.execute(event);
		}		
		event_queue.pop();
	}
}

void EventSYSTEM::Update()
{
	Poll_Events();
	Poll_SDL_Events();
	Handle_Events();
}
