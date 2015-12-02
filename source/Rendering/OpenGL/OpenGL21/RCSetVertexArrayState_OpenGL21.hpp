// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_Rendering_OpenGL21_RCSetVertexArrayState
#define GT_Rendering_OpenGL21_RCSetVertexArrayState

#include "../../RenderCommand.hpp"
#include <GTGE/Rendering/VertexFormat.hpp>
#include <gtgl/gtgl.h>
#include <cstdint>

namespace GT
{
    /// Render command for clearing the buffers/
    class RCSetVertexArrayState : public RenderCommand
    {
    public:

        /// Constructor.
        RCSetVertexArrayState();

        /// Renderer::SetVertexArrayVertexData()
        void SetVertexData(GLuint* vertexBufferObject, const float* vertices, size_t vertexCount, size_t vertexSizeInBytes, GLenum usage);

        /// Renderer::SetVertexArrayIndexData()
        void SetIndexData(GLuint* indexBufferObject, const unsigned int* indices, size_t indexCount, GLenum usage);


        /// RenderCommand::Execute()
        void Execute();


    private:

        /// A bitfield containing information about the operations to perform.
        uint32_t operationBitfield;


        /// The vertex buffer object that would usually be bound to GL_ARRAY_BUFFER.
        GLuint* vertexBufferObject;

        /// The vertex buffer object that would usually be bound to GL_ELEMENT_ARRAY_BUFFER which contains the vertex array indices.
        GLuint* indexBufferObject;


        /// The vertex data to set.
        struct
        {
            float* vertices;
            size_t count;
            size_t vertexSizeInBytes;
            GLenum usage;

        }vertexData;

        /// The index data to set.
        struct
        {
            unsigned int* indices;
            size_t        count;
            GLenum        usage;

        }indexData;


    private:    // No copying.
        RCSetVertexArrayState(const RCSetVertexArrayState &);
        RCSetVertexArrayState & operator=(const RCSetVertexArrayState &);
    };
}

#endif
