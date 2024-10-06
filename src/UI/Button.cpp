#include "Button.h"

void Button::Setup(int index, string text, function<void()> onClick)
{
    Index = index;
    Text = text;
    OnClick = onClick;
}