// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_DefaultSceneRenderer_hpp_
#define __GTEngine_DefaultSceneRenderer_hpp_

#include "../SceneRenderer.hpp"
#include "../Scene.hpp"
#include "../MaterialLibrary.hpp"
#include "../Rendering/Renderer.hpp"

#include "DefaultSceneRenderer_VisibilityProcessor.hpp"


#include <GTCore/Map.hpp>
#include <utility>

namespace GTEngine
{
    /// Structure containing information about a framebuffer that is attached to each viewport in the default scene renderer.
    ///
    /// One of these framebuffers is created for every viewport.
    struct DefaultSceneRendererFramebuffer
    {
        /// A pointer to main framebuffer object.
        Framebuffer* framebuffer;


        /// The depth/stencil buffer.
        Texture2D* depthStencilBuffer;

        /// The main colour buffer (RGBA16F). This is the HDR buffer.
        Texture2D* opaqueColourBuffer;

        /// The final HDR colour buffer. RGBA16F.
        Texture2D* finalColourBufferHDR;




        /// The first lighting accumulation buffer. RGBA16F. Stores diffuse RGB. A is unused right now.
        Texture2D* lightingBuffer0;

        /// The second light accumulation buffer. RGBA16F. Stores specular RGB. A is unused right now.
        Texture2D* lightingBuffer1;


        /// The final colour output buffer. This is the LDR buffer that will have the final image applied. RGBA8.
        Texture2D* finalColourBuffer;


        /// A pointer to the framebuffer object to use for bloom. We use a separate object so we can do a half-sized bloom map for efficiency.
        Framebuffer* bloomFramebuffer;

        /// The bloom colour buffer. RGBA16F.
        Texture2D* bloomBuffer;

        /// The intermediary buffer to use for blurring the bloom buffer.
        Texture2D* bloomBlurBuffer;


        /// The width of the framebuffer.
        unsigned int width;

        /// The height of the framebuffer.
        unsigned int height;



        /// Constructor
        DefaultSceneRendererFramebuffer(unsigned int widthIn, unsigned int heightIn)
            : framebuffer(nullptr),
              depthStencilBuffer(nullptr), opaqueColourBuffer(nullptr), finalColourBufferHDR(nullptr),
              lightingBuffer0(nullptr), lightingBuffer1(nullptr),
              finalColourBuffer(nullptr),
              bloomFramebuffer(nullptr), bloomBuffer(nullptr), bloomBlurBuffer(nullptr),
              width(widthIn), height(heightIn)
        {
            this->framebuffer          = Renderer::CreateFramebuffer();
            this->depthStencilBuffer   = Renderer::CreateTexture2D();
            this->opaqueColourBuffer   = Renderer::CreateTexture2D();
            this->lightingBuffer0      = Renderer::CreateTexture2D();
            this->lightingBuffer1      = Renderer::CreateTexture2D();
            this->finalColourBufferHDR = Renderer::CreateTexture2D();
            this->finalColourBuffer    = Renderer::CreateTexture2D();

            this->bloomFramebuffer     = Renderer::CreateFramebuffer();
            this->bloomBuffer          = Renderer::CreateTexture2D();
            this->bloomBlurBuffer      = Renderer::CreateTexture2D();


            // Sizes and formats need to be set. All we need to do is call the Resize() method.
            this->Resize(width, height);


            // Filters.
            Renderer::SetTexture2DFilter(*this->opaqueColourBuffer,    TextureFilter_NearestNearest, TextureFilter_Nearest);
            Renderer::SetTexture2DFilter(*this->lightingBuffer0,       TextureFilter_Nearest,        TextureFilter_Nearest);
            Renderer::SetTexture2DFilter(*this->lightingBuffer1,       TextureFilter_Nearest,        TextureFilter_Nearest);
            Renderer::SetTexture2DFilter(*this->finalColourBufferHDR,  TextureFilter_NearestNearest, TextureFilter_Nearest);
            Renderer::SetTexture2DFilter(*this->finalColourBuffer,     TextureFilter_Nearest,        TextureFilter_Nearest);
            Renderer::SetTexture2DFilter(*this->bloomBuffer,           TextureFilter_Linear,         TextureFilter_Linear);
            Renderer::SetTexture2DFilter(*this->bloomBlurBuffer,       TextureFilter_Nearest,        TextureFilter_Nearest);

            // Wrap Modes.
            Renderer::SetTexture2DWrapMode(*this->opaqueColourBuffer, TextureWrapMode_ClampToEdge);
            Renderer::SetTexture2DWrapMode(*this->bloomBuffer,        TextureWrapMode_ClampToEdge);
            Renderer::SetTexture2DWrapMode(*this->bloomBlurBuffer,    TextureWrapMode_ClampToEdge);


            // Attach to the main framebuffer.
            this->framebuffer->AttachDepthStencilBuffer(this->depthStencilBuffer);
            this->framebuffer->AttachColourBuffer(this->opaqueColourBuffer,   0);
            this->framebuffer->AttachColourBuffer(this->lightingBuffer0,      1);
            this->framebuffer->AttachColourBuffer(this->lightingBuffer1,      2);
            this->framebuffer->AttachColourBuffer(this->finalColourBufferHDR, 3);
            this->framebuffer->AttachColourBuffer(this->finalColourBuffer,    4);
            Renderer::PushAttachments(*this->framebuffer);

            this->bloomFramebuffer->AttachColourBuffer(this->bloomBuffer,     0);
            this->bloomFramebuffer->AttachColourBuffer(this->bloomBlurBuffer, 1);
            Renderer::PushAttachments(*this->bloomFramebuffer);
        }

