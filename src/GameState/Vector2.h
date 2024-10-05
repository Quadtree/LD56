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

    Vector2 operator*(const float &scalar)
    {
        return Vector2(X * scalar, Y * scalar);
    }

    Vector2 operator-(const Vector2 &v2)
    {
        return Vector2(X - v2.X, Y - v2.Y);
    }

    Vector2 operator+(const Vector2 &v2)
    {
        return Vector2(X + v2.X, Y + v2.Y);
    }

    Vector2 &operator+=(const Vector2 &v2)
    {
        X += v2.X;
        Y += v2.Y;
        return *this;
    }

    Vector2 &operator-=(const Vector2 &v2)
    {
        *this = (*this - v2);
        return *this;
    }

    Vector2 &operator/=(const float &scalar)
    {
        X /= scalar;
        Y /= scalar;

        return *this;
    }

    Vector2 &operator*=(const float &scalar)
    {
        *this = (*this * scalar);
        return *this;
    }

    float DistToSquared(const Vector2 &v2)
    {
        return powf(X - v2.X, 2) + powf(Y - v2.Y, 2);
    }
};
