#include "LD56.h"
#include "Util.h"
#include "GameState/GameState.h"
#include "Camera.h"

thread gameUpdateThread;
Uint64 ticksHandledByGameStateUpdates;

extern SDL_Renderer *rnd;
extern SDL_Window *wnd;

shared_ptr<SDL_Texture> testImage;

GameState gameStates[2];
int currentGameState = 0;
int nextGameStateToUpdate = 1;

bool showingDebugInfo = false;
int currentFPS = 0;

Camera camera;

Vector2 attractionPoint;
bool attractionTypes[(int)BacteriaType::Max];

double lastGameUpdateTime = 0;
double lastRenderTime = 0;

atomic<double> elapsedTime;
atomic<double> elapsedGameUpdateTime;

bool gameRunning = true;

#define TERRAIN_GRID_CELL_SIZE 1

TerrainType Terrain[TERRAIN_GRID_SIZE * TERRAIN_GRID_SIZE];

shared_ptr<SDL_Texture> worldTexture;

extern function<void()> teardownFunction;

vector<function<void(GameState *)>> preUpdateFunctions;

bool anyAlliesAlive = true;
bool anyEnemiesAlive = true;

int numAlliesAlive = 0;
int numEnemiesAlive = 0;

string currentLevelName;
int currentLevelNumber;

void UpdateWorldState()
{
    lock_guard gameStateLock(gameStates[nextGameStateToUpdate].mutex);

    memcpy(gameStates[currentGameState].AttractionPoints[0].Types, attractionTypes, sizeof(attractionTypes));
    gameStates[currentGameState].AttractionPoints[0].Location = attractionPoint;

    gameStates[currentGameState].DoUpdate(gameStates[nextGameStateToUpdate], Terrain);

    for (auto &it : preUpdateFunctions)
    {
        it(&gameStates[nextGameStateToUpdate]);
    }

    preUpdateFunctions.resize(0);

    auto loc_anyAlliesAlive = false;
    auto loc_anyEnemiesAlive = false;
    auto loc_numAlliesAlive = 0;
    auto loc_numEnemiesAlive = 0;

    for (int i = 0; i < gameStates[nextGameStateToUpdate].NumActiveBacteria; ++i)
    {
        if (gameStates[nextGameStateToUpdate].BacteriaList[i].Health > 0)
        {
            if (gameStates[nextGameStateToUpdate].BacteriaList[i].Faction == 0)
            {
                loc_numAlliesAlive++;
                loc_anyAlliesAlive = true;
            }
            if (gameStates[nextGameStateToUpdate].BacteriaList[i].Faction == 1)
            {
                loc_numEnemiesAlive++;
                loc_anyEnemiesAlive = true;
            }
        }
    }

    anyAlliesAlive = loc_anyAlliesAlive;
    anyEnemiesAlive = loc_anyEnemiesAlive;
    numAlliesAlive = loc_numAlliesAlive;
    numEnemiesAlive = loc_numEnemiesAlive;

    nextGameStateToUpdate = currentGameState;
    currentGameState = 1 - currentGameState;

    elapsedGameUpdateTime = GetTimeAsDouble();
}