        /// Destructor
        ~DefaultSceneRendererFramebuffer()
        {
            Renderer::DeleteTexture2D(this->depthStencilBuffer);
            Renderer::DeleteTexture2D(this->opaqueColourBuffer);
            Renderer::DeleteTexture2D(this->lightingBuffer0);
            Renderer::DeleteTexture2D(this->lightingBuffer1);
            Renderer::DeleteTexture2D(this->finalColourBufferHDR);
            Renderer::DeleteTexture2D(this->finalColourBuffer);
            Renderer::DeleteFramebuffer(this->framebuffer);

            Renderer::DeleteTexture2D(this->bloomBlurBuffer);
            Renderer::DeleteTexture2D(this->bloomBuffer);
            Renderer::DeleteFramebuffer(this->bloomFramebuffer);
        }


        /// Resizes all of the attachments on the framebuffer.
        void Resize(unsigned int newWidth, unsigned int newHeight)
        {
            this->width  = newWidth;
            this->height = newHeight;


            this->depthStencilBuffer->SetData(  newWidth, newHeight, GTImage::ImageFormat_Depth24_Stencil8);
            this->opaqueColourBuffer->SetData(  newWidth, newHeight, GTImage::ImageFormat_RGB16F);
            this->lightingBuffer0->SetData(     newWidth, newHeight, GTImage::ImageFormat_RGB16F);
            this->lightingBuffer1->SetData(     newWidth, newHeight, GTImage::ImageFormat_RGB16F);
            this->finalColourBufferHDR->SetData(newWidth, newHeight, GTImage::ImageFormat_RGB16F);
            this->finalColourBuffer->SetData(   newWidth, newHeight, GTImage::ImageFormat_RGB8);

            Renderer::PushTexture2DData(*this->depthStencilBuffer);
            Renderer::PushTexture2DData(*this->opaqueColourBuffer);
            Renderer::PushTexture2DData(*this->lightingBuffer0);
            Renderer::PushTexture2DData(*this->lightingBuffer1);
            Renderer::PushTexture2DData(*this->finalColourBufferHDR);
            Renderer::PushTexture2DData(*this->finalColourBuffer);


            unsigned int bloomWidth  = GTCore::Max(1U, newWidth  / 4);
            unsigned int bloomHeight = GTCore::Max(1U, newHeight / 4);
            this->bloomBuffer->SetData(bloomWidth, bloomHeight, GTImage::ImageFormat_RGB8);
            this->bloomBlurBuffer->SetData(bloomWidth, bloomHeight, GTImage::ImageFormat_RGB8);

            Renderer::PushTexture2DData(*this->bloomBuffer);
            Renderer::PushTexture2DData(*this->bloomBlurBuffer);
        }


