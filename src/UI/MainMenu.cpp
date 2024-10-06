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

    for (int i = 0; i < 2; ++i)
    {
        mainMenuButtons[i].Render();
    }

    SDL_RenderPresent(rnd);
}

void EnterMainMenuState()
{
    cout << "EnterMainMenuState()" << endl;
    mainMenuButtons[0].Setup(0, "Start Game", []() {});
    mainMenuButtons[1].Setup(1, "Level Select", []() {});

    emscripten_cancel_main_loop();
    emscripten_set_main_loop(MainMenuMainLoop, 0, 0);
}
