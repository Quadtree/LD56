#pragma once

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
};