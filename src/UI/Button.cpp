#include "Button.h"
#include "../LD56.h"

extern SDL_Renderer *rnd;
extern SDL_Window *wnd;

void Button::Setup(int index, string text, function<void()> onClick)
{
    Index = index;
    Text = text;
    OnClick = onClick;

    IdleTexture = CreateTexture({200, 100, 100, 255}, {150, 100, 100, 255});
}

shared_ptr<SDL_Texture> Button::CreateTexture(SDL_Color edgeColor, SDL_Color backgroundColor)
{
    auto surf = SDL_CreateRGBSurface(0, 400, 150, 32, 0, 0, 0, 0);

    SDL_FillRect(surf, nullptr, SDL_MapRGB(surf->format, edgeColor.r, edgeColor.g, edgeColor.b));

    SDL_Rect innerRect;
    innerRect.x = 10;
    innerRect.y = 10;
    innerRect.w = 400 - 20;
    innerRect.h = 150 - 20;

    SDL_FillRect(surf, &innerRect, SDL_MapRGB(surf->format, backgroundColor.r, backgroundColor.g, backgroundColor.b));

    SDL_Color color;
    color.r = 255;
    color.g = 255;
    color.b = 255;
    color.a = 255;

    auto txtSurf = TTF_RenderUTF8_Blended(GetFont(24), Text.c_str(), color);

    SDL_BlitSurface(txtSurf, nullptr, surf, nullptr);

    auto ret = shared_ptr<SDL_Texture>(SDL_CreateTextureFromSurface(rnd, surf), SDL_DestroyTexture);

    SDL_FreeSurface(surf);
    SDL_FreeSurface(txtSurf);

    return ret;
}

void Button::Render()
{
    auto r = GetRect();

    SDL_RenderCopy(rnd, IdleTexture.get(), nullptr, &r);
}

SDL_Rect Button::GetRect()
{
    int wndW, wndH;
    SDL_GetWindowSize(wnd, &wndW, &wndH);

    SDL_Rect r;
    r.x = (wndW - 400) / 2;
    r.y = wndH - 500 + (Index * 200);
    r.w = 300;
    r.h = 150;

    return r;
}
