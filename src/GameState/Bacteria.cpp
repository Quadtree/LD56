#include "../LD56.h"
#include "Bacteria.h"
#include "../Camera.h"
#include "../Util.h"
#include "GameState.h"

#define DEFAULT_REPULSION_RADIUS 5
#define DEFAULT_REPULSION_POWER (1f / 10f)
#define CURSOR_ATTRACTION_RADIUS 10
#define CURSOR_ATTRACTION_POWER (1f / 5f)

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
        nextState.Velocity += (delta * (SQUARE(DEFAULT_REPULSION_RADIUS) - Position.DistToSquared(it->Position))) * DEFAULT_REPULSION_POWER;
    }

    if (curGameState->AttractionPoints[Faction].Type == Type && curGameState->AttractionPoints[Faction].Location.DistToSquared(Position) <= SQUARE(CURSOR_ATTRACTION_RADIUS))
    {
        auto delta = (curGameState->AttractionPoints[Faction].Location - Position).Normalized();
        nextState.Velocity += (delta * (SQUARE(CURSOR_ATTRACTION_RADIUS) - Position.DistToSquared(curGameState->AttractionPoints[Faction].Location))) * CURSOR_ATTRACTION_POWER;
    }

    nextState.Position += Velocity / 60;

    nextState.Velocity *= 0.9f;

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
