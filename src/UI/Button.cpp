#include "Button.h"

extern TTF_Font *defaultFont;

void Button::Setup(int index, string text, function<void()> onClick)
{
    Index = index;
    Text = text;
    OnClick = onClick;
}

shared_ptr<SDL_Texture> Button::CreateTexture(SDL_Color edgeColor, SDL_Color backgroundColor)
{
    auto surf = SDL_CreateRGBSurface(0, 400, 150, 32, 0, 0, 0, 0);

    SDL_FillRect(surf, nullptr, SDL_MapRGB(surf->format, edgeColor.r, edgeColor.g, edgeColor.b));

    SDL_Color color;
    color.r = 255;
    color.g = 255;
    color.b = 255;
    color.a = 255;

    auto txtSurf = TTF_RenderUTF8_Blended(GetFont(24), text.c_str(), color);

    SDL_BlitSurface(txtSurf, nullptr, surf, nullptr);

    auto ret = shared_ptr<SDL_Texture>(SDL_CreateTextureFromSurface(rnd, surf), SDL_DestroyTexture);

    SDL_FreeSurface(surf);
    SDL_FreeSurface(txtSurf);

    return ret;
}
