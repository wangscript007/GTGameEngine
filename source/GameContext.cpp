// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/EngineContext.hpp>
#include <GTGameEngine/GameContext.hpp>
#include <GTGameEngine/GameDisplay_Windowed.hpp>

namespace GT
{
    namespace GE
    {
        GameContext::GameContext(EngineContext &engineContext)
            : m_engineContext(engineContext),
              m_windowedDisplays(),
              m_flags()
        {
            m_flags |= IsSingleThreaded;    // TODO: Remove this flag so that multi-threading is used by default.
        }

        GameContext::~GameContext()
        {
        }


        ResultCode GameContext::Startup()
        {
            return 0;   // No error.
        }

        void GameContext::Shutdown()
        {
        }



        void GameContext::Step()
        {
            // The way the game is stepped depends on whether or not multithreading is being used. In some cases it is
            // desireable to not multi-thread things, such as when single-threading is efficient enough that it becomes
            // faster.
            //
            // In a single threaded environment it is very simple - we simply do an update tick, and then issue the
            // rendering commands directly.
            //
            // In a multi-threaded environment, we want to buffer the rendering commands, and then execute them on the
            // following frame.
            //
            // Because the multi-threaded implementation is less efficient we do not want to use a buffering system for
            // the single-threaded case. We do, however, need a consistent API for both since we need to allow custom
            // rendering for displays.

            if ((m_flags & IsSingleThreaded) == 0)
            {
                // Multi-threaded pipeline.
            }
            else
            {
                // Single-threaded pipeline.

                // We need to iterate over each display and draw it.
                for (size_t iDisplay = 0; iDisplay < m_windowedDisplays.GetCount(); ++iDisplay)
                {
                    auto display = m_windowedDisplays[iDisplay];
                    assert(display != nullptr);
                    {
                    
                    }
                }
            }
        }

        




        //////////////////////////////////////////////////////////////////
        // Win32
#if defined(GT_PLATFORM_WINDOWS)
        ResultCode GameContext::CreateWindowedDisplay(GPURenderingDevice &renderingDevice, HWND hWnd, GameDisplay* &displayOut)
        {
            if (hWnd != 0)
            {
                displayOut = new GameDisplay_Windowed(renderingDevice, hWnd);

                return 0;   // No error.
            }
            else
            {
                return InvalidWindow;
            }
        }

        void GameContext::DeleteDisplay(GameDisplay* display)
        {
            delete display;
        }
#endif

    }
}