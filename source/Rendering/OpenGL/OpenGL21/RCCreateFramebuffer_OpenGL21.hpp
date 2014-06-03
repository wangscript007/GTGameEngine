// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Rendering_OpenGL21_RCCreateFramebuffer_hpp_
#define __GTEngine_Rendering_OpenGL21_RCCreateFramebuffer_hpp_

#include "../../RenderCommand.hpp"
#include "FramebufferState_OpenGL21.hpp"

namespace GTEngine
{
    /// Render command for clearing the buffers.
    class RCCreateFramebuffer : public RenderCommand
    {
    public:

        /// Constructor.
        RCCreateFramebuffer();

        /// Destructor.
        virtual ~RCCreateFramebuffer();

        
        /// Renderer::CreateFramebuffer().
        void CreateFramebuffer(FramebufferState_OpenGL21* framebufferStateIn);



        /// RenderCommand::Execute()
        void Execute();


    protected:

        /// glGenFramebuffers.
        virtual void __glGenFramebuffers(GLsizei n, GLuint *framebuffers) = 0;


    private:

        /// A pointer to the program object.
        FramebufferState_OpenGL21* framebufferState;


    private:    // No copying.
        RCCreateFramebuffer(const RCCreateFramebuffer &);
        RCCreateFramebuffer & operator=(const RCCreateFramebuffer &);
    };



    /// EXT_framebuffer_object version of RCCreateFramebuffer.
    class RCCreateFramebufferEXT : public RCCreateFramebuffer
    {
    public:

        /// Constructor.
        RCCreateFramebufferEXT()
            : RCCreateFramebuffer()
        {
        }


    private:

        /// glGenFramebuffers.
        void __glGenFramebuffers(GLsizei n, GLuint *framebuffers);
    };

    /// ARB_framebuffer_object version of RCCreateFramebuffer.
    class RCCreateFramebufferARB : public RCCreateFramebuffer
    {
    public:

        /// Constructor.
        RCCreateFramebufferARB()
            : RCCreateFramebuffer()
        {
        }


    private:

        /// glGenFramebuffers.
        void __glGenFramebuffers(GLsizei n, GLuint *framebuffers);
    };
}

#endif