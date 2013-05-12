// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_DefaultSceneRendererMesh_hpp_
#define __GTEngine_DefaultSceneRendererMesh_hpp_

#include "../SceneRenderer.hpp"
#include "DefaultSceneRenderer_LightGroup.hpp"

namespace GTEngine
{
    /// Structure representing a mesh in the default renderer.
    struct DefaultSceneRendererMesh : public SceneRendererMesh
    {
        /// The indices of the lights that are touching this mesh.
        const DefaultSceneRenderer_LightGroup* touchingLights;

        /// The light groups that affect this mesh.
        GTCore::Vector<DefaultSceneRenderer_LightGroup> lightGroups;

        /// The highlight colour.
        glm::vec3 highlightColour;



        /// Constructor.
        DefaultSceneRendererMesh()
            : touchingLights(nullptr), lightGroups(),
              highlightColour(1.0f, 1.0f, 1.0f)
        {
        }

        /// Copy constructor.
        DefaultSceneRendererMesh(const DefaultSceneRendererMesh &other)
            : SceneRendererMesh(other), touchingLights(other.touchingLights), lightGroups(other.lightGroups),
              highlightColour(other.highlightColour)
        {
        }

        /// Destructor.
        ~DefaultSceneRendererMesh()
        {
            this->ClearLightGroups();
        }


        /// Clears the light groups.
        void ClearLightGroups()
        {
            this->lightGroups.Clear();
        }


    private:    // No copying.
        DefaultSceneRendererMesh & operator=(const DefaultSceneRendererMesh &);
    };
}

#endif