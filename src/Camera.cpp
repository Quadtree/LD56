#include "LD56.h"
#include "Camera.h"

Camera::Camera() : ZoomLevel(10), ZoomStep(2)
{
    Reset();
}

void Camera::SetZoomLevelFromZoomStep()
{
    ZoomLevel = powf(1.4f, ZoomStep);
}

Vector2 Camera::ScreenToReal(Vector2 screen)
{
    screen -= ScreenPixelSize / 2;
    screen /= ZoomLevel;
    screen += CenterPos;
    return screen;
}

Vector2 Camera::RealToScreen(Vector2 real)
{
    real -= CenterPos;
    real *= ZoomLevel;
    real += ScreenPixelSize / 2;
    return real;
}

float Camera::RealToScreenScale(float inputValue)
{
    return inputValue * ZoomLevel;
}

void Camera::Reset()
{
    ZoomStep = 2;
    CenterPos = Vector2(0, 0);
    SetZoomLevelFromZoomStep();
}
