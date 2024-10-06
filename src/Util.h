#include "GameState/Vector2.h"

std::shared_ptr<class SDL_Texture> LoadTexture(std::string filename);
void _AssertOnMainThread(int line, std::string file);

#ifdef _DEBUG
#define AssertOnMainThread() _AssertOnMainThread(__LINE__, __FILE__)
#else
#define AssertOnMainThread()
#endif

double GetTimeAsDouble();

#define TIME_COMMAND(x)                                                                 \
    {                                                                                   \
        auto startTime = GetTimeAsDouble();                                             \
        x;                                                                              \
        auto endTime = GetTimeAsDouble();                                               \
        cout << #x << " time: " << (int)((endTime - startTime) * 1000) << "ms" << endl; \
    }

void DrawText(std::string text, Vector2 pos, int ptSize, SDL_Color color);
TTF_Font *GetFont(int ptSize);

void PlaySound(std::string filename, float volume);
void QueueSound(std::string filename, float volume);
void FlushSoundQueue();