#include <FileSYSTEM.h>

FileSYSTEM::FileSYSTEM(Engine& engine): SYSTEM(engine)
{}

JSON FileSYSTEM::Read_Json_From_Files(std::string path)
{
    SDL_RWops* file = SDL_RWFromFile(path.c_str(), "r");
    if (file == NULL)
    {
        Logger::log(LogLevel::ERROR, "Json file %s cannot be read! %s", path, SDL_GetError()); 
    }
    Sint64 fileSize = SDL_RWsize(file);

    if(fileSize == -1)
    {
        Logger::log(LogLevel::ERROR, "Json file %s size cannot be read! %s", path, SDL_GetError());
    }

    char* buffer = new char[fileSize + 1];
    Sint64 bytesRead = SDL_RWread(file, buffer, 1, fileSize);
    buffer[fileSize] = '\0';  // Add null terminator at the end

    // Create a string from the buffer
    std::string jsonString(buffer);

    // Parse the JSON string using nlohmann/json
    JSON jsonData = JSON::parse(jsonString);

    // Clean up resources
    delete[] buffer;
    SDL_RWclose(file);
    return jsonData;
}