    private:    // No copying.
        DefaultSceneRendererFramebuffer(const DefaultSceneRendererFramebuffer &);
        DefaultSceneRendererFramebuffer & operator=(const DefaultSceneRendererFramebuffer &);
    };


    /// The framebuffer for shadow maps.
    struct DefaultSceneRendererShadowFramebuffer
    {
        /// A pointer to the main framebuffer object.
        Framebuffer* framebuffer;

        /// The depth/stencil buffer.
        Texture2D* depthStencilBuffer;

        /// The main colour output buffer (RG32F)
        Texture2D* colourBuffer;

        /// The buffer for helping perform the blurring step.
        Texture2D* blurBuffer;


        /// The width of the framebuffer.
        unsigned int width;

        /// The height of the framebuffer.
        unsigned int height;


        /// Constructor.
        DefaultSceneRendererShadowFramebuffer(unsigned int widthIn, unsigned int heightIn)
            : framebuffer(nullptr), depthStencilBuffer(nullptr), colourBuffer(nullptr), blurBuffer(nullptr),
              width(widthIn), height(heightIn)
        {
            this->framebuffer        = Renderer::CreateFramebuffer();
            this->depthStencilBuffer = Renderer::CreateTexture2D();
            this->colourBuffer       = Renderer::CreateTexture2D();
            this->blurBuffer         = Renderer::CreateTexture2D();

            // We just resize to setup the texture formats and whatnot.
            this->Resize(widthIn, heightIn);

            // Now we can setup the filtering and attach the textures to the framebuffer itself.
            Renderer::SetTexture2DFilter(  *this->colourBuffer, TextureFilter_Linear,  TextureFilter_Linear);
            Renderer::SetTexture2DFilter(  *this->blurBuffer,   TextureFilter_Nearest, TextureFilter_Nearest);
            Renderer::SetTexture2DWrapMode(*this->colourBuffer, TextureWrapMode_ClampToEdge);
            Renderer::SetTexture2DWrapMode(*this->blurBuffer,   TextureWrapMode_ClampToEdge);

            this->framebuffer->AttachDepthStencilBuffer(this->depthStencilBuffer);
            this->framebuffer->AttachColourBuffer(this->colourBuffer, 0);
            this->framebuffer->AttachColourBuffer(this->blurBuffer,   1);

            Renderer::PushAttachments(*this->framebuffer);
        }

        /// Destructor.
        ~DefaultSceneRendererShadowFramebuffer()
        {
            Renderer::DeleteTexture2D(this->blurBuffer);
            Renderer::DeleteTexture2D(this->colourBuffer);
            Renderer::DeleteTexture2D(this->depthStencilBuffer);
            Renderer::DeleteFramebuffer(this->framebuffer);
        }




        /// Resizes the attachments on the framebuffer.
        void Resize(unsigned int newWidth, unsigned int newHeight)
        {
            this->width  = newWidth;
            this->height = newHeight;

            this->depthStencilBuffer->SetData(newWidth, newHeight, GTImage::ImageFormat_Depth24_Stencil8);
            this->colourBuffer->SetData(      newWidth, newHeight, GTImage::ImageFormat_RG32F);
            this->blurBuffer->SetData(        newWidth, newHeight, GTImage::ImageFormat_RG32F);

            Renderer::PushTexture2DData(*this->depthStencilBuffer);
            Renderer::PushTexture2DData(*this->colourBuffer);
            Renderer::PushTexture2DData(*this->blurBuffer);
        }


    private:    // No copying.
        DefaultSceneRendererShadowFramebuffer(const DefaultSceneRendererShadowFramebuffer &);
        DefaultSceneRendererShadowFramebuffer & operator=(const DefaultSceneRendererShadowFramebuffer &);
    };

    /// The framebuffer for point light shadow maps.
    struct DefaultSceneRendererPointShadowFramebuffer
    {
        /// A pointer to the main framebuffer.
        Framebuffer* framebuffer;

        /// The depth/stencil buffer. This is cleared and reused for each face.
        Texture2D* depthStencilBuffer;

        /// The cube map that will contain the shadow information. RG32F.
        TextureCube* colourBuffer;

        /// Buffer for performing the blur stage.
        Texture2D* blurBuffer0;
        Texture2D* blurBuffer1;


