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

double lastGameUpdateTime = 0;

double elapsedTime;

enum class TerrainType : uint8_t
{
    Invalid,
    Clear,
    Rough,
    Obstructed,
    Max,
};

#define TERRAIN_GRID_SIZE 512
#define TERRAIN_GRID_CELL_SIZE 1

TerrainType Terrain[TERRAIN_GRID_SIZE * TERRAIN_GRID_SIZE];

shared_ptr<SDL_Texture> worldTexture;

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

            lastGameUpdateTime = endTime - startTime;

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

bool middleButtonDown = false;

auto prevMouseX = 0, prevMouseY = 0;

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
            if (evt.key.keysym.sym == SDLK_3)
                attractionType = BacteriaType::Gobbler;
            if (evt.key.keysym.sym == SDLK_4)
                attractionType = BacteriaType::Zoomer;
            if (evt.key.keysym.sym == SDLK_5)
                attractionType = BacteriaType::Spitter;
        }

        if (evt.type == SDL_KEYUP)
        {
            if (evt.key.keysym.sym == SDLK_1 || evt.key.keysym.sym == SDLK_2 || evt.key.keysym.sym == SDLK_3 || evt.key.keysym.sym == SDLK_4 || evt.key.keysym.sym == SDLK_5)
                attractionType = BacteriaType::Invalid;
        }

        if (evt.type == SDL_MOUSEMOTION)
        {
            auto mouseX = evt.motion.x;
            auto mouseY = evt.motion.y;

            if (middleButtonDown)
            {
                camera.CenterPos = camera.CenterPos - (Vector2(mouseX, mouseY) - Vector2(prevMouseX, prevMouseY)) / camera.ZoomLevel;
            }

            prevMouseX = mouseX;
            prevMouseY = mouseY;
        }

        if (evt.type == SDL_MOUSEBUTTONDOWN && evt.button.button == SDL_BUTTON_MIDDLE)
        {
            middleButtonDown = true;
        }

        if (evt.type == SDL_MOUSEBUTTONUP && evt.button.button == SDL_BUTTON_MIDDLE)
        {
            middleButtonDown = false;
        }

        if (evt.type == SDL_MOUSEWHEEL && evt.wheel.y != 0)
        {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);

            float oldZoom = camera.ZoomLevel;
            camera.ZoomStep += evt.wheel.y / abs(evt.wheel.y);
            camera.SetZoomLevelFromZoomStep();
            float newZoom = camera.ZoomLevel;

            int wndW, wndH;

            SDL_GetWindowSize(wnd, &wndW, &wndH);

            float zoomRatio = newZoom / oldZoom;

            float expectedMoveX = (mouseX - wndW / 2) * zoomRatio - (mouseX - wndW / 2);
            float expectedMoveY = (mouseY - wndH / 2) * zoomRatio - (mouseY - wndH / 2);

            camera.CenterPos = camera.CenterPos + Vector2(expectedMoveX, expectedMoveY) / newZoom;
        }
    }

    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    attractionPoint = camera.ScreenToReal(Vector2(mouseX, mouseY));

    SDL_SetRenderDrawColor(rnd, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(rnd);

    elapsedTime = GetTimeAsDouble();

    {
        auto gameStateBeingRendered = currentGameState;

        lock_guard rendererLockGuard(gameStates[gameStateBeingRendered].mutex);

#if _DEBUG
        gameStates[gameStateBeingRendered].BeingRendered = true;
#endif

        int windowW, windowH;
        SDL_GetWindowSize(wnd, &windowW, &windowH);

        camera.ScreenPixelSize = Vector2(windowW, windowH);

        Vector2 bv2 = camera.RealToScreen(Vector2((-TERRAIN_GRID_SIZE * TERRAIN_GRID_CELL_SIZE) / 2, (-TERRAIN_GRID_SIZE * TERRAIN_GRID_CELL_SIZE) / 2));

        SDL_FRect terRect;
        terRect.x = bv2.X;
        terRect.y = bv2.Y;
        terRect.w = camera.RealToScreenScale(TERRAIN_GRID_CELL_SIZE * TERRAIN_GRID_SIZE);
        terRect.h = camera.RealToScreenScale(TERRAIN_GRID_CELL_SIZE * TERRAIN_GRID_SIZE);

        SDL_RenderCopyF(rnd, worldTexture.get(), nullptr, &terRect);

        for (auto i = 0; i < gameStates[gameStateBeingRendered].NumActiveBacteria; ++i)
        {
            if (gameStates[gameStateBeingRendered].BacteriaList[i].Health <= 0)
                continue;
#if _DEBUG
            if (gameStates[gameStateBeingRendered].BeingUpdated)
            {
                throw "Game state is also being updated!!!";
            }
#endif
            gameStates[gameStateBeingRendered].BacteriaList[i].Render(rnd, camera, elapsedTime);
        }

#if _DEBUG
        gameStates[gameStateBeingRendered].BeingRendered = false;
#endif
    }

    if (showingDebugInfo)
    {
        ostringstream oss;
        oss << "FPS: " << currentFPS << " N: " << gameStates[currentGameState].NumActiveBacteria << " Update Time: " << (int64_t)(lastGameUpdateTime * 1000000) << " CP: " << camera.CenterPos.X << "x" << camera.CenterPos.Y;
        DrawText(oss.str(), Vector2(20, 20));
    }

    SDL_RenderPresent(rnd);
}

