#include "LD56.h"
#include "Util.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

using namespace std;

extern SDL_Renderer *rnd;

shared_ptr<SDL_Texture> LoadTexture(string filename)
{
    auto tmpSurf = IMG_Load(filename.c_str());
    DUMP(tmpSurf);
    auto ret = SDL_CreateTextureFromSurface(rnd, tmpSurf);
    DUMP(ret);

    SDL_DestroySurface(tmpSurf);

    return shared_ptr<SDL_Texture>(ret, SDL_DestroyTexture);
}