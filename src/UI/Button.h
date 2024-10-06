#pragma once
#include "../LD56.h"

class Button
{
public:
    int Index;
    string Text;

    function<void()> OnClick;

    void Setup(int index, string text, function<void()> onClick);

    shared_ptr<SDL_Texture> IdleTexture;
    shared_ptr<SDL_Texture> HoverTexture;
    shared_ptr<SDL_Texture> ClickedTexture;

    shared_ptr<SDL_Texture> CreateTexture(SDL_Color edgeColor, SDL_Color backgroundColor);
};