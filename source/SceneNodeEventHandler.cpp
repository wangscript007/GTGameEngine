// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/SceneNodeEventHandler.hpp>

namespace GTEngine
{
    void SceneNodeEventHandler::OnAttach(SceneNode &, SceneNode &)
    {
    }

    void SceneNodeEventHandler::OnDetach(SceneNode &, SceneNode &)
    {
    }

    void SceneNodeEventHandler::OnDestroy(SceneNode &)
    {
    }

    void SceneNodeEventHandler::OnTransform(SceneNode &)
    {
    }

    void SceneNodeEventHandler::OnScale(SceneNode &)
    {
    }

    void SceneNodeEventHandler::OnLayerChanged(SceneNode &, unsigned int)
    {
    }

    void SceneNodeEventHandler::OnSceneChanged(SceneNode &, Scene *)
    {
    }

    void SceneNodeEventHandler::OnStaticChanged(SceneNode &)
    {
    }

    void SceneNodeEventHandler::OnVisibleChanged(SceneNode &)
    {
    }

    void SceneNodeEventHandler::OnUpdate(SceneNode &, double)
    {
    }

    void SceneNodeEventHandler::OnContact(SceneNode &, SceneNode &, const btManifoldPoint &)
    {
    }
}