        /// The width of each face of the framebuffer.
        unsigned int width;

        /// The height of each face of the framebuffer.
        unsigned int height;



        /// Constructor.
        DefaultSceneRendererPointShadowFramebuffer(unsigned int widthIn, unsigned int heightIn)
            : framebuffer(nullptr), depthStencilBuffer(nullptr), colourBuffer(nullptr), blurBuffer0(nullptr), blurBuffer1(nullptr),
              width(widthIn), height(heightIn)
        {
            this->framebuffer        = Renderer::CreateFramebuffer();
            this->depthStencilBuffer = Renderer::CreateTexture2D();
            this->colourBuffer       = Renderer::CreateTextureCube();
            this->blurBuffer0        = Renderer::CreateTexture2D();
            this->blurBuffer1        = Renderer::CreateTexture2D();

            // We just call Resize() to get the data setup.
            this->Resize(widthIn, heightIn);


            // Filters.
            Renderer::SetTextureCubeFilter(*this->colourBuffer, TextureFilter_Linear,  TextureFilter_Linear);
            Renderer::SetTexture2DFilter(  *this->blurBuffer0,  TextureFilter_Nearest, TextureFilter_Nearest);
            Renderer::SetTexture2DFilter(  *this->blurBuffer1,  TextureFilter_Nearest, TextureFilter_Nearest);

            // Wrapping modes.
            Renderer::SetTextureCubeWrapMode(*this->colourBuffer, TextureWrapMode_ClampToEdge);
            Renderer::SetTexture2DWrapMode(  *this->blurBuffer0,  TextureWrapMode_ClampToEdge);
            Renderer::SetTexture2DWrapMode(  *this->blurBuffer1,  TextureWrapMode_ClampToEdge);


            // Attach.
            this->framebuffer->AttachDepthStencilBuffer(this->depthStencilBuffer);
            this->framebuffer->AttachColourBuffer(this->colourBuffer->PositiveX, 0);
            this->framebuffer->AttachColourBuffer(this->colourBuffer->NegativeX, 1);
            this->framebuffer->AttachColourBuffer(this->colourBuffer->PositiveY, 2);
            this->framebuffer->AttachColourBuffer(this->colourBuffer->NegativeY, 3);
            this->framebuffer->AttachColourBuffer(this->colourBuffer->PositiveZ, 4);
            this->framebuffer->AttachColourBuffer(this->colourBuffer->NegativeZ, 5);
            this->framebuffer->AttachColourBuffer(this->blurBuffer0,             6);
            this->framebuffer->AttachColourBuffer(this->blurBuffer1,             7);

            Renderer::PushAttachments(*this->framebuffer);
        }

        /// Destructor.
        ~DefaultSceneRendererPointShadowFramebuffer()
        {
            Renderer::DeleteTexture2D(this->blurBuffer0);
            Renderer::DeleteTexture2D(this->blurBuffer1);
            Renderer::DeleteTextureCube(this->colourBuffer);
            Renderer::DeleteTexture2D(this->depthStencilBuffer);
            Renderer::DeleteFramebuffer(this->framebuffer);
        }


        /// Resizes the attachments on the framebuffer.
        void Resize(unsigned int newWidth, unsigned int newHeight)
        {
            this->width  = newWidth;
            this->height = newHeight;

            this->depthStencilBuffer->SetData(newWidth, newHeight, GTImage::ImageFormat_Depth24_Stencil8);
            this->colourBuffer->PositiveX->SetData(newWidth, newHeight, GTImage::ImageFormat_RG32F);
            this->colourBuffer->NegativeX->SetData(newWidth, newHeight, GTImage::ImageFormat_RG32F);
            this->colourBuffer->PositiveY->SetData(newWidth, newHeight, GTImage::ImageFormat_RG32F);
            this->colourBuffer->NegativeY->SetData(newWidth, newHeight, GTImage::ImageFormat_RG32F);
            this->colourBuffer->PositiveZ->SetData(newWidth, newHeight, GTImage::ImageFormat_RG32F);
            this->colourBuffer->NegativeZ->SetData(newWidth, newHeight, GTImage::ImageFormat_RG32F);
            this->blurBuffer0->SetData(newWidth, newHeight, GTImage::ImageFormat_RG32F);
            this->blurBuffer1->SetData(newWidth, newHeight, GTImage::ImageFormat_RG32F);

            Renderer::PushTexture2DData(  *this->depthStencilBuffer, 0);
            Renderer::PushTextureCubeData(*this->colourBuffer);
            Renderer::PushTexture2DData(  *this->blurBuffer0);
            Renderer::PushTexture2DData(  *this->blurBuffer1);
        }


