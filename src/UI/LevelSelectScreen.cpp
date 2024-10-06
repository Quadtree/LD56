#include "Button.h"



static Button mainMenuButtons[NUM_LEVELS];

void MainLoop()
{
    SDL_Event evt;
    while (SDL_PollEvent(&evt))
    {
        if (evt.type == SDL_MOUSEBUTTONDOWN)
        {
            for (int i = 0; i < NUM_LEVELS; ++i)
                mainMenuButtons[i].UserClickedSomewhere();
        }
    }

    for (int i = 0; i < NUM_LEVELS; ++i)
    {
        mainMenuButtons[i].Render();
    }

    SDL_RenderPresent(rnd);
}

void EnterLevelSelectState()
{
    CallTearDownFunction();

    cout << "EnterMainMenuState()" << endl;

    for (auto i = 0; i < NUM_LEVELS; ++i)
    {
        ostringstream levelAssetName;
        levelAssetName << "assets/level" << (i + 1) << ".xcf";

        ostringstream buttonText;
        buttonText << "Level " << (i + 1);

        auto levelAssetNameStr = levelAssetName.str();
        auto localI = i;

        mainMenuButtons[i].Setup(i, buttonText.str(), [localI, levelAssetNameStr]()
                                 {
                                     cout << "LEVEL SELECT TO " << (localI+1) << " " << levelAssetNameStr <<  endl;
                                     EnterInGameState(localI+1); });
    }

    emscripten_cancel_main_loop();
    emscripten_set_main_loop(MainLoop, 0, 0);
}
