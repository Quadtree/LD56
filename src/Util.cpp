#include "LD56.h"
#include "Util.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

using namespace std;

shared_ptr<SDL_Texture> LoadTexture(string filename)
{
    auto tmpSurf = IMG_Load(filename.c_str());
    DUMP(tmpSurf);
    auto ret = SDL_CreateTextureFromSurface(rnd, testSurf);
    DUMP(ret);

    return shared_ptr<SDL_Texture>(ret, SDL_DestroyTexture);
}