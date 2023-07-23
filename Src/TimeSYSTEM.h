#ifndef TIMESYSTEM_H
#define TIMESYSTEM_H
#include <SYSTEM.h>
#include <SDL2/SDL.h>
#include <Components.h>
class TimeSYSTEM: public SYSTEM
{
    public:
        TimeSYSTEM(Engine& engine): SYSTEM(engine){}
        void Update_Time_Tracker(TimedStateMachineComponent& state_machine){   
            Uint64 current_time = SDL_GetTicks64();
            state_machine.elapsed_time += current_time - state_machine.previous_time;
            state_machine.previous_time = current_time;
            if(state_machine.elapsed_time >= state_machine.state_duration)
            {
                state_machine.current_state = (state_machine.current_state + 1) % state_machine.num_state;
                state_machine.elapsed_time = state_machine.elapsed_time % state_machine.state_duration;
                state_machine.trigger = true;
                return;
            }
            state_machine.trigger = false;
        }
    private:
};
#endif // !TIMESYSTEM_H

