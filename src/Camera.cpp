#include "Camera.h"

Camera::Camera() : ZoomLevel(10)
{
}

Vector2 Camera::ScreenToReal(Vector2 screen)
{
    return Vector2();
}

Vector2 Camera::RealToScreen(Vector2 screen)
{
    return Vector2();
}

float Camera::RealToScreenScale(float inputValue)
{
    return inputValue * ZoomLevel;
}
