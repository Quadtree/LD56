#include "LD56.h"
#include "Util.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include <thread>

using namespace std;

extern SDL_Renderer *rnd;
extern thread::id mainThreadId;

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

