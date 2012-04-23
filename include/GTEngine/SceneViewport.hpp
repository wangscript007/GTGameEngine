
#ifndef __GTEngine_SceneViewport_hpp_
#define __GTEngine_SceneViewport_hpp_

#include <GTCore/Vector.hpp>
#include "Math.hpp"

namespace GTEngine
{
    class Scene;
    class SceneNode;
    class ViewportRenderer;
    class Framebuffer;
    class Texture2D;

    /// Base class representing a viewport that a scene renders to.
    ///
    /// The scene the viewport is attached to can be changed dynamically. Do NOT use SceneViewport::SetScene(). Instead,
    /// attach/detach the viewport with Scene::AttachViewport()/Scene::DetachViewport().
    ///
    /// In order to render the viewport, a ViewportRenderer must be attached. Use SetRenderer() to set the renderer to
    /// use when rendering the viewport.
    class SceneViewport
    {
    public:

        /// Constructor.
        SceneViewport(unsigned int width, unsigned int height);

        /// Destructor.
        virtual ~SceneViewport();



    public:

        /// Sets the scene this viewport is attached to. Do not use this publically. Instead, use Scene::AttachViewport()/Scene::DetachViewport().
        /// @param scene [in] A pointer to the new scene. Can be null, it which case the viewport is not attached to any scene.
        void SetScene(Scene *scene);

        /// Retrieves a pointer to the scene this viewport is attached to. This can return null if it is not attached to any scene.
        Scene* GetScene();


        /// Sets the scene node to act as the camera.
        /// @param cameraNode [in] A pointer to the camera node. Can be null, in which case nothing is rendered.
        void SetCameraNode(SceneNode* cameraNode);

        /// Retrieves a pointer to the camera node. Can return null if a camera hasn't been set, or SetCameraNode() was set with an argument of null.
        SceneNode* GetCameraNode();

        
        /// Sets the renderer.
        void SetRenderer(ViewportRenderer* renderer);

        /// Retrieves a pointer to the renderer.
        ViewportRenderer* GetRenderer();


        /// Retrieves the width of the viewport.
        /// @return The width of the viewport.
        unsigned int GetWidth() const;

        /// Retrieves the height of the viewport.
        /// @return The height of the viewport.
        unsigned int GetHeight() const;

        /// Resizes the viewport.
        void Resize(unsigned int newWidth, unsigned int newHeight);


        /// Retrieves a reference to the cache of model nodes.
        GTCore::Vector<SceneNode*> & GetModelNodes();

        /// Retrieves a reference to the cache of ambient light nodes.
        GTCore::Vector<SceneNode*> & GetAmbientLightNodes();

        /// Retrieves a reference to the cache of directional light nodes.
        GTCore::Vector<SceneNode*> & GetDirectionalLightNodes();

        /// Retrieves a reference to the cache of point light nodes.
        GTCore::Vector<SceneNode*> & GetPointLightNodes();


        /// Clears the cache of visible nodes. This also clears the lighting nodes.
        void ClearVisibleNodes();

        // TODO: Consider replacing this will specific versions for the sake of efficiency.
        /// Adds a node to the internal cache of visible nodes.
        /// @param node [in] A reference to the node being added.
        ///
        /// @remarks.
        ///     This method should be called for lighting nodes, too. Lighting nodes will filtered into their own caches which
        ///     can be retrieved with the Get*Nodes() methods.
        void AddVisibleNode(SceneNode &node);


        /// Draws the content of the viewport using the attached renderer. If no renderer is attached, this will do nothing.
        void Render();

        /// Retrieves the framebuffer that the renderer is drawing to. An application will need to retrieve this in order to
        /// show the rendering result. This will return null if there is no renderer.
        Framebuffer* GetFramebuffer();


        /// Retrieves the final colour output buffer. Can return null if there is no renderer.
        Texture2D* GetColourOutputBuffer();

        /// Retrieves the final depth/stencil output buffer. Can return null if there is no renderer.
        Texture2D* GetDepthStencilOutputBuffer();


    // Picking.
    public:

        /// Calculates a ray that can be used for picking.
        /// @param  x       [in]  The x position on the viewport the ray should start at.
        /// @param  y       [in]  The y position on the viewport the ray should start at.
        /// @param  rayNear [out] The end of the ray that is closest to the viewer.
        /// @param  rayFar  [out] The end of the ray that is farthest from the viewport.
        void CalculatePickingRay(int x, int y, glm::vec3 &rayNear, glm::vec3 &rayFar);

        /// Picks a scene node using the given coordinates on the viewport.
        /// @param  x [in] The x position on the viewport the picking ray should start at.
        /// @param  y [in] The y position on the viewport the picking ray should start at.
        /// @return A pointer to the scene node that was picked, or null if nothing was picked.
        ///
        /// @remarks
        ///     The picking is performed based on the collision volumes of the visual representation of the scene node.
        SceneNode* PickSceneNode(int x, int y);


    // Misc stuff.
    public:

        /// Projects a 3D point to window coordinates based on the viewport.
        /// @param position [in] The point in 3D space being projected.
        glm::vec3 Project(const glm::vec3 &position);

        /// Retrieves a 2D projection matrix for this viewport.
        ///
        /// @param yDown [in] Whether or not the +y axis should move from top to bottom. Defaults to false (+y up).
        ///
        /// @return A new 2D projection matrix for this viewport.
        ///
        /// @remarks
        ///     This method does a full construction of the matrix. Do not assume it does a direct access.
        glm::mat4 Get2DProjectionMatrix(bool yDown = false);


    private:

        /// The scene this viewport is attached to.
        Scene* scene;

        /// A pointer to the scene node acting as the camera. Must have a CameraComponent attached.
        SceneNode* cameraNode;

        /// The renderer to use when drawing this viewport.
        ViewportRenderer* renderer;

        /// The width of the viewport.
        unsigned int width;

        /// The height of the viewport.
        unsigned int height;


        /// The cache of model scene nodes.
        GTCore::Vector<SceneNode*> modelNodes;

        /// The cache of ambient light nodes.
        GTCore::Vector<SceneNode*> ambientLightNodes;

        /// The cache of directional light nodes.
        GTCore::Vector<SceneNode*> directionalLightNodes;

        /// The cache of point light nodes.
        GTCore::Vector<SceneNode*> pointLightNodes;


    private: // No copying.
        SceneViewport(const SceneViewport &);
        SceneViewport & operator=(const SceneViewport &);
    };
}

#endif