    private:    // No copying.
        DefaultSceneRendererPointShadowFramebuffer(const DefaultSceneRendererPointShadowFramebuffer &);
        DefaultSceneRendererPointShadowFramebuffer & operator=(const DefaultSceneRendererPointShadowFramebuffer &);
    };



    /// Structure containing the shaders associated with a material.
    struct DefaultSceneRendererMaterialShaders
    {
        DefaultSceneRendererMaterialShaders()
            : ambientLightShader(nullptr),
              directionalLightShader(nullptr),
              pointLightShader(nullptr),
              spotLightShader(nullptr),
              shadowDirectionalLightShader(nullptr),
              shadowPointLightShader(nullptr),
              shadowSpotLightShader(nullptr),
              materialShader(nullptr)
        {
        }

        ~DefaultSceneRendererMaterialShaders()
        {
            Renderer::DeleteShader(this->ambientLightShader);
            Renderer::DeleteShader(this->directionalLightShader);
            Renderer::DeleteShader(this->pointLightShader);
            Renderer::DeleteShader(this->spotLightShader);
            Renderer::DeleteShader(this->shadowDirectionalLightShader);
            Renderer::DeleteShader(this->shadowPointLightShader);
            Renderer::DeleteShader(this->shadowSpotLightShader);
            Renderer::DeleteShader(this->materialShader);
        }


        /// The shader to use when doing an ambient light pass.
        Shader* ambientLightShader;

        /// The shader to use when doing a directional light pass.
        Shader* directionalLightShader;

        /// The shader to use when doing a point light pass.
        Shader* pointLightShader;

        /// The shader to use when doing a spot light pass.
        Shader* spotLightShader;


        /// The shader to use when doing a shadow directional light pass.
        Shader* shadowDirectionalLightShader;

        /// The shader to use when doing a shadow point light pass.
        Shader* shadowPointLightShader;

        /// The shader to use when doing a shadow spot light pass.
        Shader* shadowSpotLightShader;


        /// The shader to use when doing the material pass.
        Shader* materialShader;


    private:    // No copying.
        DefaultSceneRendererMaterialShaders(const DefaultSceneRendererMaterialShaders &);
        DefaultSceneRendererMaterialShaders & operator=(const DefaultSceneRendererMaterialShaders &);
    };



    /// Class representing the default scene renderer.
    class DefaultSceneRenderer : public SceneRenderer
    {
    public:

        /// Constructor.
        DefaultSceneRenderer();

        /// Destructor.
        virtual ~DefaultSceneRenderer();



        ////////////////////////////////////////////////////////////////
        // Virtual Methods.

        /// SceneRenderer::Begin().
        void Begin(Scene &scene);

        /// SceneRenderer::End().
        void End(Scene &scene);

        /// SceneRenderer::RenderViewport()
        void RenderViewport(Scene &scene, SceneViewport &viewport);

        /// SceneRenderer::AddViewport()
        void AddViewport(SceneViewport &viewport);

        /// SceneRenderer::RemoveViewport()
        void RemoveViewport(SceneViewport &viewport);

        /// SceneRenderer::OnViewportResized()
        void OnViewportResized(SceneViewport &viewport);

        /// SceneRenderer::AddExternalMesh()
        void AddExternalMesh(const SceneRendererMesh &meshToAdd);

        /// SceneRenderer::RemoveExternalMesh()
        void RemoveExternalMesh(const SceneRendererMesh &meshToRemove);



        ////////////////////////////////////////////////////////////////
        // Settings.

        /// Enables HDR.
        void EnableHDR();

        /// Disables HDR.
        void DisableHDR();

        /// Determines whether or not HDR is enabled.
        bool IsHDREnabled() const;


        /// Enables bloom.
        void EnableBloom();

