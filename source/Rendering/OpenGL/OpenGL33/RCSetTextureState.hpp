// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Rendering_OpenGL33_RCSetTextureState_hpp_
#define __GTEngine_Rendering_OpenGL33_RCSetTextureState_hpp_

#include <GTEngine/Rendering/RenderCommand.hpp>
#include <GTImage/ImageFormats.hpp>
#include <GTCore/Map.hpp>
#include <gtgl/gtgl.h>
#include <cstdint>

#include "ShaderState_OpenGL33.hpp"

namespace GTEngine
{
    /// Render command for clearing the buffers/
    class RCSetTextureState : public RenderCommand
    {
    public:

        /// Constructor.
        RCSetTextureState();


        /// Renderer::PushTexture1DData().
        void SetTexture1DData(GLuint* textureObject, GLenum target, int mipmap, GTImage::ImageFormat format, unsigned int width, const void* data, size_t dataSizeInBytes);

        /// Renderer::PushTexture2DData().
        void SetTexture2DData(GLuint* textureObject, GLenum target, int mipmap, GTImage::ImageFormat format, unsigned int width, unsigned int height, const void* data, size_t dataSizeInBytes);

        /// Renderer::PushTexture3DData().
        void SetTexture3DData(GLuint* textureObject, GLenum target, int mipmap, GTImage::ImageFormat format, unsigned int width, unsigned int height, unsigned int depth, const void* data, size_t dataSizeInBytes);


        /// Renderer::SetTexture2DFilter().
        void SetTexture2DFilter(GLuint* textureObject, GLenum target, GLint minification, GLint magnification);

        /// Renderer::SetTexture2DAnisotropy().
        void SetTexture2DAnisotropy(GLuint* textureObject, GLenum target, GLint anisotropy);

        /// Renderer::SetTexture2DWrapMode().
        void SetTexture2DWrapMode(GLuint* textureObject, GLenum target, GLint wrapMode);

        /// Renderer::SetTexture2DMipmapLevels().
        void SetTexture2DMipmapLevels(GLuint* textureObject, GLenum target, GLint baseLevel, GLint maxLevel);

        /// Renderer::SetTexture2DGenerateMipmaps().
        void GenerateTexture2DMipmaps(GLuint* textureObject, GLenum target);



        /// Retrieves the texture object whose state is being set.
        GLuint* GetTextureObject();

        /// Retrieves the target (texture type) of the state being set.
        GLenum GetTarget();


        /// Sets the state of the current shader.
        void SetCurrentProgramState(ShaderState_OpenGL33* currentProgramState);


        /// RenderCommand::Execute()
        void Execute();


    private:

        /// A bitfield containing information about the operations to perform.
        uint32_t operationBitfield;


        /// The texture object whose state is being modified.
        GLuint* textureObject;

        /// The texture target.
        GLenum target;


        /// The vertex data to set.
        struct TextureData
        {
            GLint   internalFormat;
            GLenum  format;
            GLenum  type;
            GLsizei width;
            GLsizei height;
            GLsizei depth;
            GLvoid* data;
        };

        /// The map of TextureData objects for each mipmap whose data is getting set. The key is the mipmap level.
        GTCore::Map<GLint, TextureData> mipmapDatas;



        /// The new filters.
        struct
        {
            GLint minification;
            GLint magnification;

        }filters;

        /// The new anisotropy.
        GLint anisotropy;

        /// The new wrapping mode.
        GLint wrapMode;

        /// The new mipmap levels.
        struct
        {
            GLint baseLevel;
            GLint maxLevel;

        }mipmapLevels;



        /// A pointer to the state of the current shader.
        ShaderState_OpenGL33* currentProgramState;



    private:    // No copying.
        RCSetTextureState(const RCSetTextureState &);
        RCSetTextureState & operator=(const RCSetTextureState &);
    };
}

#endif
