// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "TextureCube_OpenGL33.hpp"

namespace GTEngine
{
    TextureCube_OpenGL33::TextureCube_OpenGL33(TextureState_OpenGL33* stateGLIn, Texture2D* positiveX, Texture2D* negativeX, Texture2D* positiveY, Texture2D* negativeY, Texture2D* positiveZ, Texture2D* negativeZ)
        : TextureCube(positiveX, negativeX, positiveY, negativeY, positiveZ, negativeZ), stateGL(stateGLIn)
    {
    }

    TextureCube_OpenGL33::~TextureCube_OpenGL33()
    {
    }


    TextureState_OpenGL33* TextureCube_OpenGL33::GetOpenGLState() const
    {
        return this->stateGL;
    }
}