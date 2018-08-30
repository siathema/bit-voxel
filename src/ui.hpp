#pragma once

#include "sMath.hpp"
#include "renderer.hpp"
//#include "../sync.hpp"

namespace SMOBA
{

    static const vec4 UIColorDark(0.54f, 0.54f, 0.80f, 1.0f);
    static const vec4 UIColorMid(0.7f, 0.7f, 0.90f, 0.80f);
    static const vec4 UIColorLight(0.9f, 0.9f, 1.0f, 1.0f);
    static const vec4 UIColorTextDark(0.2f, 0.2f, 0.2f, 1.0f);

    typedef unsigned short UI_ID;
    struct UI_Context
    {
        UI_ID Hot;
        UI_ID Active;
    };

    struct Debug_Info_Panel
    {
        char Message[20];
        vec2 Pos;
        vec2 Size;
        vec2 PreviousMouse;
        u32 Cooldown;
        r32 SliderValue;
        UI_ID Panel;
        UI_ID TextRect;
        UI_ID Slider;
        b8 Closed;
        b8 MovingSlider;
        b8 Moving;
    };

    UI_ID UI_Gen_UI_ID();
    vec2 UI_Mouse_Convert_Coordinate(int x, int y);
    b8 UI_Mouse_Rect_Clicked(r32 x, r32 y, r32 width, r32 height);
    bool UI_Do_Rectangle(UI_ID id,
                         r32 x,
                         r32 y,
                         r32 width,
                         r32 height,
                         const vec4& color);
    bool UI_Do_Button(UI_ID id, r32 x, r32 y, r32 width, r32 height, const char* label);
    bool UI_Do_Panel(UI_ID id, r32 x, r32 y, r32 width, r32 height, const char* label);
    bool UI_Do_Text_Label(UI_ID id,
                          r32 x,
                          r32 y,
                          r32 width,
                          const char* label,
                          bool border=false);
    bool UI_Do_Text_Rect(UI_ID id,
                         r32 x,
                         r32 y,
                         r32 width,
                         r32 height,
                         const char* label,
                         bool border=false);
    bool UI_Do_Slider(UI_ID id, r32 x, r32 y, r32 width, r32 sliderValue);

    struct Sync;
    void Do_Debug_Info_Panel(Sync* GameSync, Debug_Info_Panel* panel);
    void Draw_Debug_Info_Panel(Sync* gameSync, Debug_Info_Panel* panel);

}
