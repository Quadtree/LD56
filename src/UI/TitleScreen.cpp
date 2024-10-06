#include "Button.h"

void TitleScreenMainLoop()
{
    SDL_Event evt;
    while (SDL_PollEvent(&evt))
    {
        if (evt.type == SDL_KEYDOWN)
        {
        }
    }
}

void EnterTitleScreenState()
{
    CallTearDownFunction();
    // title
}