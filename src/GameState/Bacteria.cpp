#include "../LD56.h"
#include "Bacteria.h"
#include "../Camera.h"
#include "../Util.h"
#include "GameState.h"

void Bacteria::Update1(Bacteria &nextState, const GameState *curGameState, class MutationQueue *queueMutation) const
{
    nextState = *this;
    // nextState.Position = Vector2(Position.X, Position.Y + 1);

    auto nearbyBacteria = curGameState->GetBacteriaNear(Position, 5);

    for (auto &it : nearbyBacteria)
    {
        if (it->ID == ID)
            continue;

        auto delta = (Position - it->Position).Normalized();
        nextState.Velocity += delta / 20;
    }

    nextState.Position += Velocity / 60;

#if _DEBUG
    nextState.NumUpdates = NumUpdates + 1;
#endif
}

vector<shared_ptr<SDL_Texture>> bacteriaTextures;

void Bacteria::Render(SDL_Renderer *rnd, Camera &camera) const
{
    if (bacteriaTextures.size() == 0)
    {
        bacteriaTextures.resize((int)BacteriaType::Max);

        bacteriaTextures[(int)BacteriaType::Converter] = LoadTexture("assets/converter1.xcf");
    }

    auto screen = camera.RealToScreen(Position);

    SDL_FRect trg;
    trg.x = screen.X;
    trg.y = screen.Y;
    trg.w = camera.RealToScreenScale(1);
    trg.h = camera.RealToScreenScale(1);

    SDL_RenderCopyExF(rnd, bacteriaTextures[(int)Type].get(), nullptr, &trg, 20, nullptr, SDL_FLIP_NONE);
}
