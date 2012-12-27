
#ifndef __GTEngine_ModelComponent_hpp_
#define __GTEngine_ModelComponent_hpp_

#include "../Component.hpp"
#include "../Model.hpp"

namespace GTEngine
{
    GTENGINE_DECL_COMPONENT_START(ModelComponent)
    public:

        ModelComponent(SceneNode &node);
        ~ModelComponent();


        /// Sets the model.
        void SetModel(Model* model, bool takeOwnership = false);

        /// Sets the model from a file.
        ///
        /// @param fileName [in] The name of the model file to load.
        ///
        /// @return A pointer to the model that was loaded by the component.
        ///
        /// @remarks
        ///     This will load the model via the model library and will take ownership, which means it will be deleted whenever the
        ///     component no longer uses it (either when it's change, or when the component is destructed).
        Model* SetModel(const char* fileName);

        /// Unsets the current model.
        void UnsetModel();


        /// Retrieves the model currently associated with this component.
              Model* GetModel()       { return this->model; }
        const Model* GetModel() const { return this->model; }


        /// Sets the back/front face culling mode. By default, back faces are culled.
        ///
        /// @param cullFront [in] Specifies whether or not front faces should be drawn.
        /// @param cullBack  [in] Specifies whether or not back faces should be drawn.
        void SetFaceCulling(bool cullFront, bool cullBack);

        /// Determines whether or not front faces should be culled.
        bool IsCullingFrontFaces() const { return (this->flags & CullFrontFaces) != 0; }

        /// Determines whether or not back faces should be culled.
        bool IsCullingBackFaces() const { return (this->flags & CullBackFaces) != 0; }

        
        /// Enables shadow casting.
        ///
        /// @remarks
        ///     Shadow casting is enabled by default.
        void EnableShadowCasting();

        /// Disables shadow casting.
        ///
        /// @remarks
        ///     Shadow casting is enabled by default.
        void DisableShadowCasting();

        /// Determines whether or not this model casts shadows.
        bool IsShadowCastingEnabled() const { return (this->flags & CastShadow) != 0; }


        /// Shows the model.
        ///
        /// @remarks
        ///     This is NOT the same as SceneNode::Show(). The scene node's visibility state will not be changed.
        void ShowModel() { this->flags |= Visible; }

        /// Hides the model.
        ///
        /// @remarks
        ///     This is NOT the same as SceneNode::Hide(). The scene node's visibility state will not be changed, and instead only the model will be hidden.
        void HideModel() { this->flags &= ~Visible; }

        /// Determines whether or not the model is visible.
        bool IsModelVisible() const { return (this->flags & Visible) != 0; }




        /// Enables drawing of the wireframe.
        void EnableWireframe() { this->drawWireframe = true; }
        
        /// Disables drawing of the wireframe.
        void DisableWireframe() { this->drawWireframe = false; }

        /// Determines whether or not the wireframe should be drawn.
        bool IsWireframeEnabled() const { return this->drawWireframe; }


        /// Sets the colour of the wireframe.
        void SetWireframeColour(float r, float g, float b) { this->wireframeColour = glm::vec3(r, g, b); }

        /// Retrieves the colour of the wireframe.
        const glm::vec3 & GetWireframeColour() const { return this->wireframeColour; }



    private:

        /// A pointer to the applicable model.
        Model *model;


        enum Flags
        {
            CullFrontFaces = (1 << 0),
            CullBackFaces  = (1 << 1),
            CastShadow     = (1 << 2),
            Visible        = (1 << 3),
            Owner          = (1 << 4),
        };

        /// The flags for this model.
        uint32_t flags;


        // TODO: Should move the rendering related attributes to a different component later on.
        
        /// The colour to draw the wireframe.
        glm::vec3 wireframeColour;

        /// Whether or not to draw the wireframe.
        bool drawWireframe;


    GTENGINE_DECL_COMPONENT_END()
}

#endif
