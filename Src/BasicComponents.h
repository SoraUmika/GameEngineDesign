#ifndef BASICCOMPONENT_H
#define BASICCOMPONENT_H
#include <SDL2/SDL.h>
#include <Logger.h>
template <typename T>
struct RectComponent{
    RectComponent(){}
    RectComponent(T x, T y, T w, T h):x(x), y(y), w(w), h(h){};
    RectComponent(const SDL_Rect& rect){
        x = rect.x;
        y = rect.y;
        w = rect.w;
        h = rect.h;
    }
    operator SDL_Rect() const {
        SDL_Rect rect;
        rect.x = static_cast<int>(x);
        rect.y = static_cast<int>(y);
        rect.w = static_cast<int>(w);
        rect.h = static_cast<int>(h);
        return rect;
    }
    T x{};
    T y{};
    T w{};
    T h{};
};

template <typename T>
struct Vect2D{
    Vect2D(){}
    Vect2D(T x, T y): x(x),y(y){}
    Vect2D(const SDL_Point& point){
        x = point.x;
        y = point.y;
    }
    Vect2D operator+(const Vect2D& other) const {
        return Vect2D(x + other.x, y + other.y);
    }
    Vect2D operator-(const Vect2D& other) const {
        return Vect2D(x - other.x, y - other.y);
    }
    Vect2D operator*(T scalar) const {
        return Vect2D(x * scalar, y * scalar);
    }
    Vect2D operator/(T scalar) const {
        if (scalar != 0) {
            return Vect2D(x / scalar, y / scalar);
        } else {
            Logger::log(LogLevel::WARNING, "Dividing by zero encountered");
            return Vect2D();
        }
    }
    T x{};
    T y{};
};

#endif //
