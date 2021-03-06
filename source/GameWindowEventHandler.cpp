// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTGE/GameWindowEventHandler.hpp>
#include <GTGE/Context.hpp>

namespace GT
{
    GameWindowEventHandler::GameWindowEventHandler(Context &context)
        : context(context)
    {
    }

    void GameWindowEventHandler::OnClose()
    {
        GameEvent e;
        e.code = EventCodes::OnClose;

        this->context.SendEvent(e);
    }

    void GameWindowEventHandler::OnSize(unsigned int width, unsigned int height)
    {
        GameEvent e;
        e.code = EventCodes::OnSize;
        e.size.width  = width;
        e.size.height = height;

        this->context.SendEvent(e);
    }

    void GameWindowEventHandler::OnMouseMove(int x, int y)
    {
        GameEvent e;
        e.code = EventCodes::OnMouseMove;
        e.mousemove.x = x;
        e.mousemove.y = y;

        this->context.SendEvent(e);
    }

    void GameWindowEventHandler::OnMouseWheel(int delta, int x, int y)
    {
        GameEvent e;
        e.code = EventCodes::OnMouseWheel;
        e.mousewheel.delta = delta;
        e.mousewheel.x     = x;
        e.mousewheel.y     = y;

        this->context.SendEvent(e);
    }

    void GameWindowEventHandler::OnMouseButtonDown(MouseButton button, int x, int y)
    {
        GameEvent e;
        e.code = EventCodes::OnMouseButtonDown;
        e.mousedown.button = button;
        e.mousedown.x      = x;
        e.mousedown.y      = y;

        this->context.SendEvent(e);
    }

    void GameWindowEventHandler::OnMouseButtonUp(MouseButton button, int x, int y)
    {
        GameEvent e;
        e.code = EventCodes::OnMouseButtonUp;
        e.mouseup.button = button;
        e.mouseup.x      = x;
        e.mouseup.y      = y;

        this->context.SendEvent(e);
    }

    void GameWindowEventHandler::OnMouseButtonDoubleClick(MouseButton button, int x, int y)
    {
        GameEvent e;
        e.code = EventCodes::OnMouseButtonDoubleClick;
        e.mousedoubleclick.button = button;
        e.mousedoubleclick.x      = x;
        e.mousedoubleclick.y      = y;

        this->context.SendEvent(e);
    }

    void GameWindowEventHandler::OnKeyPressed(Key key)
    {
        GameEvent e;
        e.code           = EventCodes::OnKeyPressed;
        e.keypressed.key = key;

        this->context.SendEvent(e);
    }

    void GameWindowEventHandler::OnKeyReleased(Key key)
    {
        GameEvent e;
        e.code           = EventCodes::OnKeyReleased;
        e.keyreleased.key = key;

        this->context.SendEvent(e);
    }

    void GameWindowEventHandler::OnKeyDown(Key key, unsigned int repeatCount)
    {
        GameEvent e;
        e.code        = EventCodes::OnKeyDown;
        e.keydown.key = key;

        for (unsigned int i = 0; i < repeatCount; ++i)
        {
            this->context.SendEvent(e);
        }
    }

    void GameWindowEventHandler::OnKeyUp(Key key)
    {
        GameEvent e;
        e.code = EventCodes::OnKeyUp;
        e.keyup.key = key;

        this->context.SendEvent(e);
    }

    void GameWindowEventHandler::OnReceiveFocus()
    {
        GameEvent e;
        e.code = EventCodes::OnReceiveFocus;
        
        this->context.SendEvent(e);
    }

    void GameWindowEventHandler::OnLoseFocus()
    {
        GameEvent e;
        e.code = EventCodes::OnLoseFocus;
        
        this->context.SendEvent(e);
    }
}