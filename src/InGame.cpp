#include "LD56.h"
#include "Util.h"
#include "GameState/GameState.h"
#include "Camera.h"

thread gameUpdateThread;
Uint64 ticksHandledByGameStateUpdates;

extern SDL_Renderer *rnd;
extern SDL_Window *wnd;

shared_ptr<SDL_Texture> testImage;

GameState *gameStates;
int currentGameState = 0;
int nextGameStateToUpdate = 1;

bool showingDebugInfo = false;
int currentFPS = 0;

Camera camera;

Vector2 attractionPoint;
BacteriaType attractionType = BacteriaType::Invalid;

void UpdateWorldState()
{
    lock_guard gameStateLock(gameStates[nextGameStateToUpdate].mutex);

    gameStates[currentGameState].AttractionPoints[0].Type = attractionType;
    gameStates[currentGameState].AttractionPoints[0].Location = attractionPoint;

    gameStates[currentGameState].DoUpdate(gameStates[nextGameStateToUpdate]);

    nextGameStateToUpdate = currentGameState;
    currentGameState = 1 - currentGameState;
}

void GameUpdateThread()
{
    auto ticksPerGameUpdate = SDL_GetPerformanceFrequency() / 60;

    Uint64 lastSecond = 0;
    auto updatesLastSecond = 0;

    ticksHandledByGameStateUpdates = SDL_GetPerformanceCounter();

    while (true)
    {
        if (SDL_GetPerformanceCounter() > ticksHandledByGameStateUpdates)
        {
            ticksHandledByGameStateUpdates += ticksPerGameUpdate;

            auto startTime = GetTimeAsDouble();
            UpdateWorldState();
            auto endTime = GetTimeAsDouble();
            // cout << "World update time: " << (int)((endTime - startTime) * 1000) << "ms" << endl;

            updatesLastSecond++;
        }
        else
        {
            SDL_Delay(0);
        }

        auto currentSecond = SDL_GetTicks64() / 1000;
        if (currentSecond != lastSecond)
        {
            // cout << "Game Update FPS=" << updatesLastSecond << endl;
            currentFPS = updatesLastSecond;
            updatesLastSecond = 0;
            lastSecond = currentSecond;
        }
    }
}

void InGameMainLoop()
{
    SDL_Event evt;
    while (SDL_PollEvent(&evt))
    {
        if (evt.type == SDL_KEYDOWN)
        {
            if (evt.key.keysym.sym == SDLK_F3)
            {
                cout << "F3 pressed" << endl;
                showingDebugInfo = !showingDebugInfo;
            }

            if (evt.key.keysym.sym == SDLK_1)
                attractionType = BacteriaType::Converter;
            if (evt.key.keysym.sym == SDLK_2)
                attractionType = BacteriaType::Swarmer;
        }

        if (evt.type == SDL_KEYUP)
        {
            if (evt.key.keysym.sym == SDLK_1 || evt.key.keysym.sym == SDLK_2)
                attractionType = BacteriaType::Invalid;
        }
    }

    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    attractionPoint = camera.ScreenToReal(Vector2(mouseX, mouseY));

    SDL_SetRenderDrawColor(rnd, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(rnd);

    {
        auto gameStateBeingRendered = currentGameState;

        lock_guard rendererLockGuard(gameStates[gameStateBeingRendered].mutex);

#if _DEBUG
        gameStates[gameStateBeingRendered].BeingRendered = true;
#endif

        int windowW, windowH;
        SDL_GetWindowSize(wnd, &windowW, &windowH);

        camera.ScreenPixelSize = Vector2(windowW, windowH);

        for (auto i = 0; i < gameStates[gameStateBeingRendered].NumActiveBacteria; ++i)
        {
#if _DEBUG
            if (gameStates[gameStateBeingRendered].BeingUpdated)
            {
                throw "Game state is also being updated!!!";
            }
#endif
            gameStates[gameStateBeingRendered].BacteriaList[i].Render(rnd, camera);
        }

#if _DEBUG
        gameStates[gameStateBeingRendered].BeingRendered = false;
#endif
    }

    if (showingDebugInfo)
    {
        ostringstream oss;
        oss << "FPS: " << currentFPS;
        DrawText(oss.str(), Vector2(20, 20));
    }

    SDL_RenderPresent(rnd);
}

void EnterInGameState()
{
    if (gameStates)
        delete gameStates;
    gameStates = new GameState[2];

    currentGameState = 0;
    nextGameStateToUpdate = 1;

    showingDebugInfo = false;
    currentFPS = 0;

    camera = Camera();

    attractionPoint = Vector2();
    attractionType = BacteriaType::Invalid;

    for (int i = 0; i < 50; ++i)
    {
        Bacteria b1;
        b1.Position = Vector2(i, 0);
        b1.Type = BacteriaType::Swarmer;
        b1.Velocity = Vector2(0, 0);

        gameStates[0].AddBacteria(b1);

        Bacteria b2;
        b2.Position = Vector2(i + 0.5f, 4);
        b2.Type = BacteriaType::Converter;
        b2.Velocity = Vector2(0, 0);

        gameStates[0].AddBacteria(b2);

        Bacteria b3;
        b3.Position = Vector2(i + 0.5f, -10);
        b3.Type = BacteriaType::Swarmer;
        b3.Velocity = Vector2(0, 0);
        b3.Faction = 1;

        gameStates[0].AddBacteria(b3);
    }

    emscripten_cancel_main_loop();

    cout << "About to call emscripten_set_main_loop" << endl;
    emscripten_set_main_loop(InGameMainLoop, 0, 0);

    gameUpdateThread = thread(GameUpdateThread);

    testImage = LoadTexture("assets/xt3.xcf");
}