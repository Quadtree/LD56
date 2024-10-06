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

#define GOBBLER_ATTACK_SPEED (2.0f / 1.0f)
#define GOBBLER_ATTACK_RANGE 1
#define GOBBLER_ATTACK_COOLDOWN 90
#define GOBBLER_ATTACK_DAMAGE 40

#define ZOOMER_DODGE_FORCE 60
#define ZOOMER_DODGE_CHANCE 10
#define ZOOMER_ATTACK_SPEED (2.0f / 1.0f)
#define ZOOMER_ATTACK_RANGE 1
#define ZOOMER_ATTACK_COOLDOWN 90
#define ZOOMER_ATTACK_DAMAGE 40

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

            queueMutation->QueueMutation(1, [targetID](GameState *gs)
                                         { gs->BacteriaList[targetID].Health -= SWARMER_ATTACK_DAMAGE; });

            nextState.AttackCharge = 0;
        }
    }

    if (closestBacteria && Type == BacteriaType::Gobbler && nextState.AttackCharge >= GOBBLER_ATTACK_COOLDOWN)
    {
        auto delta = (closestBacteria->Position - Position).Normalized();
        nextState.Velocity += delta * GOBBLER_ATTACK_SPEED;

        if (closestBacteria->Position.DistToSquared(Position) <= SQUARE(GOBBLER_ATTACK_RANGE) && nextState.AttackCharge >= GOBBLER_ATTACK_COOLDOWN)
        {
            auto targetID = closestBacteria->ID;
            auto targetIsZoomer = closestBacteria->Type == BacteriaType::Zoomer;

            queueMutation->QueueMutation(1, [targetID, targetIsZoomer](GameState *gs)
                                         {
                                                if (!targetIsZoomer && rand() % ZOOMER_DODGE_CHANCE == 0){
                                                    gs->BacteriaList[targetID].Health -= GOBBLER_ATTACK_DAMAGE;
                                                } else {
                                                    gs->BacteriaList[targetID].Velocity += Vector2(rand() % (ZOOMER_DODGE_FORCE*2) - ZOOMER_DODGE_FORCE, rand() % (ZOOMER_DODGE_FORCE*2) - ZOOMER_DODGE_FORCE);
                                                } });

            nextState.AttackCharge = 0;
        }
    }

    if (closestBacteria && Type == BacteriaType::Zoomer)
    {
        auto delta = (closestBacteria->Position - Position).Normalized();
        nextState.Velocity += delta * ZOOMER_ATTACK_SPEED;

        if (closestBacteria->Position.DistToSquared(Position) <= SQUARE(ZOOMER_ATTACK_RANGE) && nextState.AttackCharge >= ZOOMER_ATTACK_COOLDOWN)
        {
            auto targetID = closestBacteria->ID;

            queueMutation->QueueMutation(1, [targetID](GameState *gs)
                                         { gs->BacteriaList[targetID].Health -= ZOOMER_ATTACK_DAMAGE; });

            nextState.AttackCharge = 0;
        }
    }

    if (curGameState->AttractionPoints[Faction].Type == Type && curGameState->AttractionPoints[Faction].Location.DistToSquared(Position) <= SQUARE(CURSOR_ATTRACTION_RADIUS))
    {
        auto delta = (curGameState->AttractionPoints[Faction].Location - Position).Normalized();
        nextState.Velocity += delta * CURSOR_ATTRACTION_POWER;
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
        bacteriaTextures[(int)BacteriaType::Gobbler] = LoadTexture("assets/gobbler.xcf");
        bacteriaTextures[(int)BacteriaType::Zoomer] = LoadTexture("assets/zoomer.xcf");
        bacteriaTextures[(int)BacteriaType::Spitter] = LoadTexture("assets/spitter.xcf");
    }

    auto screen = camera.RealToScreen(Position);

    SDL_FRect trg;
    trg.x = screen.X;
    trg.y = screen.Y;
    trg.w = camera.RealToScreenScale(1);
    trg.h = camera.RealToScreenScale(1);

    SDL_SetTextureColorMod(bacteriaTextures[(int)Type].get(), FACTION_COLORS[Faction].r, FACTION_COLORS[Faction].g, FACTION_COLORS[Faction].b);
    SDL_RenderCopyExF(rnd, bacteriaTextures[(int)Type].get(), nullptr, &trg, 0, nullptr, SDL_FLIP_NONE);
}
