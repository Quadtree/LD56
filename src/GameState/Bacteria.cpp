#include "../LD56.h"
#include "Bacteria.h"
#include "../Camera.h"
#include "../Util.h"
#include "GameState.h"

#define DEFAULT_REPULSION_RADIUS 3
#define DEFAULT_REPULSION_POWER (1.0f / 10.0f)
#define CURSOR_ATTRACTION_RADIUS 22
#define CURSOR_ATTRACTION_POWER (1.0f / 1.0f)

#define SWARMER_ATTACK_SPEED (1.0f / 1.0f)
#define SWARMER_ATTACK_RANGE 1
#define SWARMER_ATTACK_COOLDOWN 30
#define SWARMER_ATTACK_DAMAGE 1

SDL_Color FACTION_COLORS[] = {
    {0, 0, 255, 255},
    {255, 0, 0, 255}};

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
        nextState.Velocity += (delta * min(40.f, 1.f / Position.DistToSquared(it->Position))) * DEFAULT_REPULSION_POWER;
    }

    if (curGameState->AttractionPoints[Faction].Type == Type && curGameState->AttractionPoints[Faction].Location.DistToSquared(Position) <= SQUARE(CURSOR_ATTRACTION_RADIUS))
    {
        auto delta = (curGameState->AttractionPoints[Faction].Location - Position).Normalized();
        nextState.Velocity += delta * CURSOR_ATTRACTION_POWER;
    }
    else
    {
        float bestSquaredDist = 200000;
        const Bacteria *closestBacteria = nullptr;

        if (Type == BacteriaType::Swarmer || Type == BacteriaType::Gobbler || Type == BacteriaType::Zoomer)
        {
            for (auto &it : nearbyBacteria)
            {
                if (it->Faction == Faction)
                    continue;

                auto dist = it->Position.DistToSquared(Position);
                if (dist < bestSquaredDist)
                {
                    bestSquaredDist = dist;
                    closestBacteria = it;
                }
            }
        }

        if (closestBacteria && Type == BacteriaType::Swarmer)
        {
            auto delta = (closestBacteria->Position - Position).Normalized();
            nextState.Velocity += delta * SWARMER_ATTACK_SPEED;

            if (closestBacteria->Position.DistToSquared(Position) <= SQUARE(SWARMER_ATTACK_RANGE) && nextState.AttackCharge >= SWARMER_ATTACK_COOLDOWN)
            {
                auto targetID = closestBacteria->ID;

                // cout << ID << " ATK " << closestBacteria->ID << " " << (int)closestBacteria->Health << endl;

                queueMutation->QueueMutation(1, [targetID](GameState *gs)
                                             { gs->BacteriaList[targetID].Health -= SWARMER_ATTACK_DAMAGE; });

                nextState.AttackCharge = 0;
            }
        }
    }

    nextState.Position += Velocity / 60;

    nextState.Velocity *= 0.9f;
    nextState.AttackCharge++;

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
        bacteriaTextures[(int)BacteriaType::Swarmer] = LoadTexture("assets/swarmer.xcf");
    }

    auto screen = camera.RealToScreen(Position);

    SDL_FRect trg;
    trg.x = screen.X;
    trg.y = screen.Y;
    trg.w = camera.RealToScreenScale(1);
    trg.h = camera.RealToScreenScale(1);

    SDL_SetTextureColorMod(bacteriaTextures[(int)Type].get(), FACTION_COLORS[Faction].r, FACTION_COLORS[Faction].g, FACTION_COLORS[Faction].b);
    SDL_RenderCopyExF(rnd, bacteriaTextures[(int)Type].get(), nullptr, &trg, 20, nullptr, SDL_FLIP_NONE);
}
