#ifndef FILESYSTEM_H
#define FILESYSTEM_H
#include <SYSTEM.h>
#include <JSON/json.hpp>
#include <cmath>

using JSON = nlohmann::json;
class FileSYSTEM: public SYSTEM
{
    public:
        FileSYSTEM(Engine& engine);
        JSON Read_Json_From_Files(std::string path);
        int roundToMultiple(int N, int M=16)
        {
            return std::round(N / M) * M;
        }
    private:

};
#endif // !FILESYSTEM_H