void GameUpdateThread()
{
    auto ticksPerGameUpdate = SDL_GetPerformanceFrequency() / 60;

    Uint64 lastSecond = 0;
    auto updatesLastSecond = 0;

    ticksHandledByGameStateUpdates = SDL_GetPerformanceCounter();

    while (gameRunning)
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

bool scrollLeft = false, scrollRight = false, scrollUp = false, scrollDown = false;

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
                attractionTypes[(int)BacteriaType::Converter] = true;
            if (evt.key.keysym.sym == SDLK_2)
                attractionTypes[(int)BacteriaType::Swarmer] = true;
            if (evt.key.keysym.sym == SDLK_3)
                attractionTypes[(int)BacteriaType::Gobbler] = true;
            if (evt.key.keysym.sym == SDLK_4)
                attractionTypes[(int)BacteriaType::Zoomer] = true;
            if (evt.key.keysym.sym == SDLK_5)
                attractionTypes[(int)BacteriaType::Spitter] = true;

            if (evt.key.keysym.sym == SDLK_LEFT)
                scrollLeft = true;
            if (evt.key.keysym.sym == SDLK_RIGHT)
                scrollRight = true;
            if (evt.key.keysym.sym == SDLK_UP)
                scrollUp = true;
            if (evt.key.keysym.sym == SDLK_DOWN)
                scrollDown = true;

            if (evt.key.keysym.sym == SDLK_EQUALS)
            {
                camera.ZoomStep++;
                camera.SetZoomLevelFromZoomStep();
            }

            if (evt.key.keysym.sym == SDLK_MINUS)
            {
                camera.ZoomStep--;
                camera.SetZoomLevelFromZoomStep();
            }

#if CHEATS_ENABLED
            if (evt.key.keysym.sym == SDLK_F6)
            {
                preUpdateFunctions.push_back([](GameState *gs)
                                             {
                                                cout << "Activating INSTANT WIN CHEAT!!!" << endl;
                    for (int i=0;i<gs->NumActiveBacteria;++i)
                    {
                        if (gs->BacteriaList[i].Faction == 1) gs->BacteriaList[i].Health = 0;
                    } });
            }
#endif
        }

        if (evt.type == SDL_KEYUP)
        {
            if (evt.key.keysym.sym == SDLK_1)
                attractionTypes[(int)BacteriaType::Converter] = false;
            if (evt.key.keysym.sym == SDLK_2)
                attractionTypes[(int)BacteriaType::Swarmer] = false;
            if (evt.key.keysym.sym == SDLK_3)
                attractionTypes[(int)BacteriaType::Gobbler] = false;
            if (evt.key.keysym.sym == SDLK_4)
                attractionTypes[(int)BacteriaType::Zoomer] = false;
            if (evt.key.keysym.sym == SDLK_5)
                attractionTypes[(int)BacteriaType::Spitter] = true;

            if (evt.key.keysym.sym == SDLK_LEFT)
                scrollLeft = false;
            if (evt.key.keysym.sym == SDLK_RIGHT)
                scrollRight = false;
            if (evt.key.keysym.sym == SDLK_UP)
                scrollUp = false;
            if (evt.key.keysym.sym == SDLK_DOWN)
                scrollDown = false;
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

    // 0xFFFFBBBB

    auto renderStartTime = GetTimeAsDouble();

    SDL_SetRenderDrawColor(rnd, 0xFF, 0xBB, 0xBB, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(rnd);

    auto deltaTime = GetTimeAsDouble() - elapsedTime;

    const auto CAMERA_SCROLL_SPEED = 700;

    if (scrollLeft)
        camera.CenterPos += Vector2(-1, 0) * deltaTime * CAMERA_SCROLL_SPEED / camera.ZoomLevel;
    if (scrollRight)
        camera.CenterPos += Vector2(1, 0) * deltaTime * CAMERA_SCROLL_SPEED / camera.ZoomLevel;
    if (scrollUp)
        camera.CenterPos += Vector2(0, -1) * deltaTime * CAMERA_SCROLL_SPEED / camera.ZoomLevel;
    if (scrollDown)
        camera.CenterPos += Vector2(0, 1) * deltaTime * CAMERA_SCROLL_SPEED / camera.ZoomLevel;

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

        auto timeSinceLastUpdate = (elapsedTime - elapsedGameUpdateTime) * currentFPS / 60;

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
            gameStates[gameStateBeingRendered].BacteriaList[i].Render(rnd, camera, elapsedTime, timeSinceLastUpdate);
        }

#if _DEBUG
        gameStates[gameStateBeingRendered].BeingRendered = false;
#endif
    }

    if (showingDebugInfo)
    {
        ostringstream oss;
        oss << "FPS: " << currentFPS << " N: " << gameStates[currentGameState].NumActiveBacteria << " Update Time: " << (int64_t)(lastGameUpdateTime * 1000000) << " Render Time: " << (int64_t)(lastRenderTime * 1000000) << " CP: " << camera.CenterPos.X << "x" << camera.CenterPos.Y;
        DrawText(oss.str(), Vector2(20, 20), 24, {255, 255, 255, 255});
    }

    if (!anyAlliesAlive)
    {
        cout << "Player has LOST the level" << currentLevelNumber << "!" << endl;
        EnterMessageScreen("You lose!", []()
                           { EnterInGameState(currentLevelNumber); });
    }

    if (!anyEnemiesAlive)
    {
        cout << "Player has WON the level" << currentLevelNumber << "!" << endl;
        if (currentLevelNumber >= 4)
        {
            EnterMessageScreen("You win!", []()
                               { EnterMainMenuState(); });
            return;
        }
        else
        {
            EnterInGameState(currentLevelNumber + 1);
            return;
        }
    }

    SDL_RenderPresent(rnd);

    lastRenderTime = GetTimeAsDouble() - renderStartTime;

    FlushSoundQueue();
}

void EnterInGameState(int levelNumber)
{
    CallTearDownFunction();

    cout << "EnterInGameState(" << levelNumber << ")" << endl;

    currentLevelNumber = levelNumber;

    {
        ostringstream oss;
        oss << "assets/level" << currentLevelNumber << ".xcf";

        currentLevelName = oss.str();
    }

    for (int i = 0; i < 2; ++i)
        gameStates[i].Reset();

    currentGameState = 0;
    nextGameStateToUpdate = 1;

    showingDebugInfo = false;
    currentFPS = 0;

    camera = Camera();

    attractionPoint = Vector2();
    memset(attractionTypes, 0, sizeof(attractionTypes));

    SDL_Surface *lvlSurf = IMG_Load(currentLevelName.c_str());
    SDL_LockSurface(lvlSurf);

    SDL_Surface *worldTextureSurface = SDL_CreateRGBSurface(0, TERRAIN_GRID_SIZE, TERRAIN_GRID_SIZE, 32, 0, 0, 0, 0);
    SDL_LockSurface(worldTextureSurface);

    auto pixelPtr = (uint32_t *)lvlSurf->pixels;
    auto worldTexturePixelPtr = (uint32_t *)worldTextureSurface->pixels;

    int32_t obstructedTiles = 0;
    int32_t clearTiles = 0;

    gameRunning = true;

    preUpdateFunctions.resize(0);

    anyAlliesAlive = true;
    anyEnemiesAlive = true;

    camera.Reset();

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

    teardownFunction = []()
    {
        cout << "InGame teardownFunction START" << endl;
        gameRunning = false;
        gameUpdateThread.join();
        cout << "InGame teardownFunction DONE" << endl;
    };
}