#include "LD56.h"
#include "Util.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include "GameState/Vector2.h"

#include <thread>

using namespace std;

extern SDL_Renderer *rnd;
extern thread::id mainThreadId;

TTF_Font *defaultFont;

shared_ptr<SDL_Texture> LoadTexture(string filename)
{
    AssertOnMainThread();

    auto tmpSurf = IMG_Load(filename.c_str());
    DUMP(tmpSurf);
    auto ret = SDL_CreateTextureFromSurface(rnd, tmpSurf);
    DUMP(ret);

    SDL_FreeSurface(tmpSurf);

    return shared_ptr<SDL_Texture>(ret, SDL_DestroyTexture);
}

void _AssertOnMainThread(int line, string file)
{
    if (this_thread::get_id() != mainThreadId)
    {
        cout << file << " (" << line << "): Should be on main thread, but was on " << this_thread::get_id() << " instead!" << endl;
        exit(1);
    }
}

double GetTimeAsDouble()
{
    return (double)SDL_GetPerformanceCounter() / SDL_GetPerformanceFrequency();
}

void DrawText(std::string text, class Vector2 *pos)
{
    if (!defaultFont)
    {
        defaultFont = TTF_OpenFont("assets/Roboto-Regular.ttf", 24);
    }

    SDL_Color color;
    color.r = 255;
    color.g = 255;
    color.b = 255;
    color.a = 255;

    auto surf = TTF_RenderUTF8_Blended(defaultFont, text.c_str(), color);

    auto tex = SDL_CreateTextureFromSurface(rnd, surf);

    SDL_FRect trg;
    trg.x = pos->x;
    trg.y = pos->y;
    trg.w = surf->w;
    trg.h = surf->h;

    SDL_RenderCopyF(rnd, tex, nullptr, &trg);

    SDL_DestroyTexture(tex);
    SDL_FreeSurface(surf);
}