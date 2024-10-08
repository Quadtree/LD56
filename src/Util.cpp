#include "LD56.h"
#include "Util.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include <thread>

using namespace std;

extern SDL_Renderer *rnd;
extern thread::id mainThreadId;

unordered_map<int, TTF_Font *> defaultFonts;

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

TTF_Font *GetFont(int ptSize)
{
    if (defaultFonts.find(ptSize) == defaultFonts.end())
        defaultFonts.insert(pair(ptSize, TTF_OpenFont("assets/Roboto-Regular.ttf", ptSize)));

    return defaultFonts[ptSize];
}

void PlaySound(std::string filename, float volume)
{
    // cout << "PlaySound(" << filename << ", " << volume << ")" << endl;
    EM_ASM_({ playSound(UTF8ToString($0), $1); }, filename.c_str(), volume);
}

mutex soundQueueMutex;

struct SoundQueueEntry
{
    string Filename;
    float Volume;
};

vector<SoundQueueEntry> soundQueue;

void QueueSound(std::string filename, float volume)
{
    lock_guard soundQueueMutexGuard(soundQueueMutex);

    soundQueue.push_back({filename, volume});
}

void FlushSoundQueue()
{
    for (auto &it : soundQueue)
    {
        PlaySound(it.Filename, it.Volume);
    }

    soundQueue.resize(0);
}

void DrawText(std::string text, Vector2 pos, int ptSize, SDL_Color color)
{
    // SDL_Color color;
    // color.r = 255;
    // color.g = 255;
    // color.b = 255;
    // color.a = 255;

    auto surf = TTF_RenderUTF8_Blended(GetFont(ptSize), text.c_str(), color);

    auto tex = SDL_CreateTextureFromSurface(rnd, surf);

    SDL_FRect trg;
    trg.x = pos.X;
    trg.y = pos.Y;
    trg.w = surf->w;
    trg.h = surf->h;

    SDL_RenderCopyF(rnd, tex, nullptr, &trg);

    SDL_DestroyTexture(tex);
    SDL_FreeSurface(surf);
}

void DrawTextWithWrap(std::string text, Vector2 pos, int ptSize, SDL_Color color, int wrapPixels)
{
    // SDL_Color color;
    // color.r = 255;
    // color.g = 255;
    // color.b = 255;
    // color.a = 255;

    auto surf = TTF_RenderUTF8_Blended_Wrapped(GetFont(ptSize), text.c_str(), color, wrapPixels);

    auto tex = SDL_CreateTextureFromSurface(rnd, surf);

    SDL_FRect trg;
    trg.x = pos.X;
    trg.y = pos.Y;
    trg.w = surf->w;
    trg.h = surf->h;

    SDL_RenderCopyF(rnd, tex, nullptr, &trg);

    SDL_DestroyTexture(tex);
    SDL_FreeSurface(surf);
}