#include "ui.hpp"
#include "sAssert.hpp"
#include "sync.hpp"

namespace SMOBA
{
    extern Sync* GlobalSync;

    UI_ID UI_Gen_UI_ID()
    {
        static UI_ID currentID = 0;
        return currentID++;
    }

    bool Is_Hot(UI_ID id)
    {
        return id == GlobalSync->UiContext->Hot;
    }

    bool Is_Active(UI_ID id)
    {
        return id == GlobalSync->UiContext->Active;
    }

    vec2 UI_Mouse_Convert_Coordinate(int x, int y)
    {
        int iy = -y;
        return vec2(x-(GlobalSync->Viewport->ScreenWidth / 2.0f) ,
                (iy+(GlobalSync->Viewport->ScreenHeight / 2.0f)));
    }

    b8 UI_Mouse_Rect_Clicked(r32 x, r32 y, r32 width, r32 height)
    {
        vec2 mousePos = UI_Mouse_Convert_Coordinate(GlobalSync->Ip->MouseX,
                                      GlobalSync->Ip->MouseY);

        if(mousePos.x > x && mousePos.x < x + width &&
           mousePos.y > y && mousePos.y < y + height && GlobalSync->Ip->LeftMouseDown)
        {
            return true;
        }
        return false;
    }

    void UI_State_Rect_Mouse(UI_ID id, r32 x, r32 y, r32 width, r32 height)
    {
        vec2 mousePos = UI_Mouse_Convert_Coordinate(GlobalSync->Ip->MouseX,
                                                 GlobalSync->Ip->MouseY);

        if(mousePos.x > x && mousePos.x < x + width &&
           mousePos.y > y && mousePos.y < y + height)
        {
            GlobalSync->UiContext->Active = id;
            if(GlobalSync->Ip->LeftMouseDown)
            {
                GlobalSync->UiContext->Hot = id;
            }
        }
        else
        {
            GlobalSync->UiContext->Hot = -1;
            GlobalSync->UiContext->Active = -1;
        }
    }

    bool UI_Do_Rectangle(UI_ID id, r32 x, r32 y, r32 width, r32 height, const vec4& color)
    {
        UI_State_Rect_Mouse(id, x, y-height, width, height);
        bool result = Is_Hot(id);

        // NOTE(matthias): Main body rectangle
        RenderCommand BodyRectangle = {};
        BodyRectangle.RenderType = RECTANGLERENDER;
        BodyRectangle.ShaderType = FILLRECTANGLE;
        BodyRectangle.Texture = 0;
        BodyRectangle.TextureRect = iRect(0, 0, width, height);
        BodyRectangle.Pos = vec3(x, y-height, 0.0f);
        BodyRectangle.Color = color;
        BodyRectangle.Scale = vec3(1.0f, 1.0f, 0.0f);
        BodyRectangle.Rot = 0.0f;

        GlobalSync->Rq->Push(BodyRectangle);

        return result;
    }

    bool UI_Do_Button(UI_ID id, r32 x, r32 y, r32 width, r32 height, const char* label)
    {
        UI_State_Rect_Mouse(id, x, y-height, width, height);

        bool result = Is_Hot(id);

        // NOTE(matthias): outer border rectangle
        const vec4 borderColor = Is_Active(id) ? UIColorDark : UIColorMid;
        UI_Do_Rectangle(id, x, y, width, height, borderColor);

        // NOTE(matthias): inner rectangle
        const vec4 InnerColor = Is_Hot(id) ? UIColorDark : UIColorMid;
        UI_Do_Rectangle(id, x+5, y-5, width-10, height-10, InnerColor);

        // NOTE(matthias): label
        // TODO(matthias): add real text support
        i32 stringlen = strlen(label);
        r32 labelXPos = x + ((width / 2)-(((stringlen*8)/2.0f)));
        r32 labelYPos = y - ((height / 2)-(16/2.0f));
        UI_Do_Text_Label(id, labelXPos, labelYPos, width, label);

        return result;
    }

    bool UI_Do_Panel(UI_ID id, r32 x, r32 y, r32 width, r32 height, const char* label)
    {
        //UI_State_Rect_Mouse(id, x, y-20.f, width, 20);
        bool result;

        // NOTE(matthias): Main body rectangle
        UI_Do_Rectangle(id, x, y, width, height, UIColorMid);

        // NOTE(matthias): titleRectangle rectangle
        result = UI_Do_Rectangle(id, x, y, width, 20, UIColorDark);

        //NOTE(matthias): Button rectangle
        UI_Do_Rectangle(id, x+2, y-2, 16, 16, UIColorLight);

        // NOTE(matthias): label
        // TODO(matthias): add real text support
        i32 stringlen = strlen(label);
        r32 labelXPos = x + ((width / 2)-(((stringlen*8)/2.0f)));
        UI_Do_Text_Label(id, labelXPos, y-2, width-40, label);

        return result;
    }

    bool UI_Do_Text_Label(UI_ID id, r32 x, r32 y, r32 width, const char* label, bool border)
    {
        bool result = true;

        if(border)
        {
            // NOTE(matthias): titleRectangle rectangle
            UI_Do_Rectangle(id, x-5, y+5, width+10, 26, UIColorDark);
        }

        RenderCommand Text = {};
        Text.RenderType = TEXTRECTRENDER;
        Text.ShaderType = NONE;
        Text.Pos = vec3(x, y, 0.0f);
        Text.Scale = vec3(width, 16.0f, 0.0f);
        Text.Color = UIColorTextDark;
        Text.String = label;

        GlobalSync->Rq->Push(Text);
        return result;
    }

