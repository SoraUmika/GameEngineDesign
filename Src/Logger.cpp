#include <Logger.h>

namespace Logger
{
    std::ofstream log_file;
    void close()
    {
        Logger::log(LogLevel::INFO, "Logger closed");
        log_file.close();
    }
    void initialize(const std::string& file_path)
    {
        log_file.open(file_path);
        if (!log_file.good()) {
            throw std::runtime_error("Failed to open log file");
        }
    }
}
