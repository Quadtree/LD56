#include "../LD56.h"
#include "Bacteria.h"
#include "../Camera.h"
#include "../Util.h"

void Bacteria::Update1(Bacteria &nextState, class MutationQueue *queueMutation) const
{
    nextState = *this;
    // nextState.Position = Vector2(Position.X, Position.Y + 1);

#if _DEBUG
    nextState.NumUpdates = NumUpdates + 1;
#endif
}

vector<shared_ptr<SDL_Texture>> bacteriaTextures;

void Bacteria::Render(SDL_Renderer *rnd, Camera &camera)
{
    if (bacteriaTextures.size() == 0)
    {
        bacteriaTextures.resize((int)BacteriaType::Max);

        bacteriaTextures[(int)BacteriaType::Converter] = LoadTexture("assets/converter1.xcf");
    }
}
