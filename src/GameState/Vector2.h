#pragma once

struct Vector2
{
    Vector2(float x, float y) : X(x), Y(y) {}
    Vector2() : X(0), Y(0) {}

    float X;
    float Y;

    Vector2 operator/(const float &scalar)
    {
        return Vector2(X / scalar, Y / scalar);
    }
};
