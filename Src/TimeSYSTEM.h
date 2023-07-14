#ifndef TIMESYSTEM_H
#define TIMESYSTEM_H
#include <SYSTEM.h>
#include <SDL2/SDL.h>
#include <Components.h>
class TimeSYSTEM: public SYSTEM
{
    public:
        TimeSYSTEM(Engine& engine): SYSTEM(engine){}
        bool Get_Reapeter_Signal(TimeRepeaterComponent& repeaterComp)
        {
            if (SDL_GetTicks64() - repeaterComp.previous_time >= repeaterComp.interval)
            {
                repeaterComp.previous_time = SDL_GetTicks();
                return true;
            }
            else
            {
                return false;
            }           
        }
    private:
};
#endif // !TIMESYSTEM_H

