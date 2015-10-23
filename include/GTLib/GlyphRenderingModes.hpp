// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_RenderingModes_hpp_
#define __GTLib_RenderingModes_hpp_

namespace GTLib
{
    /**
    *   \brief  Enumerator for each rendering mode.
    */
    enum RenderingMode
    {
        RenderingMode_Normal = 0x1,
        RenderingMode_LCD    = 0x2,
        RenderingMode_X2     = 0x4    // Renders the font twice on top of each other. Can look better in some situations. Bitwise this with Normal or LCD. Only used with bitmap fonts.
    };
    
}


#endif 
