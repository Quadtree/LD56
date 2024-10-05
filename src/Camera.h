#pragma once

#include "GameState/Vector2.h"

class Camera
{
public:
    Camera();

    Vector2 CenterPos;
    float ZoomLevel;

    Vector2 ScreenPixelSize;

    Vector2 ScreenToReal(Vector2 screen);
    Vector2 RealToScreen(Vector2 screen);
    float RealToScreenScale(float inputValue);
};
