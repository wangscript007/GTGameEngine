// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_GameEvent
#define GT_GameEvent

#include "GameEventCodes.hpp"
#include <GTGE/Core/Windowing/Keys.hpp>
#include <GTGE/Core/Windowing/MouseButtons.hpp>

namespace GT
{
    struct GameEvent
    {
        EventCode code;

        union
        {
            struct
            {
            }close;

            struct
            {
                unsigned int width;
                unsigned int height;
            }size;

            struct
            {
                Key key;
            }keypressed, keyreleased;

            struct
            {
                Key key;
            }keydown, keyup;

            struct
            {
                int x;
                int y;
            }mousemove;

            struct
            {
                int delta;
                int x;
                int y;
            }mousewheel;

            struct
            {
                MouseButton button;
                int x;
                int y;
            }mousedown, mouseup, mousedoubleclick;

            struct
            {
            }receivefocus, losefocus;

            struct
            {
                void* data0;
                void* data1;
                void* data2;
                void* data3;
            }custom;
        };
    };
}

#endif