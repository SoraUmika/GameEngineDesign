#ifndef LOGGER_H
#define LOGGER_H
#include <iostream>
#include <sstream>
#include <string>
#include <memory>

enum class LogLevel
{
    INFO,
    WARNING,
    ERROR,
};

class Logger
{
public:
    template<typename... Args>
    static void log(LogLevel level, const std::string& format, Args... args)
    {
    #ifdef DEBUG

        std::stringstream ss;
        ((ss << args << ' '), ...);  // (1)
        std::string message = ss.str();
        message.pop_back();  // remove trailing space
        
        switch (level)
        {
        case LogLevel::INFO:
            std::cout << "[INFO]: " << formatMessage(format, args...) << std::endl;
            break;
        case LogLevel::WARNING:
            std::cout << "[WARNING]: " << formatMessage(format, args...) << std::endl;
            break;
        case LogLevel::ERROR:
            std::cout << "[ERROR]: " << formatMessage(format, args...) << std::endl;
            break;
        default:
            std::cout << "[UNKNOWN]: " << formatMessage(format, args...) << std::endl;
        }
    #endif
    }

private:
    template<typename... Args>
    static std::string formatMessage(const std::string& format, Args... args)
    {
        size_t size = snprintf( nullptr, 0, format.c_str(), args... ) + 1; // Extra space for '\0'
        std::unique_ptr<char[]> buf( new char[ size ] ); 
        snprintf( buf.get(), size, format.c_str(), args... );
        return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
    }
};
#endif //  LOGGER_H
