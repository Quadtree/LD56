#include "Button.h"
#include "../GameState/Vector2.h"
#include "../Util.h"

void EnterMainMenuState();

string messageGlobal;
static function<void()> nextGlobal;

static double lockedUntil = 0;

static void MainLoop()
{
    SDL_Event evt;
    while (SDL_PollEvent(&evt))
    {
        if ((evt.type == SDL_KEYDOWN || evt.type == SDL_MOUSEBUTTONDOWN) && GetTimeAsDouble() >= lockedUntil)
        {
            nextGlobal();
        }
    }

    int extent, count;

    TTF_MeasureUTF8(GetFont(72), messageGlobal.c_str(), 2000, &extent, &count);

    int wndW, wndH;
    SDL_GetWindowSize(wnd, &wndW, &wndH);

    DrawText(messageGlobal, Vector2(wndW / 2 - extent / 2 + 4, 44), 72, {50, 50, 50, 255});
    DrawText(messageGlobal, Vector2(wndW / 2 - extent / 2, 40), 72, {255, 255, 255, 255});

    {
        const auto otherText = "Press any key to proceed";
        const auto otherTextSize = 24;

        TTF_MeasureUTF8(GetFont(otherTextSize), otherText, 2000, &extent, &count);

        DrawText(otherText, Vector2(wndW / 2 - extent / 2, 450), otherTextSize, {255, 255, 255, 255});

        SDL_RenderPresent(rnd);
    }
}

void EnterMessageScreen(string message, function<void()> next)
{
    CallTearDownFunction();

    lockedUntil = GetTimeAsDouble() + 1.5;

    messageGlobal = message;
    nextGlobal = next;

    emscripten_cancel_main_loop();
    emscripten_set_main_loop(MainLoop, 0, 0);
}