        /// Disables bloom.
        void DisableBloom();

        /// Determines whether or not bloom is enabled.
        bool IsBloomEnabled() const;


        /// Sets the HDR exposure.
        void SetHDRExposure(float newExposure);

        /// Retrieves the HDR exposure.
        float GetHDRExposure() const;


        /// Sets the bloom factor.
        void SetBloomFactor(float newBloomFactor);

        /// Retrieves the bloom factor.
        float GetBloomFactor() const;



        ////////////////////////////////////////////////////////////////
        // Event Handlers from MaterialLibrary.

        /// Called when a material definition is deleted.
        void OnDeleteMaterialDefinition(MaterialDefinition &definition);

        /// Called when a material definition is reloaded.
        void OnReloadMaterialDefinition(MaterialDefinition &definition);




    private:

        /// Retrieves a pointer to the framebuffer of the given viewport.
        ///
        /// @param viewport [in] A reference to the viewport whose framebuffer is being retrieved.
        DefaultSceneRendererFramebuffer* GetViewportFramebuffer(SceneViewport &viewport);



        /////////////////////////
        // Rendering.

        /// Renders the opaque pass.
        void RenderOpaquePass(DefaultSceneRendererFramebuffer* framebuffer, const DefaultSceneRenderer_VisibilityProcessor &visibleObjects);

        /// Renders the depth pre-pass.
        ///
        /// @remarks
        ///     This method assumes the renderer state has been set beforehand.
        void RenderDepthPass(DefaultSceneRendererFramebuffer* framebuffer, const DefaultSceneRenderer_VisibilityProcessor &visibleObjects);
        void RenderDepthPass(DefaultSceneRendererFramebuffer* framebuffer, const DefaultSceneRenderer_VisibilityProcessor &visibleObjects, const GTCore::Vector<DefaultSceneRendererMesh> &meshes);

        /// Renders the opaque lighting pass.
        void RenderOpaqueLightingPass(DefaultSceneRendererFramebuffer* framebuffer, const DefaultSceneRenderer_VisibilityProcessor &visibleObjects);

        /// Renders the opaque material pass.
        void RenderOpaqueMaterialPass(DefaultSceneRendererFramebuffer* framebuffer, const DefaultSceneRenderer_VisibilityProcessor &visibleObjects);
        void RenderOpaqueMaterialPass(DefaultSceneRendererFramebuffer* framebuffer, const DefaultSceneRenderer_VisibilityProcessor &visibleObjects, const GTCore::Vector<DefaultSceneRendererMesh> &meshes);

        /// Performs an ambient lighting pass in the opaque pass.
        void RenderOpaqueAmbientLightingPass(size_t lightIndex, const DefaultSceneRenderer_VisibilityProcessor &visibleObjects);
        void RenderOpaqueAmbientLightingPass(size_t lightIndex, const DefaultSceneRenderer_VisibilityProcessor &visibleObjects, const GTCore::Vector<DefaultSceneRendererMesh> &meshes);

        /// Performs a directional lighting pass in the opaque pass.
        void RenderOpaqueDirectionalLightingPass(size_t lightIndex, const DefaultSceneRenderer_VisibilityProcessor &visibleObjects);
        void RenderOpaqueDirectionalLightingPass(size_t lightIndex, const DefaultSceneRenderer_VisibilityProcessor &visibleObjects, const GTCore::Vector<DefaultSceneRendererMesh> &meshes);

        /// Performs a point lighting pass in the opaque pass.
        void RenderOpaquePointLightingPass(size_t lightIndex, const DefaultSceneRenderer_VisibilityProcessor &visibleObjects);
        void RenderOpaquePointLightingPass(size_t lightIndex, const DefaultSceneRenderer_VisibilityProcessor &visibleObjects, const GTCore::Vector<DefaultSceneRendererMesh> &meshes);

        /// Performs a spot lighting pass in the opaque pass.
        void RenderOpaqueSpotLightingPass(size_t lightIndex, const DefaultSceneRenderer_VisibilityProcessor &visibleObjects);
        void RenderOpaqueSpotLightingPass(size_t lightIndex, const DefaultSceneRenderer_VisibilityProcessor &visibleObjects, const GTCore::Vector<DefaultSceneRendererMesh> &meshes);


