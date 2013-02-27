// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Rendering_StencilOperations_hpp_
#define __GTEngine_Rendering_StencilOperations_hpp_

namespace GTEngine
{
    enum StencilOp
    {
        StencilOp_Keep,
        StencilOp_Zero,
        StencilOp_Replace,
        StencilOp_Increment,
        StencilOp_IncrementWrap,
        StencilOp_Decrement,
        StencilOp_DecrementWrap,
        StencilOp_Invert
    };
}

#endif