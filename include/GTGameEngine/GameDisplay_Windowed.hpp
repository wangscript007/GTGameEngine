// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_GameDisplay_Windowed_hpp_
#define __GT_GE_GameDisplay_Windowed_hpp_

#include "Config.hpp"
#include "GameDisplay.hpp"

#if defined(GT_PLATFORM_WINDOWS)
#include <GTLib/windef.h>
#endif

namespace GT
{
    /// Class representing a windowed game display.
    class GameDisplay_Windowed : public GameDisplay
    {
    public:

        /// Destructor.
        virtual ~GameDisplay_Windowed();


        /// @copydoc GameDisplay::GetType()
        GameDisplayType GetType() const;

        /// @copydoc GameDisplay::GetSize(unsigned int &, unsigned int &)
        void GetSize(unsigned int &widthOut, unsigned int &heightOut) const;



        ///////////////////////////////////////
        // Platform-specific

#if defined(GT_PLATFORM_WINDOWS)
        /// Constructor.
        GameDisplay_Windowed(GraphicsInterface &graphicsInterface, HWND hWnd);

        /// Retrieves a handle to the window this display is drawing to.
        ///
        /// @return A handle to the window the display is drawing to.
        HWND GetWindow() const;
#endif
            


    private:

#if defined(GT_PLATFORM_WINDOWS)
        /// A handle to the window we are drawing to.
        HWND m_hWnd;
#endif


    private:    // No copying.
        GameDisplay_Windowed(const GameDisplay_Windowed &);
        GameDisplay_Windowed & operator=(const GameDisplay_Windowed &);
    };
}

#endif