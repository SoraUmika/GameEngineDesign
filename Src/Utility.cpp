#include <Utility.h>

SDL_Rect Get_Rect_From_JSON_OBJ(const nlohmann::json& json)
{
    SDL_Rect rect;
    rect.x = Get_JSON_Key<int>(json, "x");
    rect.y = Get_JSON_Key<int>(json, "y");
    rect.w = Get_JSON_Key<int>(json, "w");
    rect.h = Get_JSON_Key<int>(json, "h");
    return rect;
}