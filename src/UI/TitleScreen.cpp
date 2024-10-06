#include "Button.h"
#include "../GameState/Vector2.h"
#include "../Util.h"

void EnterMainMenuState();

void TitleScreenMainLoop()
{
    SDL_Event evt;
    while (SDL_PollEvent(&evt))
    {
        if (evt.type == SDL_KEYDOWN || evt.type == SDL_MOUSEBUTTONDOWN)
        {
            EnterMainMenuState();
        }
    }

    int extent, count;

    TTF_MeasureUTF8(GetFont(100), "Bacterium Imperium", 2000, &extent, &count);

    int wndW, wndH;
    SDL_GetWindowSize(wnd, &wndW, &wndH);

    DrawText("Bacterium Imperium", Vector2(wndW / 2 - extent / 2 + 4, 44), 100, {50, 50, 50, 255});
    DrawText("Bacterium Imperium", Vector2(wndW / 2 - extent / 2, 40), 100, {255, 255, 255, 255});

    // draw other text
    {
        const auto otherText = "Made by Quadtree for Ludum Dare 56";
        const auto otherTextSize = 24;

        TTF_MeasureUTF8(GetFont(otherTextSize), otherText, 2000, &extent, &count);

        DrawText(otherText, Vector2(wndW - extent - 10, wndH - 35), otherTextSize, {255, 255, 255, 255});

        SDL_RenderPresent(rnd);
    }

    {
        const auto otherText = "Press any key to proceed";
        const auto otherTextSize = 24;

        TTF_MeasureUTF8(GetFont(otherTextSize), otherText, 2000, &extent, &count);

        DrawText(otherText, Vector2(wndW / 2 - extent / 2, 450), otherTextSize, {255, 255, 255, 255});

        SDL_RenderPresent(rnd);
    }
}

void EnterTitleScreenState()
{
    CallTearDownFunction();
    // title

    emscripten_cancel_main_loop();
    emscripten_set_main_loop(TitleScreenMainLoop, 0, 0);
}