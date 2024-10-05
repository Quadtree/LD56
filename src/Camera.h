#pragma once

#include "GameState/Vector2.h"

class Camera
{
public:
    Camera();

    Vector2 CenterPos;
    float ZoomLevel;
};
