#ifndef LOGGERTEST_H
#define LOGGERTEST_H
#include <iostream>
#include <sstream>
#include <memory>
#include <fstream>

enum class LogLevel
{
    INFO,
    WARNING,
    ERROR,
};

namespace Logger
{

    extern std::ofstream log_file;
    template<typename... Args>
    std::string formatMessage(const std::string& format, Args... args)
    {
        size_t size = snprintf( nullptr, 0, format.c_str(), args... ) + 1; // Extra space for '\0'
        std::unique_ptr<char[]> buf( new char[ size ] ); 
        snprintf( buf.get(), size, format.c_str(), args... );
        return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
    }

    template<typename... Args>
    void log(LogLevel level, const std::string& format, Args... args)
    {
        #ifdef DEBUG
            std::stringstream ss;
            
            switch (level)
            {
            case LogLevel::INFO:
                ss << "[INFO]: " << formatMessage(format, args...) << std::endl;
                break;
            case LogLevel::WARNING:
                ss << "[WARNING]: " << formatMessage(format, args...) << std::endl;
                break;
            case LogLevel::ERROR:
                ss << "[ERROR]: " << formatMessage(format, args...) << std::endl;
                break;
            default:
                ss << "[UNKNOWN]: " << formatMessage(format, args...) << std::endl;
            }
            log_file << ss.str();
            std::cout << ss.str();
        #endif
    }
    void initialize(const std::string& file_path);
    void close();
}

#endif //  LOGGER_H
