// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Scene/SceneNodeComponentDescriptor_Graphics.hpp>

namespace GT
{
    SceneNodeComponentTypeID SceneNodeComponentDescriptor_Graphics::TypeID()
    {
        return SceneNodeComponentType_Graphics;
    }

    const char* SceneNodeComponentDescriptor_Graphics::Name()
    {
        return "Graphics";
    }



    SceneNodeComponentDescriptor_Graphics::SceneNodeComponentDescriptor_Graphics()
    {
    }

    SceneNodeComponentDescriptor_Graphics::~SceneNodeComponentDescriptor_Graphics()
    {
    }

    SceneNodeComponentTypeID SceneNodeComponentDescriptor_Graphics::GetTypeID() const
    {
        return SceneNodeComponentDescriptor_Graphics::TypeID();
    }

    const char* SceneNodeComponentDescriptor_Graphics::GetName() const
    {
        return SceneNodeComponentDescriptor_Graphics::Name();
    }
}