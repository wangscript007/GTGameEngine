// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Rendering_OpenGL33_RCCreateFramebuffer_hpp_
#define __GTEngine_Rendering_OpenGL33_RCCreateFramebuffer_hpp_

#include <GTEngine/Rendering/RenderCommand.hpp>
#include <gtgl/gtgl.h>

namespace GTEngine
{
    /// Render command for clearing the buffers.
    class RCCreateFramebuffer : public RenderCommand
    {
    public:

        /// Constructor.
        RCCreateFramebuffer();

        
        /// Renderer::CreateFramebuffer().
        void CreateFramebuffer(GLuint* framebufferObjectIn);



        /// RenderCommand::Execute()
        void Execute();



    private:

        /// A pointer to the program object.
        GLuint* framebufferObject;


    private:    // No copying.
        RCCreateFramebuffer(const RCCreateFramebuffer &);
        RCCreateFramebuffer & operator=(const RCCreateFramebuffer &);
    };
}

#endif
