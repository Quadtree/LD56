#include "Button.h"
#include "../LD56.h"

extern SDL_Renderer *rnd;
extern SDL_Window *wnd;

#define BUTTON_HEIGHT 120

void Button::Setup(int index, string text, function<void()> onClick)
{
    Index = index;
    Text = text;
    OnClick = onClick;

    IdleTexture = CreateTexture({200, 100, 100, 255}, {150, 100, 100, 255});
}

shared_ptr<SDL_Texture> Button::CreateTexture(SDL_Color edgeColor, SDL_Color backgroundColor)
{
    auto surf = SDL_CreateRGBSurface(0, 400, BUTTON_HEIGHT, 32, 0, 0, 0, 0);

    SDL_FillRect(surf, nullptr, SDL_MapRGB(surf->format, edgeColor.r, edgeColor.g, edgeColor.b));

    SDL_Rect innerRect;
    innerRect.x = 10;
    innerRect.y = 10;
    innerRect.w = 400 - 20;
    innerRect.h = BUTTON_HEIGHT - 20;

    SDL_FillRect(surf, &innerRect, SDL_MapRGB(surf->format, backgroundColor.r, backgroundColor.g, backgroundColor.b));

    SDL_Color color;
    color.r = 255;
    color.g = 255;
    color.b = 255;
    color.a = 255;

    auto txtSurf = TTF_RenderUTF8_Blended(GetFont(36), Text.c_str(), color);

    int extent, count;

    TTF_MeasureUTF8(GetFont(36), Text.c_str(), 380, &extent, &count);

    SDL_Rect txtRect;
    txtRect.x = 200 - (extent / 2);
    txtRect.y = 40;
    txtRect.w = txtSurf->w;
    txtRect.h = txtSurf->h;

    SDL_BlitSurface(txtSurf, nullptr, surf, &txtRect);

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
    r.y = wndH - 500 + (Index * (BUTTON_HEIGHT + 40));
    r.w = 300;
    r.h = BUTTON_HEIGHT;

    return r;
}
