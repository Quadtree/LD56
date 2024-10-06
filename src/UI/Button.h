#pragma once

class Button
{
public:
    int Index;
    string Text;

    function<void()> OnClick;
};