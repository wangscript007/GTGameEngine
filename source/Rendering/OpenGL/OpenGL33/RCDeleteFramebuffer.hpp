// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Rendering_OpenGL33_RCDeleteFramebuffer_hpp_
#define __GTEngine_Rendering_OpenGL33_RCDeleteFramebuffer_hpp_

#include <GTEngine/Rendering/RenderCommand.hpp>
#include <gtgl/gtgl.h>

namespace GTEngine
{
    /// Render command for clearing the buffers/
    class RCDeleteFramebuffer : public RenderCommand
    {
    public:

        /// Constructor.
        RCDeleteFramebuffer();


        /// Renderer::DeleteFramebuffer()
        void DeleteFramebuffer(GLuint* framebufferObjectIn);


        /// RenderCommand::Execute()
        void Execute();


    private:

        /// A pointer to the OpenGL shader program object.
        GLuint* framebufferObject;



    private:    // No copying.
        RCDeleteFramebuffer(const RCDeleteFramebuffer &);
        RCDeleteFramebuffer & operator=(const RCDeleteFramebuffer &);
    };
}

#endif
