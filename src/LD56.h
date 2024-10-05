// LD56.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>
#include <memory>
#include <thread>

using namespace std;

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include <emscripten.h>
#include <emscripten/bind.h>

#define DUMP(x) cout << #x << "=" << x << endl;
