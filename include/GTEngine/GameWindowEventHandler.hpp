
#ifndef __GTEngine_GameWindowEventHandler_hpp_
#define __GTEngine_GameWindowEventHandler_hpp_

#include <GTCore/Window.hpp>

namespace GTEngine
{
    class Game;

    class GameWindowEventHandler : public GTCore::WindowEventHandler
    {
    public:

        void OnClose();
        void OnSize(unsigned int width, unsigned int height);
        
        void OnPaint(int left, int top, int right, int bottom)
        {
            (void)left;
            (void)top;
            (void)right;
            (void)bottom;
        }
        
        void OnUpdate(double dt)
        {
            (void)dt;
        }
        
        void OnMouseMove(int x, int y);
        void OnMouseWheel(int delta, int x, int y);
        void OnMouseButtonDown(GTCore::MouseButton button, int x, int y);
        void OnMouseButtonUp(GTCore::MouseButton button, int x, int y);
        void OnMouseButtonDoubleClick(GTCore::MouseButton button, int x, int y);

        void OnKeyPressed(GTCore::Key key);
        void OnKeyReleased(GTCore::Key key);
        void OnKeyDown(GTCore::Key key, unsigned int repeatCount);
        void OnKeyUp(GTCore::Key key);
        
        
        void OnReceiveFocus()   // OnFocus
        {
        }
        
        void OnLoseFocus()      // OnBlur
        {
        }


    public:

        /// Constructor.
        GameWindowEventHandler(Game &game);


    private:

        /// The game whose window this event handler is attached to.
        Game &game;


    private:    // No copying.
        GameWindowEventHandler(const GameWindowEventHandler &);
        GameWindowEventHandler & operator=(const GameWindowEventHandler &);

    };
}

#endif