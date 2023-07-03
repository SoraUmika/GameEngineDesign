#ifndef SYSTEM_H
#define SYSTEM_H
#include <Logger.h>

class Engine;
class SYSTEM
{
    public:
        SYSTEM()
        {
            this->engine = nullptr;
        }
        ~SYSTEM()
        {
            this->engine = nullptr;
        }
        static void Set_Engine_Ptr(Engine *enginePtr)
        {
            engine = enginePtr;
        }

        static Engine* Get_Engine()
        {
            if(engine == nullptr)
            {
                Logger::log(LogLevel::ERROR, "Engine pointer no set!");
            }
            return engine;
        }
    protected:
    private:
        static Engine* engine;
};
#endif // !SYSTEM_H