    bool UI_Do_Text_Rect(UI_ID id,
                         r32 x,
                         r32 y,
                         r32 width,
                         r32 height,
                         const char*
                         label,
                         bool border)
    {
        bool result = true;

        if(border)
        {
            // NOTE(matthias): titleRectangle rectangle
            UI_Do_Rectangle(id, x+5, y+5, width+10, height+10, UIColorDark);
        }

        RenderCommand Text = {};
        Text.RenderType = TEXTRECTRENDER;
        Text.ShaderType = NONE;
        Text.Pos = vec3(x, y, 0.0f);
        Text.Scale = vec3(width, height, 0.0f);
        Text.Color = UIColorTextDark;
        Text.String = label;

        GlobalSync->Rq->Push(Text);
        return result;
    }

    bool UI_Do_Slider(UI_ID id, r32 x, r32 y, r32 width, r32 sliderValue)
    {
        bool result = true;

        UI_Do_Rectangle(id, x, y, width, 20, UIColorMid);
        UI_Do_Rectangle(id, x+10, y-7, width-20, 6, UIColorLight);
        r32 calcX = 0.0f;
        if(sliderValue < 0.0f)
            calcX = (x+10);
        else if(sliderValue > 1.0f)
            calcX = (x+10) + (width-25);
        else
            calcX = (x+10) + ((width-25)*sliderValue);

        result = UI_Do_Rectangle(id, calcX, y-5, 5, 10, UIColorDark);

        return result;
    }


    void Draw_Debug_Info_Panel(Sync* gameSync, Debug_Info_Panel* panel)
    {
        if(panel->Moving && gameSync->Ip->LeftMouseDown)
        {
            UI_Do_Panel(panel->Panel,
                        panel->Pos.x,
                        panel->Pos.y,
                        panel->Size.x,
                        panel->Size.y,
                        "Debug Info Panel");
        }
        else
        {
            panel->Moving = UI_Do_Panel(panel->Panel,
                                        panel->Pos.x,
                                        panel->Pos.y,
                                        panel->Size.x,
                                        panel->Size.y,
                                        "Debug Info Panel");
        }
        if(!panel->Closed)
        {
            UI_Do_Text_Rect(panel->TextRect,
                            panel->Pos.x+10,
                            panel->Pos.y-40,
                            panel->Size.x-20,
                            panel->Size.y-200,
                            "this is a debug message. Boop");

            UI_Do_Text_Label(-1, panel->Pos.x+10, panel->Pos.y-240, 8*8, "Value: ");

            i32 num = sprintf(panel->Message, "%f", panel->SliderValue);

            UI_Do_Text_Label(-1, panel->Pos.x+10+56, panel->Pos.y-240, 20*8, panel->Message, true);

            if(panel->MovingSlider && gameSync->Ip->LeftMouseDown)
            {
                UI_Do_Slider(panel->Slider,
                             panel->Pos.x+10,
                             panel->Pos.y-200,
                             panel->Size.x-20,
                             panel->SliderValue);
            }
            else
            {
                panel->MovingSlider = UI_Do_Slider(panel->Slider,
                                                   panel->Pos.x+10,
                                                   panel->Pos.y-200,
                                                   panel->Size.x-20,
                                                   panel->SliderValue);
            }
        }
    }

    void Do_Debug_Info_Panel(Sync* GameSync, Debug_Info_Panel* panel)
    {
        if(panel->Moving && GameSync->Ip->LeftMouseDown)
        {
            Draw_Debug_Info_Panel(GameSync ,panel);
            vec2 temp = UI_Mouse_Convert_Coordinate(GameSync->Ip->MouseX,
                                                    GameSync->Ip->MouseY);

            panel->Pos += temp - panel->PreviousMouse;
            panel->PreviousMouse = temp;
        }
        else
        {
            if(panel->MovingSlider && GameSync->Ip->LeftMouseDown)
            {
                vec2 temp = UI_Mouse_Convert_Coordinate(GameSync->Ip->MouseX,
                                                        GameSync->Ip->MouseY);
                r32 pixelStep = 1.0f/(panel->Size.x - 40);
                panel->SliderValue += pixelStep*(temp.x - panel->PreviousMouse.x);
                panel->SliderValue = CLAMP(panel->SliderValue, 0.0f, 1.0f);
                panel->PreviousMouse = temp;
            }
            if(panel->Cooldown == 0)
            {
                if(UI_Mouse_Rect_Clicked(panel->Pos.x,
                                      panel->Pos.y-20,
                                      20.0f,
                                      20.0f))
                {
                    panel->Closed = !panel->Closed;
                    panel->Cooldown = 10;
                }
            }
            else
            {
                panel->Cooldown--;
            }
            panel->Size.y = panel->Closed ? 20.0f : 500.0f;
            Draw_Debug_Info_Panel(GameSync, panel);
            panel->PreviousMouse = UI_Mouse_Convert_Coordinate(GameSync->Ip->MouseX,
                                                           GameSync->Ip->MouseY);
        }
    }
}
