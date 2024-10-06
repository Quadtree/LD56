#include "Button.h"

Button mainMenuButtons[2];

void MainMenuMainLoop()
{
    SDL_Event evt;
    while (SDL_PollEvent(&evt))
    {
        if (evt.type == SDL_KEYDOWN)
        {
        }
    }
}

void EnterMainMenuState()
{
    mainMenuButtons[0].Index = 0;

}

