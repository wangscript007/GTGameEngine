// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "RCSetGlobalState.hpp"
#include <gtgl/gtgl.h>

#include "../CapabilityBits.hpp"

namespace GTEngine
{
    #define VIEWPORT_BIT                    (1 << 1)
    #define SCISSOR_BIT                     (1 << 2)
    #define CLEAR_COLOUR_BIT                (1 << 3)
    #define CLEAR_DEPTH_BIT                 (1 << 4)
    #define CLEAR_STENCIL_BIT               (1 << 5)
    #define SET_CURRENT_SHADER_BIT          (1 << 6)
    #define SET_CURRENT_FRAMEBUFFER_BIT     (1 << 7)
    #define SET_BLEND_FUNCTION_BIT          (1 << 8)
    #define SET_BLEND_EQUATION_BIT          (1 << 9)
    #define SET_ALPHA_FUNCTION_BIT          (1 << 10)
    #define SET_DEPTH_FUNCTION_BIT          (1 << 11)
    #define SET_STENCIL_MASK_BIT            (1 << 12)
    #define SET_STENCIL_FUNC_BIT            (1 << 13)
    #define SET_STENCIL_OP_BIT              (1 << 14)
    #define SET_FACE_CULLING_BIT            (1 << 15)
    #define SET_POLYGON_MODE_BIT            (1 << 16)
    #define SET_POLYGON_OFFSET_BIT          (1 << 17)
    #define SET_DRAW_BUFFERS_BIT            (1 << 18)
    #define ENABLE_BIT                      (1 << 19)
    #define DISABLE_BIT                     (1 << 20)
    


    RCSetGlobalState::RCSetGlobalState()
        : operationBitfield(0),
          viewportParams(), scissorParams(),
          clearColorParams(), clearDepthParams(), clearStencilParams(),
          currentShaderParams(), currentFramebufferParams(),
          blendFuncParams(), blendEquationParams(),
          alphaFuncParams(),
          depthFuncParams(),
          stencilMaskParams(), stencilFuncParams(), stencilOpParams(),
          cullFaceParams(),
          polygonModeParams(), polygonOffsetParams(),
          drawBuffersParams(),
          enableParams(), disableParams()
    {
    }

    void RCSetGlobalState::SetViewport(GLint x, GLint y, GLsizei width, GLsizei height)
    {
        this->viewportParams.x      = x;
        this->viewportParams.y      = y;
        this->viewportParams.width  = width;
        this->viewportParams.height = height;

        this->operationBitfield |= VIEWPORT_BIT;
    }

