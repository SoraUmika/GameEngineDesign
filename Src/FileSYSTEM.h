#ifndef FILESYSTEM_H
#define FILESYSTEM_H
#include <SYSTEM.h>
#include <JSON/json.hpp>

using JSON = nlohmann::json;
class FileSYSTEM: public SYSTEM
{
    public:
        FileSYSTEM(Engine& engine);
        JSON Read_Json_From_Files(std::string path);
    private:

};
#endif // !FILESYSTEM_H