        /// Performs a shadow-casting directional lighting pass in the opaque pass.
        void RenderOpaqueShadowDirectionalLightingPass(size_t lightIndex, const DefaultSceneRenderer_VisibilityProcessor &visibleObjects, DefaultSceneRendererFramebuffer* mainFramebuffer);
        void RenderOpaqueShadowDirectionalLightingPass(size_t lightIndex, const DefaultSceneRenderer_VisibilityProcessor &visibleObjects, const GTCore::Vector<DefaultSceneRendererMesh> &meshes);

        /// Performs a shadow-casting point lighting pass in the opaque pass.
        void RenderOpaqueShadowPointLightingPass(size_t lightIndex, const DefaultSceneRenderer_VisibilityProcessor &visibleObjects, DefaultSceneRendererFramebuffer* mainFramebuffer);
        void RenderOpaqueShadowPointLightingPass(size_t lightIndex, const DefaultSceneRenderer_VisibilityProcessor &visibleObjects, const GTCore::Vector<DefaultSceneRendererMesh> &meshes);
        void RenderPointShapowMapFace(const DefaultSceneRendererPointLight &light, const glm::mat4 &faceViewMatrix, int faceIndex, const GTCore::Vector<DefaultSceneRendererMesh> &meshes);

        /// Performs a shadow-casting spot lighting pass in the opaque pass.
        void RenderOpaqueShadowSpotLightingPass(size_t lightIndex, const DefaultSceneRenderer_VisibilityProcessor &visibleObjects, DefaultSceneRendererFramebuffer* mainFramebuffer);
        void RenderOpaqueShadowSpotLightingPass(size_t lightIndex, const DefaultSceneRenderer_VisibilityProcessor &visibleObjects, const GTCore::Vector<DefaultSceneRendererMesh> &meshes);


        /// Renders the alpha transparency pass.
        void RenderBlendedTransparentPass(DefaultSceneRendererFramebuffer* framebuffer, const DefaultSceneRenderer_VisibilityProcessor &visibleObjects);

        /// Renders the refractive transparent pass.
        void RenderRefractiveTransparentPass(DefaultSceneRendererFramebuffer* framebuffer, const DefaultSceneRenderer_VisibilityProcessor &visibleObjects);


        /// Renders the lighting of the given mesh.
        void RenderMeshLighting(const DefaultSceneRendererMesh &mesh, const DefaultSceneRenderer_VisibilityProcessor &visibleObjects);


        /// Renders the final composition.
        void RenderFinalComposition(DefaultSceneRendererFramebuffer* framebuffer, Texture2D* sourceColourBuffer);

        /// Renders the bloom map.
        void RenderBloomMap(DefaultSceneRendererFramebuffer* framebuffer, Texture2D* sourceColourBuffer);



        /////////////////////////
        // Materials.

        /// Retrieves the shaders of the given material.
        ///
        /// @param material [in] A reference to the material whose shaders are being retrieved.
        ///
        /// @remarks
        ///     If the structure has not yet been created, it will be created here. Note, this will not create the individual shaders. This will be handled by
        ///     the method responsible for retrieving those specific shaders.
        DefaultSceneRendererMaterialShaders* GetMaterialShaders(Material &material);

        /// Retrieves the shader to use for the ambient light pass.
        ///
        /// @param material [in] A reference to the material whose shader is being retrieved.
        Shader* GetMaterialAmbientLightShader(Material &material);

        /// Retrieves the shader to use for the directional light pass.
        ///
        /// @param material [in] A reference to the material whose shader is being retrieved.
        Shader* GetMaterialDirectionalLightShader(Material &materail);

        /// Retrieves the shader to use for the point light pass.
        ///
        /// @param material [in] A reference to the material whose shader is being retrieved.
        Shader* GetMaterialPointLightShader(Material &materail);

        /// Retrieves the shader to use for the spot light pass.
        ///
        /// @param material [in] A reference to the material whose shader is being retrieved.
        Shader* GetMaterialSpotLightShader(Material &materail);


        /// Retrieves the shader to use for the shadow directional light pass.
        ///
        /// @param material [in] A reference to the material whose shader is being retrieved.
        Shader* GetMaterialShadowDirectionalLightShader(Material &materail);