void EnterInGameState(string levelName)
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

    SDL_Surface *lvlSurf = IMG_Load(levelName.c_str());
    SDL_LockSurface(lvlSurf);

    SDL_Surface *worldTextureSurface = SDL_CreateRGBSurface(0, TERRAIN_GRID_SIZE, TERRAIN_GRID_SIZE, 32, 0, 0, 0, 0);
    SDL_LockSurface(worldTextureSurface);

    auto pixelPtr = (uint32_t *)lvlSurf->pixels;
    auto worldTexturePixelPtr = (uint32_t *)worldTextureSurface->pixels;

    int32_t obstructedTiles = 0;
    int32_t clearTiles = 0;

    for (int y = 0; y < lvlSurf->h; ++y)
    {
        for (int x = 0; x < lvlSurf->w; ++x)
        {
            auto curPtr = pixelPtr + x + y * lvlSurf->w;
            auto pixel = *curPtr;

            auto curPtrWT = worldTexturePixelPtr + x + y * worldTextureSurface->w;

            if (pixel == 0xFFFFFFFF)
            {
                Terrain[x + y * TERRAIN_GRID_SIZE] = TerrainType::Obstructed;
                obstructedTiles++;

                *curPtrWT = 0xFFFFBBBB;
            }
            else
            {
                Terrain[x + y * TERRAIN_GRID_SIZE] = TerrainType::Clear;
                clearTiles++;

                *curPtrWT = 0xFF333333;
            }

            auto pp = Vector2(x - TERRAIN_GRID_SIZE / 2, y - TERRAIN_GRID_SIZE / 2);

            // ALLIED
            if (pixel == 0xff0059ff)
            {
                Bacteria b;
                b.Position = pp;
                b.Type = BacteriaType::Swarmer;
                b.Faction = 0;
                gameStates[0].AddBacteria(b);
            }

            if (pixel == 0xff0000ff)
            {
                Bacteria b;
                b.Position = pp;
                b.Type = BacteriaType::Converter;
                b.Faction = 0;
                gameStates[0].AddBacteria(b);
            }

            if (pixel == 0xff00aaff)
            {
                Bacteria b;
                b.Position = pp;
                b.Type = BacteriaType::Gobbler;
                b.Faction = 0;
                gameStates[0].AddBacteria(b);
            }

            if (pixel == 0xff00ffff)
            {
                Bacteria b;
                b.Position = pp;
                b.Type = BacteriaType::Zoomer;
                b.Faction = 0;
                gameStates[0].AddBacteria(b);
            }

            // ENEMY
            if (pixel == 0xffff5900)
            {
                Bacteria b;
                b.Position = pp;
                b.Type = BacteriaType::Swarmer;
                b.Faction = 1;
                gameStates[0].AddBacteria(b);
            }

            if (pixel == 0xffff0000)
            {
                Bacteria b;
                b.Position = pp;
                b.Type = BacteriaType::Converter;
                b.Faction = 1;
                gameStates[0].AddBacteria(b);
            }

            if (pixel == 0xffffaa00)
            {
                Bacteria b;
                b.Position = pp;
                b.Type = BacteriaType::Gobbler;
                b.Faction = 1;
                gameStates[0].AddBacteria(b);
            }

            if (pixel == 0xffffff00)
            {
                Bacteria b;
                b.Position = pp;
                b.Type = BacteriaType::Zoomer;
                b.Faction = 1;
                gameStates[0].AddBacteria(b);
            }
        }
    }

    DUMP(obstructedTiles);
    DUMP(clearTiles);

    SDL_UnlockSurface(worldTextureSurface);

    worldTexture = shared_ptr<SDL_Texture>(SDL_CreateTextureFromSurface(rnd, worldTextureSurface), SDL_DestroyTexture);

    SDL_FreeSurface(worldTextureSurface);

    // for (int i = 0; i < 50; ++i)
    // {
    //     Bacteria b1;
    //     b1.Position = Vector2(i, 0);
    //     b1.Type = BacteriaType::Swarmer;
    //     b1.Velocity = Vector2(0, 0);
    //     b1.Faction = 0;

    //     gameStates[0].AddBacteria(b1);

    //     Bacteria b2;
    //     b2.Position = Vector2(i + 0.5f, 4);
    //     b2.Type = BacteriaType::Converter;
    //     b2.Velocity = Vector2(0, 0);
    //     b2.Faction = 0;

    //     gameStates[0].AddBacteria(b2);

    //     Bacteria b3;
    //     b3.Position = Vector2(i + 0.5f, -10);
    //     b3.Type = BacteriaType::Zoomer;
    //     b3.Velocity = Vector2(0, 0);
    //     b3.Faction = 1;

    //     gameStates[0].AddBacteria(b3);
    // }

    SDL_FreeSurface(lvlSurf);

    emscripten_cancel_main_loop();

    cout << "About to call emscripten_set_main_loop" << endl;
    emscripten_set_main_loop(InGameMainLoop, 0, 0);

    gameUpdateThread = thread(GameUpdateThread);

    testImage = LoadTexture("assets/xt3.xcf");
}