    void RCSetGlobalState::SetScissor(GLint x, GLint y, GLsizei width, GLsizei height)
    {
        this->scissorParams.x      = x;
        this->scissorParams.y      = y;
        this->scissorParams.width  = width;
        this->scissorParams.height = height;

        this->operationBitfield |= SCISSOR_BIT;
    }

    
    void RCSetGlobalState::SetClearColour(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
    {
        this->clearColorParams.r = r;
        this->clearColorParams.g = g;
        this->clearColorParams.b = b;
        this->clearColorParams.a = a;

        this->operationBitfield |= CLEAR_COLOUR_BIT;
    }

    void RCSetGlobalState::SetClearDepth(GLfloat depth)
    {
        this->clearDepthParams.depth = depth;

        this->operationBitfield |= CLEAR_DEPTH_BIT;
    }

    void RCSetGlobalState::SetClearStencil(GLint stencil)
    {
        this->clearStencilParams.stencil = stencil;

        this->operationBitfield |= CLEAR_STENCIL_BIT;
    }


    void RCSetGlobalState::SetCurrentShader(ShaderState_OpenGL33* programState)
    {
        this->currentShaderParams.programState = programState;

        this->operationBitfield |= SET_CURRENT_SHADER_BIT;
    }

    void RCSetGlobalState::SetCurrentFramebuffer(FramebufferState_OpenGL33* framebufferState)
    {
        this->currentFramebufferParams.framebufferState = framebufferState;

        this->operationBitfield |= SET_CURRENT_FRAMEBUFFER_BIT;
    }


    void RCSetGlobalState::SetBlendFunction(GLenum sfactor, GLenum dfactor)
    {
        this->blendFuncParams.sfactor = sfactor;
        this->blendFuncParams.dfactor = dfactor;

        this->operationBitfield |= SET_BLEND_FUNCTION_BIT;
    }

    void RCSetGlobalState::SetBlendEquation(GLenum mode)
    {
        this->blendEquationParams.mode = mode;

        this->operationBitfield |= SET_BLEND_EQUATION_BIT;
    }

    
    void RCSetGlobalState::SetAlphaTestFunction(GLenum func, GLclampf ref)
    {
        this->alphaFuncParams.func = func;
        this->alphaFuncParams.ref  = ref;

        this->operationBitfield |= SET_ALPHA_FUNCTION_BIT;
    }


    void RCSetGlobalState::SetDepthFunction(GLenum func)
    {
        this->depthFuncParams.func = func;

        this->operationBitfield |= SET_DEPTH_FUNCTION_BIT;
    }


    void RCSetGlobalState::SetStencilMask(GLenum face, GLuint mask)
    {
        this->stencilMaskParams.face = face;
        this->stencilMaskParams.mask = mask;

        this->operationBitfield |= SET_STENCIL_MASK_BIT;
    }

    void RCSetGlobalState::SetStencilFunc(GLenum face, GLenum func, GLint ref, GLuint mask)
    {
        this->stencilFuncParams.face = face;
        this->stencilFuncParams.func = func;
        this->stencilFuncParams.ref  = ref;
        this->stencilFuncParams.mask = mask;

        this->operationBitfield |= SET_STENCIL_FUNC_BIT;
    }

    void RCSetGlobalState::SetStencilOp(GLenum face, GLenum stencilFail, GLenum depthFail, GLenum pass)
    {
        this->stencilOpParams.face        = face;
        this->stencilOpParams.stencilFail = stencilFail;
        this->stencilOpParams.depthFail   = depthFail;
        this->stencilOpParams.pass        = pass;

        this->operationBitfield |= SET_STENCIL_OP_BIT;
    }



    void RCSetGlobalState::SetFaceCulling(GLenum face)
    {
        this->cullFaceParams.face = face;

        this->operationBitfield |= SET_FACE_CULLING_BIT;
    }


    void RCSetGlobalState::SetPolygonMode(GLenum face, GLenum mode)
    {
        this->polygonModeParams.face = face;
        this->polygonModeParams.mode = mode;

        this->operationBitfield |= SET_POLYGON_MODE_BIT;
    }

    void RCSetGlobalState::SetPolygonOffset(GLfloat factor, GLfloat units)
    {
        this->polygonOffsetParams.factor = factor;
        this->polygonOffsetParams.units  = units;

        this->operationBitfield |= SET_POLYGON_OFFSET_BIT;
    }


    void RCSetGlobalState::SetDrawBuffers(size_t count, int* buffers)
    {
        this->drawBuffersParams.buffers.Clear();

        for (size_t i = 0; i < count; ++i)
        {
            this->drawBuffersParams.buffers.PushBack(GL_COLOR_ATTACHMENT0_EXT + buffers[i]);
        }

        this->operationBitfield |= SET_DRAW_BUFFERS_BIT;
    }


    void RCSetGlobalState::Enable(GLenum cap)
    {
        if (cap == GL_SCISSOR_TEST)
        {
            this->enableParams.caps  |=  SCISSOR_TEST_BIT;
            this->disableParams.caps &= ~SCISSOR_TEST_BIT;
        }
        else if (cap == GL_BLEND)
        {
            this->enableParams.caps  |=  BLENDING_BIT;
            this->disableParams.caps &= ~BLENDING_BIT;
        }
        else if (cap == GL_ALPHA_TEST)
        {
            this->enableParams.caps  |=  ALPHA_TEST_BIT;
            this->disableParams.caps &= ~ALPHA_TEST_BIT;
        }
        else if (cap == GL_DEPTH_TEST)
        {
            this->enableParams.caps  |=  DEPTH_TEST_BIT;
            this->disableParams.caps &= ~DEPTH_TEST_BIT;
        }
        else if (cap == GL_POLYGON_OFFSET_FILL)
        {
            this->enableParams.caps  |=  POLYGON_OFFSET_FILL_BIT;
            this->disableParams.caps &= ~POLYGON_OFFSET_FILL_BIT;
        }
        else if (cap == GL_POLYGON_OFFSET_LINE)
        {
            this->enableParams.caps  |=  POLYGON_OFFSET_LINE_BIT;
            this->disableParams.caps &= ~POLYGON_OFFSET_LINE_BIT;
        }
        else if (cap == GL_POLYGON_OFFSET_POINT)
        {
            this->enableParams.caps  |=  POLYGON_OFFSET_POINT_BIT;
            this->disableParams.caps &= ~POLYGON_OFFSET_POINT_BIT;
        }
        else if (cap == GL_CULL_FACE)
        {
            this->enableParams.caps  |=  FACE_CULLING_BIT;
            this->disableParams.caps &= ~FACE_CULLING_BIT;
        }

        this->operationBitfield |= ENABLE_BIT;
    }

    void RCSetGlobalState::Disable(GLenum cap)
    {
        if (cap == GL_SCISSOR_TEST)
        {
            this->disableParams.caps |=  SCISSOR_TEST_BIT;
            this->enableParams.caps  &= ~SCISSOR_TEST_BIT;
        }
        else if (cap == GL_BLEND)
        {
            this->disableParams.caps |=  BLENDING_BIT;
            this->enableParams.caps  &= ~BLENDING_BIT;
        }
        else if (cap == GL_ALPHA_TEST)
        {
            this->disableParams.caps |=  ALPHA_TEST_BIT;
            this->enableParams.caps  &= ~ALPHA_TEST_BIT;
        }
        else if (cap == GL_DEPTH_TEST)
        {
            this->disableParams.caps |=  DEPTH_TEST_BIT;
            this->enableParams.caps  &= ~DEPTH_TEST_BIT;
        }
        else if (cap == GL_POLYGON_OFFSET_FILL)
        {
            this->disableParams.caps |=  POLYGON_OFFSET_FILL_BIT;
            this->enableParams.caps  &= ~POLYGON_OFFSET_FILL_BIT;
        }
        else if (cap == GL_POLYGON_OFFSET_LINE)
        {
            this->disableParams.caps |=  POLYGON_OFFSET_LINE_BIT;
            this->enableParams.caps  &= ~POLYGON_OFFSET_LINE_BIT;
        }
        else if (cap == GL_POLYGON_OFFSET_POINT)
        {
            this->disableParams.caps |=  POLYGON_OFFSET_POINT_BIT;
            this->enableParams.caps  &= ~POLYGON_OFFSET_POINT_BIT;
        }
        else if (cap == GL_CULL_FACE)
        {
            this->disableParams.caps |=  FACE_CULLING_BIT;
            this->enableParams.caps  &= ~FACE_CULLING_BIT;
        }

        this->operationBitfield |= DISABLE_BIT;
    }

    void RCSetGlobalState::EnableColourWrites()
    {
        this->enableParams.caps  |=  COLOUR_WRITES_BIT;
        this->disableParams.caps &= ~COLOUR_WRITES_BIT;
    }

    void RCSetGlobalState::DisableColourWrites()
    {
        this->disableParams.caps |=  COLOUR_WRITES_BIT;
        this->enableParams.caps  &= ~COLOUR_WRITES_BIT;
    }

    void RCSetGlobalState::EnableDepthWrites()
    {
        this->enableParams.caps  |=  DEPTH_WRITES_BIT;
        this->disableParams.caps &= ~DEPTH_WRITES_BIT;
    }

    void RCSetGlobalState::DisableDepthWrites()
    {
        this->disableParams.caps |=  DEPTH_WRITES_BIT;
        this->enableParams.caps  &= ~DEPTH_WRITES_BIT;
    }

    
    void RCSetGlobalState::Execute()
    {
        if ((this->operationBitfield & VIEWPORT_BIT))
        {
            glViewport(this->viewportParams.x, this->viewportParams.y, this->viewportParams.width, this->viewportParams.height);
        }

        if ((this->operationBitfield & SCISSOR_BIT))
        {
            glScissor(this->scissorParams.x, this->scissorParams.y, this->scissorParams.width, this->scissorParams.height);
        }

        if ((this->operationBitfield & CLEAR_COLOUR_BIT))
        {
            glClearColor(this->clearColorParams.r, this->clearColorParams.g, this->clearColorParams.b, this->clearColorParams.a);
        }

        if ((this->operationBitfield & CLEAR_DEPTH_BIT))
        {
            glClearDepth(this->clearDepthParams.depth);
        }

        if ((this->operationBitfield & CLEAR_STENCIL_BIT))
        {
            glClearStencil(this->clearStencilParams.stencil);
        }


        if ((this->operationBitfield & SET_CURRENT_SHADER_BIT))
        {
            // 1) Bind the program.
            glUseProgram(this->currentShaderParams.programState->programObject);

            // 2) Bind textures.
            auto &textures = this->currentShaderParams.programState->textures;
            for (size_t i = 0; i < textures.count; ++i)
            {
                auto &texture = textures.buffer[i]->value;

                glActiveTexture(GL_TEXTURE0 + texture.textureUnit);
                glBindTexture(texture.textureTarget, *texture.textureObject);
            }
        }

        if ((this->operationBitfield & SET_CURRENT_FRAMEBUFFER_BIT))
        {
            if (this->currentFramebufferParams.framebufferState != nullptr)
            {
                glBindFramebuffer(GL_FRAMEBUFFER, this->currentFramebufferParams.framebufferState->framebufferObject);
            }
            else
            {
                glBindFramebuffer(GL_FRAMEBUFFER, 0);

                GLenum backBuffer = GL_BACK;
                glDrawBuffers(1, &backBuffer);
            }
        }


        if ((this->operationBitfield & SET_BLEND_FUNCTION_BIT))
        {
            glBlendFunc(this->blendFuncParams.sfactor, this->blendFuncParams.dfactor);
        }

        if ((this->operationBitfield & SET_BLEND_EQUATION_BIT))
        {
            glBlendEquation(this->blendEquationParams.mode);
        }


        if ((this->operationBitfield & SET_ALPHA_FUNCTION_BIT))
        {
            glAlphaFunc(this->alphaFuncParams.func, this->alphaFuncParams.ref);
        }


        if ((this->operationBitfield & SET_DEPTH_FUNCTION_BIT))
        {
            glDepthFunc(this->depthFuncParams.func);
        }


        if ((this->operationBitfield & SET_STENCIL_MASK_BIT))
        {
            glStencilMaskSeparate(this->stencilMaskParams.face, this->stencilMaskParams.mask);
        }

        if ((this->operationBitfield & SET_STENCIL_FUNC_BIT))
        {
            glStencilFuncSeparate(this->stencilFuncParams.face, this->stencilFuncParams.func, this->stencilFuncParams.ref, this->stencilFuncParams.mask);
        }

        if ((this->operationBitfield & SET_STENCIL_OP_BIT))
        {
            glStencilOpSeparate(this->stencilOpParams.face, this->stencilOpParams.stencilFail, this->stencilOpParams.depthFail, this->stencilOpParams.pass);
        }


        if ((this->operationBitfield & SET_FACE_CULLING_BIT))
        {
            if (this->cullFaceParams.face == GL_NONE)
            {
                glDisable(GL_CULL_FACE);
            }
            else
            {
                glEnable(GL_CULL_FACE);
                glCullFace(this->cullFaceParams.face);
            }
        }


        if ((this->operationBitfield & SET_POLYGON_MODE_BIT))
        {
            glPolygonMode(this->polygonModeParams.face, this->polygonModeParams.mode);
        }

        if ((this->operationBitfield & SET_POLYGON_OFFSET_BIT))
        {
            glPolygonOffset(this->polygonOffsetParams.factor, this->polygonOffsetParams.units);
        }


        if ((this->operationBitfield & SET_DRAW_BUFFERS_BIT))
        {
            glDrawBuffers(static_cast<GLsizei>(this->drawBuffersParams.buffers.count), this->drawBuffersParams.buffers.buffer);
        }


        if ((this->operationBitfield & ENABLE_BIT))
        {
            if ((this->enableParams.caps & SCISSOR_TEST_BIT))
            {
                glEnable(GL_SCISSOR_TEST);
            }
            if ((this->enableParams.caps & BLENDING_BIT))
            {
                glEnable(GL_BLEND);
            }
            if ((this->enableParams.caps & ALPHA_TEST_BIT))
            {
                glEnable(GL_ALPHA_TEST);
            }
            if ((this->enableParams.caps & COLOUR_WRITES_BIT))
            {
                glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
            }
            if ((this->enableParams.caps & DEPTH_WRITES_BIT))
            {
                glDepthMask(GL_TRUE);
            }
            if ((this->enableParams.caps & DEPTH_TEST_BIT))
            {
                glEnable(GL_DEPTH_TEST);
            }
            if ((this->enableParams.caps & POLYGON_OFFSET_FILL_BIT))
            {
                glEnable(GL_POLYGON_OFFSET_FILL);
            }
            if ((this->enableParams.caps & POLYGON_OFFSET_LINE_BIT))
            {
                glEnable(GL_POLYGON_OFFSET_LINE);
            }
            if ((this->enableParams.caps & POLYGON_OFFSET_POINT_BIT))
            {
                glEnable(GL_POLYGON_OFFSET_POINT);
            }
            if ((this->enableParams.caps & FACE_CULLING_BIT))
            {
                glEnable(GL_CULL_FACE);
            }
        }

        if ((this->operationBitfield & DISABLE_BIT))
        {
            if ((this->disableParams.caps & SCISSOR_TEST_BIT))
            {
                glDisable(GL_SCISSOR_TEST);
            }
            if ((this->disableParams.caps & BLENDING_BIT))
            {
                glDisable(GL_BLEND);
            }
            if ((this->disableParams.caps & ALPHA_TEST_BIT))
            {
                glDisable(GL_ALPHA_TEST);
            }
            if ((this->disableParams.caps & COLOUR_WRITES_BIT))
            {
                glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
            }
            if ((this->disableParams.caps & DEPTH_WRITES_BIT))
            {
                glDepthMask(GL_FALSE);
            }
            if ((this->disableParams.caps & DEPTH_TEST_BIT))
            {
                glDisable(GL_DEPTH_TEST);
            }
            if ((this->disableParams.caps & POLYGON_OFFSET_FILL_BIT))
            {
                glDisable(GL_POLYGON_OFFSET_FILL);
            }
            if ((this->disableParams.caps & POLYGON_OFFSET_LINE_BIT))
            {
                glDisable(GL_POLYGON_OFFSET_LINE);
            }
            if ((this->disableParams.caps & POLYGON_OFFSET_POINT_BIT))
            {
                glDisable(GL_POLYGON_OFFSET_POINT);
            }
            if ((this->disableParams.caps & FACE_CULLING_BIT))
            {
                glDisable(GL_CULL_FACE);
            }
        }
    }
}