        /// Retrieves the shader to use for the shadow point light pass.
        ///
        /// @param material [in] A reference to the material whose shader is being retrieved.
        Shader* GetMaterialShadowPointLightShader(Material &materail);

        /// Retrieves the shader to use for the shadow spot light pass.
        ///
        /// @param material [in] A reference to the material whose shader is being retrieved.
        Shader* GetMaterialShadowSpotLightShader(Material &materail);


        /// Retrieves the material shader of the given material.
        ///
        /// @param material [in] A reference to the material whose material shader is being retrieved.
        Shader* GetMaterialMaterialShader(Material &material);



    private:

        /// The framebuffers for each attached viewport. Keyed by the viewport.
        GTCore::Map<SceneViewport*, DefaultSceneRendererFramebuffer*> viewportFramebuffers;

        /// Keeps track of the shaders associated with each referenced material definition. Keyed by the material definition.
        GTCore::Map<const MaterialDefinition*, DefaultSceneRendererMaterialShaders*> materialShaders;


        /// The shader to use with the depth pre-pass.
        Shader* depthPassShader;

        /// The list of external meshes.
        GTCore::Vector<const SceneRendererMesh*> externalMeshes;


        /// The framebuffer for drawing shadow maps.
        DefaultSceneRendererShadowFramebuffer shadowMapFramebuffer;

        /// The shader to use when building shadow maps.
        Shader* shadowMapShader;

        /// The framebuffer for drawing point light shadow maps.
        DefaultSceneRendererPointShadowFramebuffer pointShadowMapFramebuffer;

        /// The shader to use when building point light shadow maps.
        Shader* pointShadowMapShader;


        /// Vertex array for drawing a full screen triangle. This is in P3T2 format. The z coordinate is at 0.
        VertexArray* fullscreenTriangleVA;

        /// The shader to use when doing the final composition.
        Shader* finalCompositionShaderHDR;
        Shader* finalCompositionShaderHDRNoBloom;
        Shader* finalCompositionShaderLDR;

        /// The shader to use when doing the bloom map.
        Shader* bloomShader;

        /// Shader for doing highlights.
        Shader* highlightShader;


        /// The shader use for doing a gaussian blur.
        Shader* blurShaderX;
        Shader* blurShaderY;
        Shader* blurShaderX7x7;
        Shader* blurShaderY7x7;
        Shader* blurShaderX11x11;
        Shader* blurShaderY11x11;
        Shader* blurShaderX15x15;
        Shader* blurShaderY15x15;



        ///////////////////////////////////////////////////////
        // Settings.

        /// Keeps track of whether or not HDR is enabled.
        bool isHDREnabled;

        /// Keeps track of whether or not bloom is enabled.
        bool isBloomEnabled;

        /// Keeps track of the HDR exposure.
        float hdrExposure;

        /// Keeps track of the bloom factor.
        float bloomFactor;



        /// Material Library Event Handler.
        class MaterialLibraryEventHandler : public MaterialLibrary::EventHandler
        {
        public:

            /// Constructor.
            MaterialLibraryEventHandler(DefaultSceneRenderer &rendererIn)
                : renderer(rendererIn)
            {
            }


            /// MaterialLibrary::EventHandler::OnDeleteMaterialDefinition().
            void OnDeleteMaterialDefinition(MaterialDefinition &definition)
            {
                renderer.OnDeleteMaterialDefinition(definition);
            }

            /// MaterialLibrary::EventHandler::OnReloadMaterialDefinition().
            void OnReloadMaterialDefinition(MaterialDefinition &definition)
            {
                renderer.OnReloadMaterialDefinition(definition);
            }



            /// A reference to the renderer that owns this event handler.
            DefaultSceneRenderer &renderer;


        private:    // No copying.
            MaterialLibraryEventHandler(const MaterialLibraryEventHandler &);
            MaterialLibraryEventHandler & operator=(const MaterialLibraryEventHandler &);

        }materialLibraryEventHandler;


    private:    // No copying.
        DefaultSceneRenderer(const DefaultSceneRenderer &);
        DefaultSceneRenderer & operator=(const DefaultSceneRenderer &);
    };
}

#endif