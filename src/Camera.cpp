#include "Camera.h"

Camera::Camera() : ZoomLevel(10)
{
}

Vector2 Camera::ScreenToReal(Vector2 screen)
{
    screen -= ScreenPixelSize / 2;
    screen /= ZoomLevel;
    return screen;
}

Vector2 Camera::RealToScreen(Vector2 real)
{
    real *= ZoomLevel;
    real += ScreenPixelSize / 2;
    return real;
}

float Camera::RealToScreenScale(float inputValue)
{
    return inputValue * ZoomLevel;
}
