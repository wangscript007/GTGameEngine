// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "RCDeleteVertexArray_OpenGL21.hpp"
#include <gtgl/gtgl.h>

namespace GTEngine
{
    RCDeleteVertexArray::RCDeleteVertexArray()
        : vertexBufferObject(nullptr), indexBufferObject(nullptr)
    {
    }

    void RCDeleteVertexArray::DeleteVertexArray(GLuint* vertexBufferObjectIn, GLuint* indexBufferObjectIn)
    {
        this->vertexBufferObject = vertexBufferObjectIn;
        this->indexBufferObject  = indexBufferObjectIn;
    }

    void RCDeleteVertexArray::Execute()
    {
        glDeleteBuffers(1, this->vertexBufferObject);
        glDeleteBuffers(1, this->indexBufferObject);
    }
}